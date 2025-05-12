// PVP_C.c - 개선: 화면 깜빡임 최소화 + 입력 반응 최적화

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <locale.h>
#include <signal.h>
#include <wchar.h>
#define _XOPEN_SOURCE_EXTENDED
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
    for (int i = 0; i < LOG_LINES; i++) {
        int idx = (log_index - LOG_LINES + i + LOG_LINES) % LOG_LINES;
        mvprintw(17 + i, 0, "LOG %d: %-60s", i + 1, logs[idx]);
    }
}

void draw_data_bar(int y, int x, int value, const char *label) {
    int length = 20;
    int filled = value * length / MAX_DATA;
    mvprintw(y, x, "%s: [", label);
    for (int i = 0; i < length; i++) {
        if (i < filled) {
            const wchar_t block[] = {0x2588, 0};
            addwstr(block);
        } else {
            printw(".");
        }
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
    mvprintw(12, 0, "  Ctrl+D → Fire       Ctrl+S → Counter");
    mvprintw(13, 0, "  q to Quit");
}

void show_game_over(const char *message) {
    for (int i = 0; i < 6; i++) {
        erase();
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

    sprintf(buffer, "{\"action\":\"REGISTER\",\"nickname\":\"%s\",\"timestamp\":%lld}",
            nickname, get_current_time_ms());
    send(sock, buffer, strlen(buffer), 0);

    initscr(); raw(); noecho(); keypad(stdscr, TRUE); nodelay(stdscr, TRUE);

    timeout(0);  // getch() 즉시 반환
    curs_set(0);
    add_log("Waiting for opponent...");

    long long last_draw_time = 0;

    while (1) {
        int ch = getch();
        if (ch == 24) sprintf(buffer, "{\"action\":\"ATTACK\",\"timestamp\":%lld}", get_current_time_ms());
        else if (ch == 26) sprintf(buffer, "{\"action\":\"BLOCK\",\"timestamp\":%lld}", get_current_time_ms());
        else if (ch == 3)  sprintf(buffer, "{\"action\":\"CHARGE_WEAK\",\"timestamp\":%lld}", get_current_time_ms());
        else if (ch == 1)  sprintf(buffer, "{\"action\":\"CHARGE_STRONG\",\"timestamp\":%lld}", get_current_time_ms());
        else if (ch == 4)  sprintf(buffer, "{\"action\":\"FIRE\",\"timestamp\":%lld}", get_current_time_ms());
        else if (ch == 19) sprintf(buffer, "{\"action\":\"COUNTER\",\"timestamp\":%lld}", get_current_time_ms());
        else if (ch == 'q') break;
        else ch = -1;

        if (ch != -1) send(sock, buffer, strlen(buffer), 0);

        int bytes = recv(sock, buffer, BUFFER_SIZE - 1, MSG_DONTWAIT);
        if (bytes > 0) {
            buffer[bytes] = '\0';
            char *event_ptr = strstr(buffer, "\"event\":\"");
            if (event_ptr) {
                event_ptr += strlen("\"event\":\"");
                char *end = strchr(event_ptr, '"');
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
            }

            char *self_ptr = strstr(buffer, "\"self\":{");
            if (self_ptr) sscanf(self_ptr, "\"self\":{\"data\":%d,\"charged_attack\":%d}", &my_data, &my_charge);

            char *op_ptr = strstr(buffer, "\"opponent\":{\"data\":");
            if (op_ptr) {
                sscanf(op_ptr, "\"opponent\":{\"data\":%d,\"charged_attack\":%d", &enemy_data, &enemy_charge);
                char *name_ptr = strstr(op_ptr, "\"nickname\":\"");
                if (name_ptr) {
                    name_ptr += strlen("\"nickname\":\"");
                    char *end = strchr(name_ptr, '"');
                    if (end && end - name_ptr < sizeof(opponent_name)) {
                        strncpy(opponent_name, name_ptr, end - name_ptr);
                        opponent_name[end - name_ptr] = '\0';
                    }
                }
            }
        }

        long long now = get_current_time_ms();
        if (now - last_draw_time >= 33) {  // 약 30fps
            erase();
            draw_status(nickname);
            draw_logs();
            refresh();
            last_draw_time = now;
        }

        usleep(5000);
    }

    endwin();
    close(sock);
    return 0;
}
