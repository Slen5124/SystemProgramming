### 실행 전, 주의사항
- 본 게임은 UTF - 8 [setlocale(LC_ALL, "en_US.utf-8");] 을 사용합니다. 로케일 설정을 UTF-8 US로 해야 깨지지 않습니다.
- 더불어 ncurses를 사용해 아래와 같은 sudo 명령어를 사용하여 미리 다운로드 해주셔야 make 파일이 사용가능 합니다.
    - sudo apt update
    - sudo apt install libncurses5-dev libncursesw5-dev
- 이외의 폰트깨짐이 있다면 아래 명령어를 참고하시길 바랍니다.
    - export LANG=en_US.UTF-8
    - export LC_ALL=en_US.UTF-8

### 게임 실행시 실행순서
 - make를 실행합니다.
 - ./server.exe 를 입력하여 서버를 먼저 실행합니다.
 - ./game.exe 를 입력하여 클라이언트를 접속합니다. (서버가 먼저 열리고 클라이언트가 접속해야합니다.)
 - 게임이 정상실행 되었다면 닉네임을 입력하고 게임을 즐기시면 됩니다.

## 주요 기능 목록

![image](https://github.com/user-attachments/assets/4533fc41-96d2-422c-affa-cfbad8aab846)

#### 프로그램 흐름도

### [pve_game.h]
- pve 게임에 필요한 몬스터 구조체나 함수들을 정의해둔 헤더파일

### [pve_logic.c]
- pve 게임의 메인이되는 로직들을 모아둔 파일
##### 주요 함수 : void monster_turn(MonsterInfo *monster, PlayerState *Player, char *monster_action_result, int round, int turn, int selected_action, int monster_No);
 - 몬스터 턴에 몬스터의 액션(공격, 방어, Bit충전 등)을 실행 및 관리한다.

##### 주요 함수 : int wait_for_input_with_timeout(int *selected_action, int timeout_sec, int turn, int round, PlayerState Player, MonsterInfo monster, char *player_action_result, char *monster_action_result,int monster_No);
 - 플레이어 행동에 시간제약을 걸고 액션을 제어한다.

### [pve_ui.c]
 - 플레이어, 몬스터의 도트그래픽과 플레이어의 스텟을 출력하는 함수들이 모여있는 파일

#### void game_loop();
- 메인 게임 루프 함수

### [global.c /.h]임 방법]

 또한 게임 내에서 아래와 같은 자세한 조작법이 담긴 가이드 화면을 제공했습니다.
 ![image](https://github.com/user-attachments/assets/3e4f3dcd-e9aa-4023-b1fe-6aa4105ecdc5)


 ## [팀원 정보]

 강원준 
:  Pve_game.h, Pve_ui.c, Pve_logic.c 파일 제작.
 몬스터 라운드별 턴제 전투 PVE 개발, 몬스터 UI 그래픽 구상, 이스터 에그 기능 추가, 구조체 인자명 및 포인터 타입 통일, PPT 제작 및 편집

김병조 
: global.c, global.h, store.c, store.h, diver_ui.c, diver_ui.h, log.c, log.h 파일 제작
 전역변수 관리 및 통합, 상점 코드 구현, 전체 인터페이스 및 편의 기능 구현, 시그널 핸들링 적용, 통합 디버깅, 
데모 영상 제작 및 편집

이윤한 
: game_logic.c, game_logic.h, json_topic.c, json_topic.h, PVP_C.c, PVP.c 파일 제작.
 네트워크 시스템 기반 PVP 개발, 서버 구축 및 관리, 게임 시스템 구조 설계 및 전체 코드 통합, 게임 컨셉 및 
스토리텔링 구상, 프로젝트 전체 일정 수립 및 진척 관리, 프로젝트 협업 리딩, 게임  내 디자인적 요소 추가 및 툴 제공, 발표
