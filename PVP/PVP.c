// PVP.c – 이벤트 기반 서버 (ncurses 없이 콘솔)
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include "shared_eco.h"
#include "game_logic.h"
#include "json_topic.h"

#define PORT         5124
#define MAX_CLIENTS  2
#define BUFFER_SIZE  1024
#define DEFAULT_DATA 100
#define DEFAULT_ATK  5
#define DEFAULT_DEF  3

// Non-blocking 설정
int set_nonblocking(int fd) {
    int f = fcntl(fd, F_GETFL, 0);
    return fcntl(fd, F_SETFL, f | O_NONBLOCK);
}

// 이벤트 문자열 매핑
const char* event_str(ActionType a) {
    switch(a) {
      case ACTION_CHARGE_WEAK:    return "Charged Weak";
      case ACTION_CHARGE_STRONG:  return "Charged Strong";
      case ACTION_ATTACK:         return "Attacked";
      case ACTION_BLOCK:          return "Blocked";
      case ACTION_COUNTER:        return "Counter Failed";
      default:                    return NULL;
    }
}

int main() {
    int server_fd, client_fd[MAX_CLIENTS], addrlen, bytes;
    struct sockaddr_in serv, cli;
    char buffer[BUFFER_SIZE];
    PlayerState P[MAX_CLIENTS] = {0};

    // 1) 서버 소켓 세팅
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        perror("socket"); exit(1);
    }
    serv.sin_family = AF_INET;
    serv.sin_addr.s_addr = INADDR_ANY;
    serv.sin_port = htons(PORT);
    if (bind(server_fd,(struct sockaddr*)&serv,sizeof(serv))<0){
        perror("bind"); exit(1);
    }
    if (listen(server_fd, MAX_CLIENTS)<0){
        perror("listen"); exit(1);
    }
    printf("🔌 Listening on port %d\n", PORT);

    // 2) 클라이언트 2명 접속 & 닉네임 수신
    addrlen = sizeof(cli);
    for(int i=0;i<MAX_CLIENTS;i++){
        client_fd[i] = accept(server_fd,(struct sockaddr*)&cli,(socklen_t*)&addrlen);
        if (client_fd[i]<0){ perror("accept"); exit(1); }
        set_nonblocking(client_fd[i]);
        // REGISTER JSON 받기 (blocking)
        while((bytes=recv(client_fd[i],buffer,BUFFER_SIZE-1,0))<=0);
        buffer[bytes]='\0';
        parse_nickname_from_json(buffer, P[i].nickname,32);

        // 초기 상태 세팅
        P[i].id = i;
        P[i].data = DEFAULT_DATA;
        P[i].attack_power = DEFAULT_ATK;
        P[i].defense_power = DEFAULT_DEF;
        P[i].current_action = ACTION_NONE;
        P[i].is_in_delay = 0;
        P[i].defense_shield = 0;
        P[i].is_counter_ready = 0;

        // ACK
        snprintf(buffer,BUFFER_SIZE,"{\"event\":\"Registered as %s\"}", P[i].nickname);
        send(client_fd[i],buffer,strlen(buffer),0);
        printf("✅ Player %d: %s\n", i, P[i].nickname);
    }
    printf("🎮 Both connected. Game Started\n");

    // 3) 게임 루프 – 최초 상태 한 번
    for (int i = 0; i < MAX_CLIENTS; i++) {
        make_status_response(buffer, &P[i], &P[1-i], "Game Started");
        send(client_fd[i], buffer, strlen(buffer), 0);
    }

    // 4) 메인 루프: 입력 수신 → 딜레이 처리 → 이벤트 전송
    while (!is_game_over(&P[0], &P[1])) {
        // 1) 클라이언트 입력 처리 (논블로킹)
       for (int i = 0; i < MAX_CLIENTS; i++) {
            int bytes = recv(client_fd[i], buffer, BUFFER_SIZE-1, MSG_DONTWAIT);
            if (bytes > 0) {
                buffer[bytes] = '\0';
                // JSON에서 액션·타임스탬프 추출
                ActionType act = parse_action_from_json(buffer);
                long long ts = parse_timestamp_from_json(buffer);
                // 예약된 액션 처리
                process_action(&P[i], &P[1-i], act);
                printf("📩 Received %s from %s at %lld\\n",
                       event_str(act) ? event_str(act) : "None",
                       P[i].nickname, ts);
            }
        }
        // 2) 딜레이 완료 행동 처리
        check_and_apply_actions(&P[0], &P[1]);
    
        // 3) 상태/이벤트 응답
        for (int i = 0; i < MAX_CLIENTS; i++) {
            char status[BUFFER_SIZE];
            // 실제 액션 이름(예: "Charged Strong")을 event_str로 가져오기
            const char *ev = event_str(P[i].current_action);
            if (!ev) ev = "Action processed";
            make_status_response(status, &P[i], &P[1-i], ev);
            send(client_fd[i], status, strlen(status), 0);
        }
    
        usleep(10000);
    }

    // 5) 게임 종료: 승패 결과 전송
    int w = get_winner(&P[0], &P[1]);
    for (int i = 0; i < MAX_CLIENTS; i++) {
        char result[BUFFER_SIZE];
        if (w == -1) sprintf(result, "{\"event\":\"Game Over: Draw!\"}");
        else if (i == w) sprintf(result, "{\"event\":\"Game Over: You Win!\"}");
        else sprintf(result, "{\"event\":\"Game Over: You Lose!\"}");
        send(client_fd[i], result, strlen(result), 0);
        close(client_fd[i]);
    }
    printf("🎮 Game Over, winner=%d\n", w);
    close(server_fd);
    return 0;
}
