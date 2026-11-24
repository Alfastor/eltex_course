#!/bin/bash

GREEN='\033[0;32m'
RED='\033[0;31m'
NC='\033[0m'

echo -e "${GREEN}=== Подготовка к запуску теста ===${NC}"

MSQID=$(ipcs -q | grep "0x00003039" | awk '{print $2}')
if [ -n "$MSQID" ]; then
    ipcrm -q $MSQID
fi
rm -f /tmp/msg_broker.pid

make clean && make
if [ $? -ne 0 ]; then
    echo -e "${RED}Ошибка компиляции!${NC}"
    exit 1
fi

mate-terminal --window --title="Broker" -- bash -c "./msg_system -b; echo 'Нажмите Enter для закрытия...'; read" &
sleep 1

mate-terminal --window --title="Subscriber" -- bash -c "./msg_system -s sports news; echo 'Нажмите Enter для закрытия...'; read" &
sleep 1

mate-terminal --window --title="Publisher" -- bash -c "./msg_system -p sports; echo 'Нажмите Enter для закрытия...'; read" &

echo -e "\n${GREEN}=== Тест запущен ===${NC}"