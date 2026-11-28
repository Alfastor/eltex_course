#!/usr/bin/env bash
set -euo pipefail
cd "$(dirname "$0")"
make all
./task5 producer &
producer_pid=$!
cleanup() {
    kill -TERM "$producer_pid" 2>/dev/null || true
}
trap cleanup INT TERM
sleep 1
open_consumer() {
    if command -v mate-terminal >/dev/null 2>&1; then
        mate-terminal --title="Потребитель $1" -- bash -lc "cd '$PWD'; ./task5 consumer; exec bash" >/dev/null 2>&1 &
    elif command -v gnome-terminal >/dev/null 2>&1; then
        gnome-terminal --title="Потребитель $1" -- bash -lc "cd '$PWD'; ./task5 consumer; exec bash" >/dev/null 2>&1 &
    elif command -v xterm >/dev/null 2>&1; then
        xterm -T "Потребитель $1" -e bash -lc "cd '$PWD'; ./task5 consumer; exec bash" >/dev/null 2>&1 &
    else
        echo "Не найден mate-terminal, gnome-terminal или xterm. Запустите ./task5 consumer в другом терминале."
        return 1
    fi
}
open_consumer 1
open_consumer 2
wait "$producer_pid"
