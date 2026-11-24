#!/bin/bash

GREEN='\033[0;32m'
NC='\033[0m'

echo -e "${GREEN}=== Подготовка к запуску чата ===${NC}"

rm -f /dev/mqueue/chat_1 /dev/mqueue/chat_2 2>/dev/null

echo "Компиляция проекта..."
make clean && make
if [ $? -ne 0 ]; then
    echo "Ошибка компиляции!"
    exit 1
fi

echo -e "${GREEN}Запуск первого участника (Создатель)...${NC}"
mate-terminal --window --title="Chat - Creator" -- bash -c "./chat_p2p chat; echo 'Нажмите Enter для закрытия...'; read" &

sleep 1

echo -e "${GREEN}Запуск второго участника (Участник)...${NC}"
mate-terminal --window --title="Chat - Participant" -- bash -c "./chat_p2p chat; echo 'Нажмите Enter для закрытия...'; read" &

echo -e "\n${GREEN}=== Чат запущен! ===${NC}"
