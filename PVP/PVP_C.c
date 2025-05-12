// PVP_C.c - JSON 기반 클라이언트 with 닉네임 & 로그 & UI + DATA 바 + 종료 애니메이션

#define _XOPEN_SOURCE_EXTENDED
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <locale.h>
#include <signal.h>
#include <wchar.h>
#include <ncursesw/ncurses.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <time.h>

#include "shared_eco.h"
#include "json_topic.h"

#define SERVER_IP "172.31.149.95"
#define PORT 5124
#define BUFFER_SIZE 1024
#define LOG_LINES 5
#define MAX_DATA 100

char logs[LOG_LINES][BUFFER_SIZE];
int log_index = 0;
char opponent_name[32] = "???";
int my_data = 0, my_charge = 0;
int enemy_data = 0, enemy_charge = 0;

void add_log(const char *msg) {
    strncpy(logs[log_index % LOG_LINES], msg, BUFFER_SIZE - 1);
    logs[log_index % LOG_LINES][BUFFER_SIZE - 1] = '\0';
    log_index++;
}

void draw_logs() {
    int start = log_index > LOG_LINES ? log_index - LOG_LINES : 0;
    for (int i = 0; i < LOG_LINES; i++) {
        int idx = (start + i) % LOG_LINES;
        mvprintw(17 + i, 0, "LOG %d: %-60s", i + 1, logs[idx]);
    }
}

void draw_data_bar(int y, int x, int value, const char *label) {
    int length = 20;
    int filled = value * length / MAX_DATA;
    mvprintw(y, x, "%s: [", label);
    for (int i = 0; i < length; i++) {
        if (i < filled) printw("█");
        else printw(".");
    }
    printw("] %3d/100", value);
}

void draw_status(const char *nickname) {
    mvprintw(1, 0, "🧑 You:    %-12s", nickname);
    draw_data_bar(2, 0, my_data, "DATA");
    mvprintw(3, 0, "⚡ Charge: %3d", my_charge);

    mvprintw(5, 0, "👾 Enemy:  %-12s", opponent_name);
    draw_data_bar(6, 0, enemy_data, "DATA");
    mvprintw(7, 0, "⚡ Charge: %3d", enemy_charge);

    mvprintw(9, 0, "Controls:");
    mvprintw(10, 0, "  Ctrl+X → ATTACK     Ctrl+Z → BLOCK");
    mvprintw(11, 0, "  Ctrl+C → Charge(3x) Ctrl+A → Charge(5x)");
    mvprintw(12, 0, "  Ctrl+V → Fire       Ctrl+S → Counter");
    mvprintw(13, 0, "  q to Quit");
}

void show_game_over(const char *message) {
    for (int i = 0; i < 6; i++) {
        clear();
        if (i % 2 == 0) attron(A_BOLD);
        mvprintw(8, 20, "%s", message);
        if (i % 2 == 0) attroff(A_BOLD);
        refresh();
        usleep(500000);
    }
    getch();
}

int main() {
    setlocale(LC_ALL, "");
    signal(SIGINT, SIG_IGN);
    signal(SIGTSTP, SIG_IGN);

    char nickname[32];
    printf("Enter your nickname: ");
    scanf("%31s", nickname);

    int sock;
    struct sockaddr_in server_addr;
    char buffer[BUFFER_SIZE];

    sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock < 0) { perror("Socket creation failed"); exit(EXIT_FAILURE); }

    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(PORT);
    inet_pton(AF_INET, SERVER_IP, &server_addr.sin_addr);

    if (connect(sock, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        perror("Connection to server failed"); close(sock); exit(EXIT_FAILURE);
    }

    snprintf(buffer, BUFFER_SIZE,
        "{\"action\":\"REGISTER\",\"nickname\":\"%s\",\"timestamp\":%lld}",
        nickname, get_current_time_ms());
    send(sock, buffer, strlen(buffer), 0);

    // 서버 응답 확인 (등록 메시지 수신)
    int bytes = recv(sock, buffer, BUFFER_SIZE - 1, 0);
    if (bytes > 0) {
        buffer[bytes] = '\0';
        add_log(buffer);
    }

    initscr(); raw(); noecho(); keypad(stdscr, TRUE); nodelay(stdscr, TRUE);
    add_log("Waiting for opponent...");

    while (1) {
        int ch = getch();
        if (ch != ERR) refresh();

        if (ch == 24) snprintf(buffer, BUFFER_SIZE, "{\"action\":\"ATTACK\",\"timestamp\":%lld}", get_current_time_ms());
        else if (ch == 26) snprintf(buffer, BUFFER_SIZE, "{\"action\":\"BLOCK\",\"timestamp\":%lld}", get_current_time_ms());
        else if (ch == 3)  snprintf(buffer, BUFFER_SIZE, "{\"action\":\"CHARGE_WEAK\",\"timestamp\":%lld}", get_current_time_ms());
        else if (ch == 1)  snprintf(buffer, BUFFER_SIZE, "{\"action\":\"CHARGE_STRONG\",\"timestamp\":%lld}", get_current_time_ms());
        else if (ch == 22) snprintf(buffer, BUFFER_SIZE, "{\"action\":\"FIRE\",\"timestamp\":%lld}", get_current_time_ms());
        else if (ch == 19) snprintf(buffer, BUFFER_SIZE, "{\"action\":\"COUNTER\",\"timestamp\":%lld}", get_current_time_ms());
        else if (ch == 'q') break;
        else continue;

        send(sock, buffer, strlen(buffer), 0);

        bytes = recv(sock, buffer, BUFFER_SIZE - 1, MSG_DONTWAIT);
        if (bytes > 0) {
            buffer[bytes] = '\0';
            char *event_ptr = strstr(buffer, "\"event\":\"");
            if (event_ptr) {
                event_ptr += strlen("\"event\":\"");
                char *end = strchr(event_ptr, '\"');
                if (end) *end = '\0';
                add_log(event_ptr);

                if (strstr(event_ptr, "Game Over")) {
                    endwin();
                    if (strstr(event_ptr, "Win")) show_game_over("🎉 YOU WIN!");
                    else if (strstr(event_ptr, "Lose")) show_game_over("💀 YOU LOSE");
                    else show_game_over("🤝 DRAW");
                    close(sock);
                    return 0;
                }

                if (strncmp(event_ptr, "Opponent: ", 10) == 0) {
                    strncpy(opponent_name, event_ptr + 10, sizeof(opponent_name) - 1);
                    opponent_name[sizeof(opponent_name) - 1] = '\0';
                }
            }

            char *self_ptr = strstr(buffer, "\"self\":{");
            if (self_ptr) sscanf(self_ptr, "\"self\":{\"data\":%d,\"charged_attack\":%d}", &my_data, &my_charge);
            char *op_ptr = strstr(buffer, "\"opponent\":{");
            if (op_ptr) sscanf(op_ptr, "\"opponent\":{\"data\":%d,\"charged_attack\":%d}", &enemy_data, &enemy_charge);
        }

        clear();
        draw_status(nickname);
        draw_logs();
        refresh();
        usleep(10000);
    }

    endwin();
    close(sock);
    return 0;
}
