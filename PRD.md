# PRD.md — 반도체 샘플 생산주문관리 시스템

## 1. 배경 및 목적

반도체 회사 S-Semi는 다양한 종류의 반도체 샘플을 팹리스 업체·연구기관에 공급한다.  
현재 주문 관리가 메모리(구두·메모) 방식으로 이뤄져 주문 누락·중복·현황 파악 불가 문제가 발생하고 있다.  
이를 해결하기 위해 샘플 등록부터 주문·생산·출하까지 전 흐름을 추적하는 콘솔 기반 관리 시스템을 구축한다.

---

## 2. 범위

| 포함 | 제외 |
|------|------|
| 샘플 등록·조회·검색 | 사용자 인증·권한 관리 |
| 주문 생성·승인·거절 | 외부 API 연동 |
| 생산 처리 (자동 생산량 계산, FIFO 큐) | GUI / 웹 UI |
| 출하 처리 | 결제·청구 |
| 대시보드 (재고·주문 현황) | 다중 사용자 동시 접속 |
| 데이터 영속성 (JSON) | |
| 데이터 모니터링 외부 도구 | |
| Dummy 데이터 생성 도구 | |

---

## 3. 핵심 도메인 모델

### 3.1 Sample (샘플)

| 필드 | 타입 | 설명 |
|------|------|------|
| SampleId | string | 시스템 고유 식별자 |
| Name | string | 샘플 이름 |
| UnitProductionTime | int | 단위 생산 시간 (분/개) |
| Yield | double | 수율 (0.0 ~ 1.0) |

**수율 계산**: `Yield = 불량없는 샘플 수 / 총 생산 샘플 수`  
예) 100개 생산 후 양품 90개 → `Yield = 0.9`

### 3.2 Order (주문)

| 필드 | 타입 | 설명 |
|------|------|------|
| OrderId | string | 시스템 고유 식별자 |
| SampleId | string | 주문 대상 샘플 ID |
| Quantity | int | 주문 수량 |
| Status | OrderStatus | 주문 상태 |
| CreatedAt | DateTime | 주문 생성 시각 |

### 3.3 OrderStatus (주문 상태)

```
RESERVED   주문 생성 직후 대기 상태
PRODUCING  승인 시 재고 부족 → 자동 생산 시작
CONFIRMED  승인 시 재고 충분 OR 생산 완료
RELEASE    출하 완료
REJECTED   주문 거절 (이후 모든 처리에서 제외)
```

---

## 4. 주문 상태 전이

```
[생성]
  └─ RESERVED
       ├─ 승인 + 재고 충분  ──→ CONFIRMED ──→ RELEASE
       ├─ 승인 + 재고 부족  ──→ PRODUCING ──→ CONFIRMED ──→ RELEASE
       └─ 거절             ──→ REJECTED
```

**재고 판단 기준**: 승인 시점에 현재 재고 ≥ 주문 수량이면 재고 충분.

---

## 5. 기능 요구사항

### 5.1 샘플 관리

| ID | 요구사항 | 우선순위 |
|----|---------|---------|
| S-01 | 새로운 샘플을 시스템에 등록한다 (SampleId, Name, UnitProductionTime, Yield 입력) | Must |
| S-02 | 등록된 전체 샘플 목록을 조회한다 (현재 재고 포함) | Must |
| S-03 | 이름 또는 속성으로 특정 샘플을 검색한다 | Must |

### 5.2 주문

| ID | 요구사항 | 우선순위 |
|----|---------|---------|
| O-01 | 등록된 샘플 ID·수량을 입력하여 주문을 생성한다 (초기 상태: RESERVED) | Must |
| O-02 | RESERVED 주문 목록을 표시하고 특정 주문을 승인한다 | Must |
| O-03 | 승인 시 재고 충분하면 즉시 CONFIRMED로 전환한다 | Must |
| O-04 | 승인 시 재고 부족하면 자동으로 생산을 시작하고 PRODUCING으로 전환한다 | Must |
| O-05 | 특정 주문을 거절하여 REJECTED로 전환한다 | Must |

### 5.3 대시보드

| ID | 요구사항 | 우선순위 |
|----|---------|---------|
| D-01 | 상태별(RESERVED / CONFIRMED / PRODUCING / RELEASE) 주문 목록을 표시한다 (REJECTED 제외) | Must |
| D-02 | 샘플별 재고를 표시한다 (입고 누계 / 출고 누계 / 현재 재고) | Must |
| D-03 | 주문 순서에 따라 입고·출고·재고를 누적하여 표시한다 | Must |

### 5.4 생산 처리

| ID | 요구사항 | 우선순위 |
|----|---------|---------|
| P-01 | 총 생산량을 자동 계산한다: `ceil(주문수량 / (수율 × 0.9))` | Must |
| P-02 | 총 생산 시간을 계산한다: `UnitProductionTime × 총 생산량` | Must |
| P-03 | 생산은 FIFO 우선순위 큐로 처리한다 | Must |
| P-04 | 생산 처리는 별도 스레드에서 실행된다 | Must |
| P-05 | 생산 완료 시 주문 상태를 PRODUCING → CONFIRMED로 전환한다 | Must |
| P-06 | 현재 생산 중인 주문 및 대기 큐를 화면에 표시한다 | Must |
| P-07 | 애플리케이션 종료 시 생산 스레드를 안전하게 종료(graceful shutdown)한다 | Must |

### 5.5 출하 처리

| ID | 요구사항 | 우선순위 |
|----|---------|---------|
| SH-01 | CONFIRMED 상태 주문 목록을 표시한다 | Must |
| SH-02 | 특정 주문을 선택하여 출하 처리한다 (→ RELEASE) | Must |

### 5.6 데이터 영속성

| ID | 요구사항 | 우선순위 |
|----|---------|---------|
| DP-01 | 샘플·주문 데이터를 JSON 파일로 저장한다 | Must |
| DP-02 | 애플리케이션 시작 시 저장된 데이터를 자동으로 불러온다 | Must |
| DP-03 | 상태 변경 즉시 파일에 반영한다 (in-memory와 동기화) | Must |

### 5.7 데이터 모니터링 도구 (외부 프로그램)

| ID | 요구사항 | 우선순위 |
|----|---------|---------|
| M-01 | 별도 실행 가능한 프로그램으로 제공한다 | Must |
| M-02 | 메인 시스템의 JSON 파일(또는 공유 저장소)을 읽어 실시간 현황을 표시한다 | Must |
| M-03 | 주문 상태별 집계 및 샘플별 재고를 출력한다 | Must |

### 5.8 Dummy 데이터 생성 도구

| ID | 요구사항 | 우선순위 |
|----|---------|---------|
| DG-01 | 별도 실행 가능한 프로그램으로 제공한다 | Must |
| DG-02 | 테스트용 샘플·주문 Dummy Data를 생성하여 JSON(또는 DB)에 삽입한다 | Must |
| DG-03 | 생성할 데이터 수량을 파라미터로 지정할 수 있다 | Should |

---

## 6. 비기능 요구사항

| 항목 | 요구사항 |
|------|---------|
| 영속성 | 애플리케이션 재시작 후에도 모든 데이터가 보존된다 |
| 동시성 | 승인 로직의 재고 확인 → 상태 전이는 원자적으로 처리한다 (lock 사용) |
| 확장성 | Repository 레이어를 인터페이스로 추상화하여 저장소 교체가 가능하다 |
| 유지보수 | MVC 레이어 간 직접 참조를 금지한다 (View → Controller → Model 단방향) |

---

## 7. 프로젝트 구조 (예상)

```
SampleOrderSystem/
├── Models/
│   ├── Sample.cs
│   ├── Order.cs
│   └── OrderStatus.cs          # enum
├── Repositories/
│   ├── IRepository.cs          # 제네릭 인터페이스
│   ├── SampleRepository.cs     # samples.json
│   └── OrderRepository.cs      # orders.json
├── Controllers/
│   ├── SampleController.cs
│   ├── OrderController.cs      # 승인/거절 로직, 재고 판단
│   ├── ProductionController.cs # FIFO 큐, 생산 스레드
│   └── ShipmentController.cs
├── Views/
│   ├── MainMenuView.cs
│   ├── SampleView.cs
│   ├── OrderView.cs
│   ├── DashboardView.cs
│   ├── ProductionView.cs
│   └── ShipmentView.cs
└── Program.cs
```

---

## 8. 미션 진행 순서

### 미션 1 — PoC (각 항목 별도 GitHub Public Repository)

| 순서 | Repository 이름 형식 | 핵심 구현 |
|------|---------------------|----------|
| 1 | `ConsoleMVC-hahagil` | Model·Controller·View 네임스페이스 분리, 콘솔 메뉴 루프 |
| 2 | `DataPersistence-hahagil` | JSON 저장·불러오기, CRUD 구현, IRepository 인터페이스 |
| 3 | `DataMonitor-hahagil` | 공유 JSON 파일 읽기, 실시간 현황 출력 |
| 4 | `DummyDataGenerator-hahagil` | Dummy Sample·Order 생성, JSON 삽입 |

### 미션 2 — 최종 프로젝트 (`SampleOrderSystem-이니셜-번호`)

```
1. 문서 준비   CLAUDE.md + PRD.md 완성
2. Harness     테스트 픽스처·헬퍼 구성 (Dummy 데이터, In-memory Repository stub)
3. Test        단위 테스트 작성 (상태 전이, 생산량 공식, 재고 계산)
4. CleanCode   중복 제거, 네이밍 통일, 레이어 의존성 정리
5. Commit      단계별 의미 있는 커밋 (글로벌 커밋 컨벤션 준수)
```

---

## 9. 승인 기준 (Definition of Done)

- [ ] 모든 Must 요구사항 구현 완료
- [ ] 주문 상태 전이 5가지 경로 모두 테스트 통과
- [ ] 애플리케이션 재시작 후 데이터 보존 확인
- [ ] 생산 스레드 graceful shutdown 확인
- [ ] 데이터 모니터링 도구 별도 실행 확인
- [ ] Dummy 데이터 생성 도구 별도 실행 확인
- [ ] 각 PoC Repository GitHub Public 공개 확인
- [ ] 최종 Repository GitHub Public 공개 확인
