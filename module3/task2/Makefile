CC = gcc
CFLAGS = -Wall -Wextra -std=gnu11 -g

TARGET = msg_system
OBJS = main.o broker.o publisher.o subscriber.o

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $@ $^

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

main.o: main.c common.h
broker.o: broker.c common.h
publisher.o: publisher.c common.h
subscriber.o: subscriber.c common.h

clean:
	rm -f $(TARGET) *.o

.PHONY: all clean