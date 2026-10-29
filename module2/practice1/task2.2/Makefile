CC = gcc
CFLAGS = -Wall -Wextra -std=c11 -g

TARGET = calc_app
OBJS = main.o calc.o input.o

# Настройки для тестов
TEST_TARGET = test_runner
INC_DIRS = -Iunity -I.
TEST_OBJS = test/test_calc.o calc.o input.o unity/unity.o

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $@ $^

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

# Цель для запуска тестов
test: $(TEST_TARGET)
	./$(TEST_TARGET)

# Сборка раннера тестов
$(TEST_TARGET): $(TEST_OBJS)
	$(CC) $(CFLAGS) $(INC_DIRS) -o $@ $^

# Компиляция файлов для тестов
unity/unity.o: unity/unity.c
	$(CC) $(CFLAGS) $(INC_DIRS) -c $< -o $@

test/test_calc.o: test/test_calc.c
	$(CC) $(CFLAGS) $(INC_DIRS) -c $< -o $@

# Зависимости
main.o: main.c calc.h input.h
calc.o: calc.c calc.h
input.o: input.c input.h

clean:
	rm -f $(TARGET) $(TEST_TARGET) *.o test/*.o unity/*.o

.PHONY: all clean test