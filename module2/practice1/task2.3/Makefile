CC = gcc
CFLAGS = -Wall -Wextra -std=c11 -g
INC_DIRS = -I.

TARGET = calc_parser
OBJS = main.o input.o tokenizer.o parser.o eval.o

TEST_TARGET = test_runner
TEST_INC = -Iunity -I.
TEST_OBJS = test/test_calc.o tokenizer.o parser.o eval.o unity/unity.o

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

test/test_calc.o: test/test_calc.c
	$(CC) $(CFLAGS) $(TEST_INC) -c $< -o $@

main.o: main.c input.h tokenizer.h parser.h eval.h
input.o: input.c input.h
tokenizer.o: tokenizer.c tokenizer.h
parser.o: parser.c parser.h tokenizer.h
eval.o: eval.c eval.h parser.h

clean:
	rm -f $(TARGET) $(TEST_TARGET) *.o test/*.o unity/*.o

.PHONY: all clean test