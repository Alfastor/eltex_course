CC = gcc
CFLAGS = -Wall -Wextra -std=c11 -g

TARGET = copy_files
OBJS = main.o ipc.o protocol.o file_ops.o

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $@ $^

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

main.o: main.c ipc.h protocol.h file_ops.h
ipc.o: ipc.c ipc.h
protocol.o: protocol.c protocol.h
file_ops.o: file_ops.c file_ops.h

clean:
	rm -f $(TARGET) *.o

.PHONY: all clean