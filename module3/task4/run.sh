#!/usr/bin/env bash
set -euo pipefail
cd "$(dirname "$0")"
make all

./task4 producer &
producer_pid=$!
sleep 1
open_consumer() {
    if command -v mate-terminal >/dev/null 2>&1; then
        mate-terminal --title="Потребитель $1" -- bash -lc "cd '$PWD'; ./task4 consumer; exec bash" >/dev/null 2>&1 &
    elif command -v gnome-terminal >/dev/null 2>&1; then
        gnome-terminal --title="Потребитель $1" -- bash -lc "cd '$PWD'; ./task4 consumer; exec bash" >/dev/null 2>&1 &
    elif command -v xterm >/dev/null 2>&1; then
        xterm -T "Потребитель $1" -e bash -lc "cd '$PWD'; ./task4 consumer; exec bash" >/dev/null 2>&1 &
    else
        echo "Не найден mate-terminal, gnome-terminal или xterm. Запустите ./task4 consumer в другом терминале."
        return 1
    fi
}
open_consumer 1
open_consumer 2
wait "$producer_pid"
