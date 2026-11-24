#!/bin/bash
echo "Тест 1: Анонимный канал"
echo "Hello from file A" > a.txt
echo "Content of file B" > b.txt
./copy_files a.txt b.txt
sleep 1
echo "Результат для a.txt.copy: $(cat a.txt.copy)"
echo "Результат для b.txt.copy: $(cat b.txt.copy)"

echo -e "\nТест 2: Именованный канал (-p)"
./copy_files -p mypipe a.txt b.txt
sleep 1
echo "Результат для a.txt.copy: $(cat a.txt.copy)"

echo -e "\nТест 3: Обработка несуществующего файла"
./copy_files -p mypipe missing.txt

rm -f a.txt b.txt a.txt.copy b.txt.copy mypipe_p2c mypipe_c2p
