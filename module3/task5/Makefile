CC = gcc
CFLAGS = -D_DEFAULT_SOURCE -std=c11 -Wall -Wextra -Wpedantic -Werror -O2 -Iinclude
LDFLAGS = -pthread -lrt

SOURCES = src/main.c src/producer.c src/consumer.c src/storage.c src/ipc.c src/console.c

all: task5

task5: $(SOURCES)
	$(CC) $(CFLAGS) $(SOURCES) -o task5 $(LDFLAGS)

test_storage: tests/test_storage.c tests/unity/unity.c src/storage.c
	$(CC) $(CFLAGS) -Itests/unity tests/test_storage.c tests/unity/unity.c src/storage.c -o test_storage

test_ipc: tests/test_ipc.c tests/unity/unity.c src/ipc.c
	$(CC) $(CFLAGS) -Itests/unity tests/test_ipc.c tests/unity/unity.c src/ipc.c -o test_ipc $(LDFLAGS)

test: test_storage test_ipc
	./test_storage
	./test_ipc

clean:
	rm -f task5 test_storage test_ipc

.PHONY: all test clean
