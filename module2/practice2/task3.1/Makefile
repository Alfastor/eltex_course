CC = gcc
CFLAGS = -Wall -Wextra -std=c11 -g
INC_DIRS = -I.

TARGET = perm_tool
OBJS = main.o input.o permissions.o

TEST_TARGET = test_perm_runner
TEST_OBJS = test/test_main.o permissions.o unity/unity.o
TEST_INC = -Iunity -I.

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $@ $^

%.o: %.c
	$(CC) $(CFLAGS) $(INC_DIRS) -c $< -o $@

test: $(TEST_TARGET)
	./$(TEST_TARGET)

$(TEST_TARGET): $(TEST_OBJS)
	$(CC) $(CFLAGS) $(TEST_INC) -o $@ $^

unity/unity.o: unity/unity.c
	$(CC) $(CFLAGS) $(TEST_INC) -c $< -o $@

test/test_main.o: test/test_main.c
	$(CC) $(CFLAGS) $(TEST_INC) -c $< -o $@

main.o: main.c input.h permissions.h
input.o: input.c input.h
permissions.o: permissions.c permissions.h

clean:
	rm -f $(TARGET) $(TEST_TARGET) *.o test/*.o unity/*.o

.PHONY: all clean test