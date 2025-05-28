// PVP.c – 이벤트 기반 서버 (ncurses 없이 콘솔)
#include <stdio.h>
#include <stdlib.h>
#include <string.h>      // memset
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
    int flags = fcntl(fd, F_GETFL, 0);
    return fcntl(fd, F_SETFL, flags | O_NONBLOCK);
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
    // 모든 인터페이스(0.0.0.0)에 바인드
    memset(&serv, 0, sizeof(serv));
    serv.sin_family      = AF_INET;
    serv.sin_port        = htons(PORT);
    serv.sin_addr.s_addr = htonl(INADDR_ANY);
    if (bind(server_fd, (struct sockaddr*)&serv, sizeof(serv)) < 0) {
        perror("bind"); exit(1);
    }
    if (listen(server_fd, MAX_CLIENTS) < 0) {
        perror("listen"); exit(1);
    }
    printf("🔌 Listening on port %d (all interfaces)\n", PORT);

    // 2) 클라이언트 2명 접속 & 닉네임 수신
    addrlen = sizeof(cli);
    for (int i = 0; i < MAX_CLIENTS; i++) {
        client_fd[i] = accept(server_fd, (struct sockaddr*)&cli, (socklen_t*)&addrlen);
        if (client_fd[i] < 0) { perror("accept"); exit(1); }
        set_nonblocking(client_fd[i]);
        // REGISTER JSON 받기 (blocking)
        while ((bytes = recv(client_fd[i], buffer, BUFFER_SIZE-1, 0)) <= 0);
        buffer[bytes] = '\0';
        parse_nickname_from_json(buffer, P[i].nickname, sizeof(P[i].nickname));

        // 초기 상태 세팅
        P[i].id              = i;
        P[i].data            = DEFAULT_DATA;
        P[i].attack_power    = DEFAULT_ATK;
        P[i].defense_power   = DEFAULT_DEF;
        P[i].current_action  = ACTION_NONE;
        P[i].is_in_delay     = 0;
        P[i].defense_shield  = 0;
        P[i].is_counter_ready= 0;

        // ACK
        snprintf(buffer, BUFFER_SIZE, "{\"event\":\"Registered as %s\"}", P[i].nickname);
        send(client_fd[i], buffer, strlen(buffer), 0);
        printf("✅ Player %d: %s connected\n", i, P[i].nickname);
    }
    printf("🎮 Both connected. Game Started\n");

    // 3) 게임 시작 한 번 상태 전송
    for (int i = 0; i < MAX_CLIENTS; i++) {
        make_status_response(buffer, &P[i], &P[1-i], "Game Started");
        send(client_fd[i], buffer, strlen(buffer), 0);
    }

    // 4) 메인 루프
    while (!is_game_over(&P[0], &P[1])) {
        // (1) 입력 처리
        for (int i = 0; i < MAX_CLIENTS; i++) {
            int n = recv(client_fd[i], buffer, BUFFER_SIZE-1, MSG_DONTWAIT);
            if (n > 0) {
                buffer[n] = '\0';
                ActionType act = parse_action_from_json(buffer);
                long long ts  = parse_timestamp_from_json(buffer);
                process_action(&P[i], &P[1-i], act);
                printf("📩 Received %s from %s at %lld\n",
                       event_str(act) ? event_str(act) : "None",
                       P[i].nickname, ts);
            }
        }

        // (2) 딜레이 완료된 액션 적용
        check_and_apply_actions(&P[0], &P[1]);

        // (3) 상태 & 이벤트 전송
        for (int i = 0; i < MAX_CLIENTS; i++) {
            const char *ev = event_str(P[i].current_action);
            if (!ev) ev = "Action processed";
            make_status_response(buffer, &P[i], &P[1-i], ev);
            send(client_fd[i], buffer, strlen(buffer), 0);
        }

        usleep(10000);
    }

    // 5) 게임 종료: 승패 전송
    int w = get_winner(&P[0], &P[1]);
    for (int i = 0; i < MAX_CLIENTS; i++) {
        if (w == -1)       strcpy(buffer, "{\"event\":\"Game Over: Draw!\"}");
        else if (i == w)   strcpy(buffer, "{\"event\":\"Game Over: You Win!\"}");
        else               strcpy(buffer, "{\"event\":\"Game Over: You Lose!\"}");
        send(client_fd[i], buffer, strlen(buffer), 0);
        close(client_fd[i]);
    }
    printf("🎮 Game Over, winner=%d\n", w);
    close(server_fd);
    return 0;
}