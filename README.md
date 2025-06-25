### [실행 전 주의사항]
- 본 게임은 UTF - 8 [setlocale(LC_ALL, "en_US.utf-8");] 을 사용합니다. 로케일 설정을 UTF-8 US로 해야 깨지지 않습니다.
- 더불어 ncurses를 사용해 아래와 같은 sudo 명령어를 사용하여 미리 다운로드 해주셔야 make 파일이 사용가능 합니다.
    - sudo apt update
    - sudo apt install libncurses5-dev libncursesw5-dev
- 이외의 폰트깨짐이 있다면 아래 명령어를 참고하시길 바랍니다.
    - export LANG=en_US.UTF-8
    - export LC_ALL=en_US.UTF-8

### [게임 실행순서]
 - make를 실행합니다.
 - ./server.exe 를 입력하여 서버를 먼저 실행합니다.
 - ./game.exe 를 입력하여 클라이언트를 접속합니다. (서버가 먼저 열리고 클라이언트가 접속해야합니다.)
 - 게임이 정상실행 되었다면 닉네임을 입력하고 게임을 즐기시면 됩니다.

## 주요 기능 목록

![image](https://github.com/user-attachments/assets/4533fc41-96d2-422c-affa-cfbad8aab846)

#### 프로그램 흐름
 - server.exe로 PVP.c 의 서버를 열고, game.exe를 통해 main.c의 메인 함수를 실행시킵니다. PVE를 5분간 진행(pve_game.h, pve_logic.c 등이 관여)하며 DATA를 획득합니다. 도중에 상점에도 진입할 수 있으며 DATA를 사용하여 플레이어의 스텟을 강화시킬 수 있습니다. 5분이 끝난 후에는 PVP모드로 넘어가기 전에 마지막으로 상점 이용 후 PVP를 진행(PVP_C.c, game_logic.c/.h 등이 관여)하게 됩니다. 패배한 플레이어는 게임 실행파일이 삭제되며 승리한 플레이어는 스텟이 초기화 된 채로 게임을 다시 시작하게 됩니다. 이름옆에 [n연승] 이라는 칭호도 얻게 됩니다.

## [함수 및 파일 설명]

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

##### 주요 함수 : void game_loop();
- 메인 게임 루프 함수

### [global.c /.h]Add commentMore actions
- 전역변수를 관리하는 파일
    
### [diver_ui.c /.h]
- 게임 전체의 UI와 편의기능을 관리하는 파일
##### 주요함수 : int pause_choice()
-일시 정지 화면에서 사용자의 선택 입력을 받아 해당 메뉴 항목 번호를 반환한다.
##### 주요함수 : void loser_ending_screen() 
- PVP 게임에서 패배자의 엔딩화면을 출력하며,  system() 명령어를 이용해 실행파일을 삭제한다.


### [store.c /.h]
- 능력 및 아이템 구매의 로직과 UI 등 상점과 관련된 함수를 모아둔 파일
##### 주요함수 : void handle_buy(int choice)
 - 유저가 선택한 선택을 정수형으로 받아 능력 강화 또는 아이템 구매를 실행한다.
##### 주요함수 : int store_menu_ui(int time_limit,time_t start)     
 - 지정된 시간(time_limit) 동안 상점 UI를 호출하고, 사용자의 입력을 받아 선택 결과를 반환한다.

### [log.c /.h]
- 상점에서 구매 정보를 txt 파일로 기록하고 이를 상점 방문시 보여주는 파일

### Server [PVP.c]
 - 서버를 열고, 로그를 출력하며 서버를 관리하는 파일
##### 주요함수 : void parse_register_json(const char* json, char* nick, int nicklen, int* data, int* max_data, int* atk, int* dfs,int* pvp_charge_minus, float* pvp_counter_atk_power_stat, float* pvp_charge_strong); 
##### 주요함수 : int main();
 - 서버 소켓팅 및 접속, 수신, 게임승패 전송을 하는 서버의 핵심.

### ㄴ[json_topic.c / .h]
 - 서버와 클라이언트 간의 JSON 메시지를 처리하는 역할을 하는 파일

### Client [PVP_C.c]
##### 주요함수 : int run_pvp_mode(int sock);
 - 소켓을 인자로 받아서 서버에 접속하고 json 파일로 서버와 소통합니다.
### ㄴ[game_logic.c / .h]
 - 게임로직이 담긴 파일로 플레이어의 입력처리와 액션, 승패판단을 하는 파일.

## [플레이 동영상]

[00.LOSER] : https://www.youtube.com/watch?v=fSdrv4mLDfk 

- 이 영상은 서버를 만들고 플레이어가 패배하고 게임 실행파일이 삭제되기까지의 전체 과정을 빠른 재생으로 담아냈습니다. 
PVE, 상점 이용, PVP 전투를 간결하게 보여줍니다.

[01.WINNER] : https://www.youtube.com/watch?v=zuqgisVv3pA
 - 이 영상은 PVP에서 승리한 플레이어의 PVP 이후 화면을 담았습니다. 
승리 후 패배와는 다른 엔딩화면, 그리고 삭제되지 않고 바로 새로운 게임으로 이어지는 과정을 담고 있습니다. 
새로운 게임에 들어가며 연승 수가 늘었고, 이를 제외한 데이터들은 초기화되었습니다.

[02.Store] : https://www.youtube.com/watch?v=jyIqrt0OGdk
- 이 영상은 PVE 중 일시정지로 상점에 이동해 아이템을 전부 구매하는 과정을 담고 있습니다.


 ## [게임 방법]

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
