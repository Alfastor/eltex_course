CC = gcc
CFLAGS = -D_DEFAULT_SOURCE -std=c11 -Wall -Wextra -Wpedantic -Werror -O2 -Iinclude
LDFLAGS =

SOURCES = src/main.c src/producer.c src/consumer.c src/storage.c src/ipc.c src/console.c

all: task4

task4: $(SOURCES)
	$(CC) $(CFLAGS) $(SOURCES) -o task4 $(LDFLAGS)

test_storage: tests/test_storage.c tests/unity/unity.c src/storage.c
	$(CC) $(CFLAGS) -Itests/unity tests/test_storage.c tests/unity/unity.c src/storage.c -o test_storage

test: test_storage
	./test_storage

clean:
	rm -f task4 test_storage

.PHONY: all test clean
