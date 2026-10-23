# Компилятор
CC = gcc

# Флаги компиляции: 
# -Wall, -Wextra (показывать все предупреждения)
# -std=c11 (стандарт C11)
# -g (добавить отладочную информацию для GDB)
CFLAGS = -Wall -Wextra -std=c11 -g

# Имя итогового исполняемого файла
TARGET = phonebook

# Список объектных файлов
OBJS = main.o contact.o input.o

# Цель по умолчанию (выполняется при вводе просто `make`)
all: $(TARGET)

# Правило линковки: создаем исполняемый файл из объектных файлов
# $@ - имя цели (phonebook), $^ - все зависимости (main.o contact.o input.o)
$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $@ $^

# Шаблонное правило компиляции: как из .c получить .o
# $< - первая зависимость (имя .c файла), $@ - имя цели (.o файл)
%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

# Явное указание зависимостей от заголовочных файлов
# Если изменится contact.h, пересоберутся main.o и contact.o
main.o: main.c contact.h input.h
contact.o: contact.c contact.h input.h
input.o: input.c input.h

# Очистка проекта от временных файлов и бинарника
clean:
	rm -f $(TARGET) *.o

# Помечаем цели, которые не являются файлами
.PHONY: all clean