# Mission 2 — SampleOrderSystem 구현 플랜

## Context

미션 1에서 4개 PoC(MVC 스켈레톤, JSON 영속성, 데이터 모니터, 더미 생성기)를 완성했다.
미션 2는 이 PoC들을 통합하여 생산 스레드·출하·대시보드까지 갖춘 완전한 시스템을 구축하는 것이다.
PRD §8의 5단계(문서→Harness→Test→CleanCode→Commit) 순서를 준수한다.

---

## 레포지토리

`SampleOrderSystem-hahagil`  

---

## 프로젝트 구조

```
SampleOrderSystem-hahagil/
├── SampleOrderSystem-hahagil.sln        ← 솔루션 (프로젝트 2개 포함)
├── docs/
│   └── PLAN.md                        ← 이 파일
│
├── SampleOrderSystem/                 ← 메인 프로젝트
│   ├── SampleOrderSystem.vcxproj
│   ├── main.cpp
│   ├── Models/                        ← PoC#2 그대로 복사
│   │   ├── OrderStatus.h
│   │   ├── Sample.h
│   │   └── Order.h                    ← createdAt: std::time_t 유지
│   ├── Repositories/                  ← PoC#2 그대로 복사
│   │   ├── IRepository.h
│   │   ├── JsonUtil.h
│   │   ├── SampleRepository.h/.cpp
│   │   └── OrderRepository.h/.cpp
│   ├── Controllers/
│   │   ├── SampleController.h/.cpp    ← PoC#1 기반 + IRepository<Sample>& 주입
│   │   ├── OrderController.h/.cpp     ← PoC#1 기반 + IRepository<Order>& 주입
│   │   ├── ProductionController.h/.cpp ← 신규: FIFO 큐 + worker thread
│   │   └── ShipmentController.h/.cpp  ← 신규: CONFIRMED → RELEASE
│   └── Views/
│       ├── MainMenuView.h/.cpp        ← PoC#1 기반 + 메뉴 확장
│       ├── SampleView.h/.cpp          ← PoC#1 기반 유지
│       ├── OrderView.h/.cpp           ← PoC#1 기반 유지
│       ├── DashboardView.h/.cpp       ← 신규: 상태별 집계 + 재고 현황
│       ├── ProductionView.h/.cpp      ← 신규: 생산 큐 현황
│       └── ShipmentView.h/.cpp        ← 신규: 출하 처리
│
└── SampleOrderSystemTest/             ← Google Test 프로젝트
    ├── SampleOrderSystemTest.vcxproj
    ├── Stubs/
    │   ├── InMemorySampleRepository.h ← IRepository<Sample> 메모리 구현
    │   └── InMemoryOrderRepository.h  ← IRepository<Order> 메모리 구현
    └── Tests/
        ├── FormulaTest.cpp            ← 생산량 공식 단위 테스트
        ├── OrderControllerTest.cpp    ← 상태 전이 5경로 테스트
        └── ProductionControllerTest.cpp ← tick 완료(unitProductionTime=0) + graceful shutdown
```

---

## 핵심 설계

### ProductionController

생산 처리를 단위 생산 시간(UnitProductionTime)마다 tick 이벤트를 발생시켜 수량을 하나씩 올리는 방식으로 구현한다.

```
tick 1 (UnitProductionTime 후) → produced++ → 1/13
tick 2 (UnitProductionTime 후) → produced++ → 2/13
...
tick 13                        → produced++ → 13/13 → CONFIRMED
```

`cv_.wait_for()` 에 stopFlag 를 조건으로 전달하므로 shutdown 시 다음 tick 경계에서 즉시 탈출한다.
IClock 주입 없이도 테스트 가능: Sample.unitProductionTime = 0 이면 tick이 즉시 발생한다.

```cpp
class ProductionController {
public:
    ProductionController(IRepository<Order>&, IRepository<Sample>&);
    void start();
    void stop();                          // graceful: stopFlag + notify + join
    void enqueue(const Model::Order&);
    std::optional<Model::Order> currentOrder() const;
    std::vector<Model::Order>   queueSnapshot() const;

    static int calcProductionQty(int qty, double yield);
    // ceil(qty / (yield × 0.9))
private:
    void workerLoop();
    void processTick(Model::Order& order, int totalQty,
                     std::chrono::seconds tickDuration);
    // cv_.wait_for(tickDuration, [&]{ return stopFlag_; }) 루프
    // produced == totalQty 도달 시 order.status = CONFIRMED

    std::queue<Model::Order>     queue_;
    mutable std::mutex           mutex_;
    std::condition_variable      cv_;
    bool                         stopFlag_ = false;
    std::thread                  worker_;
    std::optional<Model::Order>  currentOrder_;
    int                          producedCount_ = 0;   // 진행률 표시용
};
```

### OrderController::approve — 원자적 처리

```cpp
bool approve(const std::string& orderId) {
    std::lock_guard<std::mutex> lock(approveMutex_);
    // findById → 재고 확인 → reduceStock or enqueue → update (DP-03)
}
```

`SampleRepository::update()`는 내부 mutex로 보호 → ProductionController가 addStock 시에도 안전.

### main.cpp 핵심 흐름

```cpp
SampleRepository sampleRepo("samples.json");
OrderRepository  orderRepo("orders.json");
SampleController     sampleCtrl(sampleRepo);
ProductionController prodCtrl(orderRepo, sampleRepo);
OrderController      orderCtrl(orderRepo, sampleCtrl, prodCtrl);
ShipmentController   shipCtrl(orderRepo, sampleCtrl);

prodCtrl.start();
// DP-02: PRODUCING 상태 주문 재투입 (앱 재시작 복구)
for (auto& o : orderRepo.findByStatus(PRODUCING)) prodCtrl.enqueue(o);

MainMenuView(sampleCtrl, orderCtrl, prodCtrl, shipCtrl).run();

prodCtrl.stop();   // P-07 graceful shutdown
```

### DashboardView 재고 집계

Sample.stock 필드를 직접 사용한다.
- 출고 누계 = CONFIRMED + RELEASE 상태 주문의 quantity 합산
- 입고 누계 = 현재 재고 + 출고 누계
- 별도 Model 필드 추가 없이 orders.json에서 역산

---

## 구현 단계 및 커밋 계획

각 Phase 내에 `← ✅` 표시된 커밋이 빌드·실행 가능한 체크포인트다.

### Phase 0 — 프로젝트 초기화
작업: GitHub 레포 생성, 솔루션/프로젝트 파일, 최소 main.cpp, 빈 테스트 파일
```
[docs] Add CLAUDE.md, PRD.md, and PLAN.md
[chore] Initialize VS2026 solution with main and test projects   ← ✅ (양쪽 프로젝트 빌드·실행)
```
- main.cpp: `std::cout << "SampleOrderSystem starting...\n"; return 0;`
- 테스트 프로젝트: stub `main()` 파일 (gtest NuGet은 Phase 2에서 구성)

### Phase 1 — Models + Repositories + Stubs
작업: PoC#2 코드 이식, InMemory stub 헤더 작성, main.cpp에 Repository 동작 확인 추가
```
[chore] Port Models and Repository layer from PoC #2
[test]  Add InMemorySampleRepository and InMemoryOrderRepository stubs
[chore] Verify repository round-trip in main.cpp                ← ✅ (JSON 읽기/쓰기 출력 확인)
```
- main.cpp: 샘플/주문 add → save → reload → count 출력으로 Repository 동작 육안 확인

### Phase 2 — Controller 스켈레톤 + 테스트 (컴파일 GREEN, 런타임 RED)
작업: Controller 헤더 + 빈 .cpp 추가(stub 반환값), 테스트 파일 작성
```
[feat] Add Controller skeletons with stub implementations
[test] Add unit tests for formula, state transitions, and production (runtime red)  ← ✅
```
- Controller stub: 메서드가 존재하지만 `return false;` / 빈 구현 → 컴파일 성공
- 테스트: 빌드 성공, 실행 시 assertion 실패 (RED 확인 가능)
- main.cpp: stub main 유지 (Controller 헤더 include 추가해도 빌드 유지)

### Phase 3 — Controllers 구현 (Green)
작업: SampleController, OrderController, ProductionController, ShipmentController 로직 완성
```
[feat] Implement SampleController and OrderController with repository injection
[feat] Add ProductionController with tick-based worker thread
[feat] Add ShipmentController for CONFIRMED to RELEASE transition
[test] All unit tests green                                      ← ✅ (테스트 전체 통과)
```
- ProductionController: `cv_.wait_for(tickDuration, stopFlag)` 루프, producedCount_ 추적
- main.cpp: 간단한 시나리오(샘플 등록 → 주문 → 승인) 출력으로 Controller 동작 확인

### Phase 4 — Views + main.cpp 기본 연결
작업: 신규 View 3개 추가, MainMenuView 확장, main.cpp 메뉴 동작
```
[feat] Add DashboardView with order status summary and stock overview
[feat] Add ProductionView for queue status and produced count display
[feat] Add ShipmentView for CONFIRMED to RELEASE processing
[feat] Extend MainMenuView with all menu entries
[feat] Wire all layers in main.cpp for basic menu operation      ← ✅ (메뉴 6개 모두 진입 가능)
```

### Phase 5 — 통합 완성 (PRODUCING 복구 + graceful shutdown)
작업: 앱 재시작 시 PRODUCING 주문 재투입, 종료 시 worker thread 안전 종료
```
[feat] Add PRODUCING order recovery on application restart
[feat] Add graceful shutdown sequence for ProductionController   ← ✅ (전체 시나리오 동작)
```

### Phase 6 — CleanCode + 마무리
작업: 중복 제거, 네이밍 통일, DoD 체크리스트 점검
```
[refactor] Remove duplicated code and unify naming across layers
[docs] Update PLAN.md with final implementation notes            ← ✅ (PRD DoD 전체 통과)
```

---

## Subagent 병렬화 전략

의존성 구조가 단방향(Model→Repository→Controller→View)이라 각 Phase는 순차 진행이 기본이다.
다음 두 지점에서만 병렬화가 실질적으로 효과적이다.

| 시점 | Agent A | Agent B |
|------|---------|---------|
| Phase 1 | Models + Repositories 이식 | InMemory Stub 작성 |
| Phase 3 | ProductionController (독립 로직) | SampleCtrl + OrderCtrl |
| Phase 4 | DashboardView + ProductionView | ShipmentView |

---

## 검증 방법

| Phase | 검증 |
|-------|------|
| 0 | `msbuild` 양쪽 프로젝트 빌드 성공, 메인 .exe 실행 가능 |
| 1 | main.cpp 실행 시 JSON 파일 생성·재로드 출력 확인 |
| 2 | 테스트 빌드 성공, 실행 시 assertion 실패(RED) 확인 |
| 3 | `SampleOrderSystemTest.exe` 전체 GREEN, main.cpp 시나리오 출력 확인 |
| 4 | 메뉴 6개 모두 진입 가능, 대시보드 숫자 확인 |
| 5 | 전체 시나리오: 등록→주문→승인(재고 부족)→생산 완료→출하→재시작 후 보존 |
| 6 | PRD DoD 체크리스트 전체 통과 |

### PRODUCING 복구 시나리오
1. 주문 승인 → PRODUCING 상태 → `orders.json` 확인
2. 앱 강제 종료 → 재시작
3. 생산 현황 메뉴에서 해당 주문이 큐에 재투입됨 확인
4. 생산 완료 → CONFIRMED 전환 확인
