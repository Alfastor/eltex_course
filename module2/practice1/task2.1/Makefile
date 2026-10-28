# ==========================================
# 1. БЛОК ПЕРЕМЕННЫХ
# ==========================================
CC = gcc
CFLAGS = -Wall -Wextra -std=c11 -g

# Переменные основной программы
TARGET = phonebook
OBJS = main.o contact.o input.o

TEST_TARGET = test_runner
INC_DIRS = -Iunity -I.
TEST_OBJS = test/test_main.o contact.o input.o unity/unity.o


# ==========================================
# 2. ОСНОВНЫЕ ПРАВИЛА СБОРКИ (ПРОГРАММА)
# ==========================================
all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $@ $^

# Шаблонное правило компиляции (для основной программы)
%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@


# ==========================================
# 3. ПРАВИЛА ДЛЯ ТЕСТОВ (UNIT-ТЕСТЫ)
# ==========================================
# Цель для запуска тестов
test: $(TEST_TARGET)
	./$(TEST_TARGET)

# Сборка исполняемого файла тестов
$(TEST_TARGET): $(TEST_OBJS)
	$(CC) $(CFLAGS) $(INC_DIRS) -o $@ $^

# Компиляция самого фреймворка Unity
unity/unity.o: unity/unity.c
	$(CC) $(CFLAGS) $(INC_DIRS) -c $< -o $@

# Компиляция тестового файла (у него своя папка, поэтому указываем путь явно)
test/test_main.o: test/test_main.c
	$(CC) $(CFLAGS) $(INC_DIRS) -c $< -o $@


# ==========================================
# 4. ЯВНЫЕ ЗАВИСИМОСТИ ОТ ЗАГОЛОВОЧНЫХ ФАЙЛОВ
# ==========================================
main.o: main.c contact.h input.h
contact.o: contact.c contact.h
input.o: input.c input.h


# ==========================================
# 5. СЛУЖЕБНЫЕ ЦЕЛИ (ОЧИСТКА И PHONY)
# ==========================================
clean:
	rm -f $(TARGET) $(TEST_TARGET) *.o test/*.o unity/*.o

# Не забываем добавить test в список фальшивых целей
.PHONY: all clean test