// PVP.c - 서버 코드: 완전한 apply_action 및 상태 적용 추가
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

#define PORT 5124
#define MAX_CLIENTS 2
#define BUFFER_SIZE 1024
#define DEFAULT_DATA 100
#define DEFAULT_ATTACK 5
#define DEFAULT_DEFENSE 3
#define BLOCK_DURATION 3000
#define COUNTER_WINDOW 500

int set_nonblocking(int fd) {
    int flags = fcntl(fd, F_GETFL, 0);
    return fcntl(fd, F_SETFL, flags | O_NONBLOCK);
}

// 내부 함수: 딜레이가 끝난 행동 실제 적용
static void apply_action(PlayerState *actor, PlayerState *opponent) {
    ActionType action = actor->current_action;

    switch (action) {
        case ACTION_CHARGE_WEAK:
            actor->charged_attack += actor->attack_power * 3;
            break;
        case ACTION_CHARGE_STRONG:
            actor->charged_attack += actor->attack_power * 5;
            break;
        case ACTION_FIRE:
            if (opponent->is_blocking && get_current_time_ms() <= opponent->block_end_ms) {
                // 공격이 막힘 → 무효화
                // 로그를 남길 수 있음
            } else if (opponent->is_counter_ready &&
                       get_current_time_ms() - opponent->counter_window_start_ms <= COUNTER_WINDOW) {
                opponent->data -= actor->charged_attack; // 카운터 성공
            } else {
                opponent->data -= actor->charged_attack; // 일반 공격 적용
            }
            actor->charged_attack = 0;
            break;
        case ACTION_BLOCK:
            actor->is_blocking = 1;
            actor->block_end_ms = get_current_time_ms() + BLOCK_DURATION;
            break;
        case ACTION_COUNTER:
            actor->is_counter_ready = 1;
            actor->counter_window_start_ms = get_current_time_ms();
            break;
        default:
            break;
    }
}

int main() {
    int server_fd, client_fd[MAX_CLIENTS];
    struct sockaddr_in server_addr, client_addr;
    socklen_t addrlen = sizeof(client_addr);
    char buffer[BUFFER_SIZE];
    PlayerState players[MAX_CLIENTS] = {0};

    // 서버 소켓 생성
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
        perror("Socket failed"); exit(EXIT_FAILURE);
    }

    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(PORT);

    if (bind(server_fd, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        perror("Bind failed"); close(server_fd); exit(EXIT_FAILURE);
    }

    if (listen(server_fd, MAX_CLIENTS) < 0) {
        perror("Listen failed"); close(server_fd); exit(EXIT_FAILURE);
    }

    printf("🔌 Waiting for 2 players to connect on port %d...\n", PORT);

    for (int i = 0; i < MAX_CLIENTS; ++i) {
        client_fd[i] = accept(server_fd, (struct sockaddr *)&client_addr, &addrlen);
        if (client_fd[i] < 0) {
            perror("Accept failed"); exit(EXIT_FAILURE);
        }
        printf("✅ Player %d connected.\n", i);
        set_nonblocking(client_fd[i]);

        int bytes = recv(client_fd[i], buffer, BUFFER_SIZE - 1, 0);
        if (bytes > 0) {
            buffer[bytes] = '\0';
            parse_nickname_from_json(buffer, players[i].nickname, 32);
            players[i].id = i;
        } else {
            strcpy(players[i].nickname, "???");
            players[i].id = i;
        }

        players[i].data = DEFAULT_DATA;
        players[i].attack_power = DEFAULT_ATTACK;
        players[i].defense_power = DEFAULT_DEFENSE;
        players[i].current_action = ACTION_NONE;
        players[i].charged_attack = 0;
    }

    printf("🎮 Both players registered. Game started.\n");

    while (!is_game_over(&players[0], &players[1])) {
        for (int i = 0; i < MAX_CLIENTS; ++i) {
            int bytes = recv(client_fd[i], buffer, BUFFER_SIZE - 1, MSG_DONTWAIT);
            if (bytes > 0) {
                buffer[bytes] = '\0';
                ActionType action = parse_action_from_json(buffer);
                long long ts = parse_timestamp_from_json(buffer);

                if (action != ACTION_NONE) {
                    if (players[i].current_action != ACTION_NONE) {
                        char reject_msg[BUFFER_SIZE];
                        snprintf(reject_msg, BUFFER_SIZE,
                            "{\"event\":\"Currently executing: %d\"}\n", players[i].current_action);
                        send(client_fd[i], reject_msg, strlen(reject_msg), 0);
                    } else {
                        players[i].current_action = action;
                        players[i].delay_until_ms = get_current_time_ms() + get_action_delay(action);
                        players[i].is_in_delay = 1;
                        printf("📩 [%s] Action received: %d\n", players[i].nickname, action);
                    }
                }
            }
        }

        check_and_apply_actions(&players[0], &players[1]);

        for (int i = 0; i < MAX_CLIENTS; ++i) {
            char status[BUFFER_SIZE];
            make_status_response(status, &players[i], &players[1 - i], "Action processed");
            send(client_fd[i], status, strlen(status), 0);
        }

        usleep(10000);
    }

    int winner = get_winner(&players[0], &players[1]);
    for (int i = 0; i < MAX_CLIENTS; ++i) {
        char result[BUFFER_SIZE];
        if (winner == -1) snprintf(result, BUFFER_SIZE, "{\"event\":\"Game Over: Draw!\"}\n");
        else if (i == winner) snprintf(result, BUFFER_SIZE, "{\"event\":\"Game Over: You Win!\"}\n");
        else snprintf(result, BUFFER_SIZE, "{\"event\":\"Game Over: You Lose.\"}\n");
        send(client_fd[i], result, strlen(result), 0);
        close(client_fd[i]);
    }

    close(server_fd);
    return 0;
}
