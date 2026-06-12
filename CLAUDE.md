# CLAUDE.md

This file provides guidance to Claude Code (claude.ai/code) when working with code in this repository.

## 프로젝트 개요

반도체 회사 S-Semi의 샘플 생산주문관리 시스템. 팹리스 업체·연구기관에 공급하는 반도체 샘플의 주문·생산·출하 흐름을 콘솔 애플리케이션으로 관리한다.

- **플랫폼**: C++ Console App, Visual Studio 2026
- **아키텍처**: MVC 패턴 (Model / Controller / View 네임스페이스 분리)
- **데이터 영속성**: JSON 파일 기반 저장·불러오기 (애플리케이션 재시작 후에도 데이터 유지)
- **멀티스레딩**: 생산 처리는 FIFO 우선순위 큐 + 별도 스레드로 처리

## 빌드 및 실행

```
# Visual Studio 2026에서 솔루션 열기
SampleOrderSystem-hahagil.sln

# CLI 빌드 (Developer Command Prompt)
nuget restore SampleOrderSystem-hahagil.sln
msbuild SampleOrderSystem-hahagil.sln /p:Configuration=Release /p:Platform=x64

# 실행
.\x64\Release\SampleOrderSystem.exe

# 테스트
.\x64\Release\SampleOrderSystemTest.exe
```

## 도메인 및 기능 명세

주문 상태 전이, 핵심 계산 공식(수율·생산량·재고), 메인 메뉴 구조, 기능 요구사항은 **[PRD.md](PRD.md)** 를 참조한다.

- 주문 상태 머신 → PRD.md §3.3·§4
- 수율·생산량·재고 계산 공식 → PRD.md §3·§5.4·§5.3
- 메인 메뉴 및 기능 요구사항 → PRD.md §5
- 구현 단계 및 커밋 계획 → docs/PLAN.md

## 핵심 설계 결정

- `IRepository<T>`: add / update / remove / findById / findAll 인터페이스
- `ProductionController`: tick 기반 생산 (`cv_.wait_for(unitTick, stopFlag)` 루프), IClock 불필요
- `OrderController::approve()`: approveMutex_로 재고 확인 → 상태 전이 원자적 처리
- `Sample.stock`: 현재 재고 직접 관리 (출고 시 감소, 생산 완료 시 증가)

## 주의 사항

- `Sonnet` 모델 / Effort: Medium 으로 작업한다. Opus 모델 사용 시 과금 유의.
- 승인 로직에서 재고 확인 → 상태 전이는 **원자적으로** 처리해야 한다 (동시성 버그 방지).
- 생산 처리 스레드는 애플리케이션 종료 시 안전하게 종료(graceful shutdown)해야 한다.
- 데이터 영속성 저장 파일(`*.json`)은 `.gitignore`에 추가하지 않는다 — 테스트 픽스처로 활용.
