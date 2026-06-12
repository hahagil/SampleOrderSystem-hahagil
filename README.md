# SampleOrderSystem

반도체 회사 S-Semi의 샘플 생산주문관리 콘솔 애플리케이션.  
팹리스 업체·연구기관에 공급하는 반도체 샘플의 주문·생산·출하 흐름을 관리한다.

## 아키텍처

```
View → Controller → Repository → Model
```

- **MVC 패턴**: 네임스페이스 단위 레이어 분리, 단방향 의존성
- **생산 처리**: FIFO 큐 + 별도 worker thread (`ProductionController`)
- **데이터 영속성**: JSON 파일 (`samples.json` / `orders.json`)
- **동시성**: 승인 로직의 재고 확인 → 상태 전이를 mutex로 원자적 처리

## 주문 상태 전이

```
RESERVED
  ├─ 승인 + 재고 충분  →  CONFIRMED  →  RELEASE
  ├─ 승인 + 재고 부족  →  PRODUCING  →  CONFIRMED  →  RELEASE
  └─ 거절             →  REJECTED
```

## 빌드

Visual Studio 2022/2026 또는 MSBuild (x64):

```
msbuild SampleOrderSystem-hahagil.sln /p:Configuration=Release /p:Platform=x64
```

> Google Test는 NuGet 패키지로 관리된다. 첫 빌드 전 `nuget restore` 또는 Visual Studio에서 패키지 복원이 필요하다.

## 실행

```
# 메인 애플리케이션
.\x64\Release\SampleOrderSystem.exe

# 단위 테스트
.\x64\Release\SampleOrderSystemTest.exe

# 더미 데이터 생성 (샘플 5개, 주문 10개)
.\x64\Release\DummyDataGenerator.exe

# 더미 데이터 생성 (수량 지정)
.\x64\Release\DummyDataGenerator.exe --samples 3 --orders 5

# 데이터 모니터링 (별도 실행)
.\x64\Release\DataMonitor.exe
```

## 메뉴 구조

```
1. 샘플 관리    — 등록 / 목록 / 검색
2. 주문 관리    — 주문 생성 / 승인 / 거절
3. 대시보드     — 상태별 주문 수 + 샘플별 재고 현황
4. 생산 현황    — 현재 생산 중인 주문 + 대기 큐
5. 출하 처리    — CONFIRMED 주문 출하 (→ RELEASE)
0. 종료
```

## 검증 시나리오 (더미 데이터 활용)

```
1. DummyDataGenerator.exe 실행  →  DUM-001~005 샘플 + RESERVED 주문 10개 생성
2. SampleOrderSystem.exe 실행
3. 메뉴 3 (대시보드)            →  샘플별 재고·누계 확인
4. 메뉴 2 (주문 관리) → 승인    →  재고 충분 시 CONFIRMED / 부족 시 PRODUCING 자동 전환
5. 메뉴 4 (생산 현황)           →  진행률 확인
6. 생산 완료 후 메뉴 5 (출하)   →  CONFIRMED 주문 선택 → RELEASE 전환
7. 앱 재시작                    →  PRODUCING 주문 자동 복구 확인
```

> 생산 속도는 샘플 등록 시 입력한 `단위 생산 시간(분/개)`에 따라 결정된다.  
> 빠른 테스트가 필요하면 단위 시간 1분 + 소량 주문을 권장한다.

## 생산량 공식

```
총 생산량 = ceil(주문수량 / (수율 × 0.9))
총 생산시간 = 단위생산시간(분) × 총 생산량
```

예) 수율 0.9, 주문 10개, 단위 1분 → `ceil(10 / 0.81) = 13개`, 13분 소요

## 프로젝트 구성

| 프로젝트 | 설명 |
|---------|------|
| `SampleOrderSystem` | 메인 애플리케이션 |
| `SampleOrderSystemTest` | Google Test 단위 테스트 (18개) |
| `DataMonitor` | JSON 파일 읽기 전용 모니터링 도구 |
| `DummyDataGenerator` | 테스트용 샘플·주문 더미 데이터 생성기 |
