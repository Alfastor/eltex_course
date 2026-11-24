CC = gcc
CFLAGS = -Wall -Wextra -std=gnu11 -g -lrt
INC_DIRS = -I.

TARGET = chat_p2p
OBJS = main.o chat.o ipc_posix.o

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $@ $^

%.o: %.c
	$(CC) $(CFLAGS) $(INC_DIRS) -c $< -o $@

main.o: main.c common.h ipc_posix.h chat.h
chat.o: chat.c chat.h common.h ipc_posix.h
ipc_posix.o: ipc_posix.c ipc_posix.h common.h

clean:
	rm -f $(TARGET) *.o

.PHONY: all clean