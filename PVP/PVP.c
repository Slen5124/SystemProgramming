// PVP.c – 이벤트 기반 서버 (ncurses 없이 콘솔)
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include "../global.h"
#include "game_logic.h"
#include "json_topic.h"

#define PORT         5124
#define MAX_CLIENTS  2
#define BUFFER_SIZE  1024
#define DEFAULT_DATA 100
#define DEFAULT_ATK  5
#define DEFAULT_DEF  3

int set_nonblocking(int fd) {
    int flags = fcntl(fd, F_GETFL, 0);
    return fcntl(fd, F_SETFL, flags | O_NONBLOCK);
}

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

// JSON에서 nick, atk, dfs, max_data 파싱
void parse_register_json(
    const char* json, char* nick, int nicklen, int* data, int* max_data, int* atk, int* dfs,
    int* pvp_charge_minus, float* pvp_counter_atk_power_stat, float* pvp_charge_strong
) {
    if (nick) strcpy(nick, "");
    if (data) *data = DEFAULT_DATA;
    if (max_data) *max_data = DEFAULT_DATA;
    if (atk)  *atk  = DEFAULT_ATK;
    if (dfs)  *dfs  = DEFAULT_DEF;
    if (pvp_charge_minus) *pvp_charge_minus = 0;
    if (pvp_counter_atk_power_stat) *pvp_counter_atk_power_stat = 1.0f;
    if (pvp_charge_strong) *pvp_charge_strong = 1.0f;

    parse_nickname_from_json(json, nick, nicklen);

    if (data && strstr(json, "\"data\":")) {
        sscanf(strstr(json, "\"data\":"), "\"data\":%d", data);
    }
    if (max_data && strstr(json, "\"max_data\":")) {
        sscanf(strstr(json, "\"max_data\":"), "\"max_data\":%d", max_data);
    }
    if (atk && strstr(json, "\"atk\":")) {
        sscanf(strstr(json, "\"atk\":"), "\"atk\":%d", atk);
    }
    if (dfs && strstr(json, "\"dfs\":")) {
        sscanf(strstr(json, "\"dfs\":"), "\"dfs\":%d", dfs);
    }
    if (pvp_charge_minus && strstr(json, "\"pvp_charge_minus\":")) {
        sscanf(strstr(json, "\"pvp_charge_minus\":"), "\"pvp_charge_minus\":%d", pvp_charge_minus);
    }
    if (pvp_counter_atk_power_stat && strstr(json, "\"pvp_counter_atk_power_stat\":")) {
        sscanf(strstr(json, "\"pvp_counter_atk_power_stat\":"), "\"pvp_counter_atk_power_stat\":%f", pvp_counter_atk_power_stat);
    }
    if (pvp_charge_strong && strstr(json, "\"pvp_charge_strong\":")) {
        sscanf(strstr(json, "\"pvp_charge_strong\":"), "\"pvp_charge_strong\":%f", pvp_charge_strong);
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

    while(1){
        // 2) 클라이언트 2명 접속 & REGISTER 수신 // 무한루프로 게임이 끝나도 서버가 닫히지 않게게
        addrlen = sizeof(cli);
        for (int i = 0; i < MAX_CLIENTS; i++) {
            printf("accept 대기중...\n"); fflush(stdout);
            client_fd[i] = accept(server_fd, (struct sockaddr*)&cli, (socklen_t*)&addrlen);
            printf("accept 완료: %d\n", client_fd[i]); fflush(stdout);
            if (client_fd[i] < 0) { perror("accept"); exit(1); }
            set_nonblocking(client_fd[i]);
            // REGISTER JSON 받기 (blocking)
            printf("REGISTER 대기중...\n"); fflush(stdout);
            while ((bytes = recv(client_fd[i], buffer, BUFFER_SIZE-1, 0)) <= 0);
            buffer[bytes] = '\0';
            printf("REGISTER 수신: %s\n", buffer); fflush(stdout);

            int data, max_data, atk, dfs, pvp_charge_minus;
            float pvp_counter_atk_power_stat, pvp_charge_strong;
            parse_register_json(buffer, P[i].nick, sizeof(P[i].nick), &data, &max_data, &atk, &dfs,
                &pvp_charge_minus, &pvp_counter_atk_power_stat, &pvp_charge_strong);

            P[i].id              = i;
            P[i].data            = data;
            P[i].max_data        = max_data;
            P[i].atk_stat        = atk;
            P[i].dfs_stat        = dfs;
            P[i].pvp_charge_minus = pvp_charge_minus;
            P[i].pvp_counter_atk_power_stat = pvp_counter_atk_power_stat;
            P[i].pvp_charge_strong = pvp_charge_strong;
            P[i].current_action  = ACTION_NONE;
            P[i].is_in_delay     = 0;
            P[i].defense_shield  = 0;
            P[i].is_counter_ready= 0;

            snprintf(buffer, BUFFER_SIZE, "{\"event\":\"Registered as %s\"}", P[i].nick);
            send(client_fd[i], buffer, strlen(buffer), 0);
            printf("✅ Player %d: %s connected (DATA=%d, ATK=%d, DFS=%d, MAX=%d)\n", i, P[i].nick, data, atk, dfs, max_data);
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
                        P[i].nick, ts);
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
    }
    close(server_fd);
    return 0;
}
