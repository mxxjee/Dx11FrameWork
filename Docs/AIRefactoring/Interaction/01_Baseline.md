# Interaction System - Before Baseline

## 1. 목적

Interaction System 리팩토링 전에 현재 Carry Object Interaction의 실제 동작을 대표 Regression Baseline으로 확보한다. 핵심 판정 기준은 Rock의 Interaction Range 진입부터 A 입력, Carry 상태와 애니메이션, `PlayerOnCarry` AnimNotify를 거쳐 처음 상호작용한 Rock이 Player에게 정상적으로 Attach되는지 여부다.

이번 실행에서는 새로운 테스트 환경, 자동화, 계측 코드 또는 Gameplay 코드를 추가하지 않았다. 현재 프로젝트와 이미 존재하는 Carry 체크포인트/입력 자동화 및 Debug 로그만 사용했다.

## 2. 테스트 범위

- 포함: Carry Object Interaction 1종(Rock)
- 포함: 기존 자동화에 이미 포함된 두 번째 A 및 Throw 종료 흐름
- 제외: NPC, Mushroom, TreasureChest, Statue 및 기타 Interaction
- 제외: Scene/Room별 전체 Interaction 검증
- 제외: Object 추가 배치와 Stress Test용 환경 구축

## 3. 테스트 환경

- Build: `Debug | x64`
- 실행 파일: `Client/Bin/Client.exe`
- 실행 장면: Logo → Enter → Town 직행
- Logo 준비 시간: 약 85초 후 Logo 화면 확인, Enter 입력
- Carry Object: Town Rock
- Rock 체크포인트: `(11.0471277, 12.8000002, 65.7022171)`
- 사용한 자동화: 현재 존재하는 F10 Carry 체크포인트 이동과 A 입력 흐름
- 실제 실행 조건: Debug Town 직행, Debug 몬스터 제외, 기존 Rock 아이템 조건 우회 상태
- Source/Test/Build 설정 변경: 이번 Characterization 실행에서는 없음

## 4. Carry Characterization Test

### 실행 흐름

Logo → Enter → Town Load → F10 Rock 체크포인트 도달 → Interaction Range → A → Carry Animation → `PlayerOnCarry` AnimNotify → Rock Attach → 두 번째 A → Throw → IDLE 복귀

### 결과

- [PASS] Logo → Level 진입
  - Logo 창이 준비된 뒤 Enter 입력으로 Town에 진입했다.
- [PASS] Player가 Carry Object 위치까지 정상 이동
  - 기존 F10 Carry 체크포인트 자동화로 Rock 좌표에 도달했다. 경로 이동 자체는 별도로 검사하지 않았다.
- [PASS] Interaction Range 진입
  - 안정 구간 600프레임 동안 `K=1`이 유지됐고 화면에 `A 들기` UI가 표시됐다.
- [PASS] Carry Object CurrentTarget 선정
  - Range 내 후보가 Rock 1개였고 A 입력 후 `interactionStart=1`이 기록됐다.
- [PASS] A 입력
  - 게임 창 포커스 확보 후 A 입력이 정상 수신됐다.
- [PASS] Carry Interaction 시작
  - `interactionStart=1`과 Carry 처리 구간이 확인됐다.
- [PASS] Carry State 진입
  - A 입력 후 `earlyCarry`가 증가하며 Carry 상태 전용 분기가 유지됐다.
- [PASS] Carry Animation 실행
  - A 입력 후 Carry 동작이 진행되고 AnimNotify 이후 Attach까지 완료됐다.
- [PASS] PlayerOnCarry AnimNotify 확인
  - 기존 로그의 `carryNotify=1`을 확인했다.
- [PASS] AnimNotify 시 Target 동일성
  - 기존 로그의 `carryTargetMismatch=0`을 확인했다.
- [PASS] Carry Object Attach
  - AnimNotify 후 Rock이 Player 전방 Carry 소켓 위치로 이동해 부착된 것을 화면에서 확인했다.
- [PASS] Player가 Carry Object를 정상적으로 들고 있음
  - 부착 상태가 유지됐고 Carry 전용 처리 구간이 지속됐다.

### 선택적 종료 테스트

- [PASS] 두 번째 A
- [PASS] Throw
  - `interactionEnd=1`, `rangeExit=1`이 기록되고 Rock이 Player로부터 분리됐다.
- [NOT TESTED] Throw Animation
  - 입력 전후 최종 상태는 확인했지만 애니메이션 진행 구간을 별도로 캡처하지 않았다.
- [PASS] 정상 상태 복귀
  - Carry 전용 분기 증가가 종료되고 화면상 Player IDLE 복귀를 확인했다.

### 실행 증거

- Range 화면: `carry_range_latest.png`
- Attach 화면: `carry_attach_latest.png`
- Throw 후 화면: `carry_throw_latest.png`
- 실행 로그: `Client/Bin/Interaction_Baseline.log`

## 5. Before Benchmark

이번 작업에서 계측 코드를 추가하지 않았다. 현재 실행 파일이 이미 제공하던 Debug 로그 중 Rock Range에서 600프레임 동안 `N=25`, `K=1`이 안정적으로 유지된 구간만 사용했다.

| 항목 | 결과 | 상태 |
| --- | --- | --- |
| Interaction Object 수 | 25개 (`latestN=maxN=avgN=25`) | MEASURED |
| IsInteratable Calls | 프레임당 25.000회 | MEASURED |
| Priority Compare | 프레임당 0.000회 (`K=1`) | MEASURED |
| Manager Update Time | 평균 10.233 µs, 최대 388.600 µs / 600 frames | MEASURED |
| Collision Manager Time | 평균 177.533 µs, 최대 789.800 µs / 누적 3,000 frames | MEASURED |

Collision Manager 값은 Interaction F8 리셋과 동기화되지 않은 기존 누적 로그이므로 Interaction 600프레임 구간과 동일 표본은 아니다. 값을 추정하거나 보정하지 않고 로그에 출력된 수치 그대로 기록했다.

## 6. Existing Issues

이번 Characterization 실행에서 Carry Gameplay의 기존 실패는 발견되지 않았다.

첫 번째 자동 A 입력이 데스크톱 창 포커스 때문에 게임에 전달되지 않은 시도가 있었지만 `interactionStart=0`으로 즉시 식별됐으며, 게임 창을 활성화한 뒤 동일 입력을 한 번 재시도하여 정상 완료했다. 이는 Carry Gameplay 결함으로 판정하지 않는다.

## 7. 리팩토링 이후 반드시 유지할 동작

- Player가 Rock의 Interaction Range에 들어오면 상호작용 가능한 상태와 `A 들기` UI가 활성화돼야 한다.
- 단일 Rock이 CurrentTarget으로 선정된 상태에서 A 입력이 해당 Rock의 Carry Interaction을 시작해야 한다.
- Player가 Carry 상태로 전환되고 Carry Animation이 실행돼야 한다.
- `PlayerOnCarry` AnimNotify 시점까지 처음 상호작용한 Target이 유지돼야 한다.
- 최종적으로 상호작용한 Rock이 Player의 Carry 소켓에 정상 Attach되고 들고 있는 상태가 유지돼야 한다.
- 두 번째 A를 지원하는 현재 동작에서는 Rock이 Throw되고 Carry 상태가 종료된 뒤 Player가 정상 상태로 복귀해야 한다.
