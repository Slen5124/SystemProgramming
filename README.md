![image](https://github.com/user-attachments/assets/af98f409-c9a5-4915-bf68-4100badab4fd)# **DIVER : ONE LIFE ONLINE**

### [소개] : Diver : one life online 은 멀지 않은 미래 사이버디스토피아를 배경으로 PVE,PVP를 진행하는 게임입니다. 게임의 설정상 한번의 패배시, 게임은 영구히 삭제됩니다.

## 주요 기능 목록

### [pve_game.h]

### [pve_logic.c]

### [pve_ui.c]

#### void monster_turn(MonsterInfo *monster, PlayerState *Player, char *monster_action_result, int round, int turn, int selected_action, int monster_No);
- 몬스터 턴에 시행되는 몬스터의 행동 패턴 정의 및 실행함수

#### int wait_for_input_with_timeout(int *selected_action, int timeout_sec, int turn, int round, PlayerState Player, MonsterInfo monster, char *player_action_result, char *monster_action_result,int monster_No);
- 제한시간 안에 액션을 위해 대기를 하며 시간초과, 엔터 입력을 통해 입력을 확정하는 함수

#### void handle_player_action(int selected_action, char* player_action_result,int monster_No);
- 플레이어 액션처리함수

#### void handle_round_end(int* round, int* turn, int* cure_data, int* boss_count, int bonus_rand, int* monster_No);
- 라운드 종료처리 함수

#### void game_loop();
- 메인 게임 루프 함수

### [global.c /.h]
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
### ㄴ[json_topic.c / .h]

### Client [PVP_C.c]
### ㄴ[game_logic.c / .h]

### void parse_register_json(
    const char* json, char* nick, int nicklen, int* data, int* max_data, int* atk, int* dfs,
    int* pvp_charge_minus, float* pvp_counter_atk_power_stat, float* pvp_charge_strong
);
- json 파일 파싱 함수수

### int run_pvp_mode(int sock);
- 메인 PVP 게임 루프 함수


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


 ## [빌드 및 실행 방법]

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
