CC = gcc
CFLAGS = -Wall -Wextra -std=c11 -g
INC_DIRS = -I.

TARGET = net_sim
OBJS = main.o network.o

TEST_TARGET = test_net_runner
TEST_OBJS = test/test_network.o network.o unity/unity.o
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

test/test_network.o: test/test_network.c
	$(CC) $(CFLAGS) $(TEST_INC) -c $< -o $@

main.o: main.c network.h
network.o: network.c network.h

clean:
	rm -f $(TARGET) $(TEST_TARGET) *.o test/*.o unity/*.o

.PHONY: all clean test