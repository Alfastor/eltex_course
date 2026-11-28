CC ?= gcc
CFLAGS ?= -std=c11 -O2 -Wall -Wextra -Wpedantic -Werror
CPPFLAGS ?= -Iinclude
LDFLAGS ?=

TARGET = udp_chat
UNIT_TEST = tests/unit_test
SOURCES = src/main.c src/network.c src/protocol.c src/users.c src/console.c
OBJECTS = $(SOURCES:.c=.o)

.PHONY: all clean test

all: $(TARGET)

$(TARGET): $(OBJECTS)
	$(CC) $(CFLAGS) $(OBJECTS) $(LDFLAGS) -o $@

src/%.o: src/%.c
	$(CC) $(CPPFLAGS) $(CFLAGS) -c $< -o $@

$(UNIT_TEST): tests/unit_test.c src/protocol.c src/users.c
	$(CC) $(CPPFLAGS) $(CFLAGS) tests/unit_test.c src/protocol.c src/users.c $(LDFLAGS) -o $@

clean:
	rm -f $(OBJECTS) $(TARGET) $(UNIT_TEST)

test: all $(UNIT_TEST)
	./$(UNIT_TEST)
