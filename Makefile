CC = gcc
CFLAGS = -Wall -I.
LDFLAGS_CLIENT = -lncursesw

# 실행파일 이름
CLIENT_BIN = game.exe
SERVER_BIN = server.exe

# 클라이언트용 소스
CLIENT_SRCS = \
    main.c \
    PVP/PVP_C.c \
    store.c \
    Diver_ui.c \
    global.c \
    log.c \
    Pve_logic.c \
    Pve_ui.c \
    PVP/json_topic.c

# 서버용 소스
SERVER_SRCS = \
    PVP/PVP.c \
    PVP/game_logic.c \
    PVP/json_topic.c \
    global.c

# 오브젝트 자동 생성
CLIENT_OBJS = $(CLIENT_SRCS:.c=.o)
SERVER_OBJS = $(SERVER_SRCS:.c=.o)

# 기본 빌드 타겟
all: $(CLIENT_BIN) $(SERVER_BIN)

# 클라이언트 빌드
$(CLIENT_BIN): $(CLIENT_OBJS)
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS_CLIENT)

# 서버 빌드
$(SERVER_BIN): $(SERVER_OBJS)
	$(CC) $(CFLAGS) -o $@ $^

# 개별 .c → .o
%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

# 정리
clean:
	del /Q *.o *.exe 2>nul || exit 0

.PHONY: all clean
