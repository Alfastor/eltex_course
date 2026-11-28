#!/usr/bin/env bash
set -euo pipefail
cd "$(dirname "$0")"

tmp_dir=$(mktemp -d)
producer_pid=""
consumer_pid=""
restart_producer_pid=""
restart_consumer_pid=""

stop_processes() {
    for pid in "$producer_pid" "$consumer_pid" "$restart_producer_pid" "$restart_consumer_pid"; do
        if [[ -n "$pid" ]]; then
            kill -TERM "$pid" 2>/dev/null || true
            kill -KILL "$pid" 2>/dev/null || true
            wait "$pid" 2>/dev/null || true
        fi
    done
}

cleanup() {
    stop_processes
    ./task5 cleanup >/dev/null 2>&1 || true
    rm -rf "$tmp_dir"
}
trap cleanup EXIT INT TERM

make clean
make all
make test
./task5 cleanup >/dev/null 2>&1 || true

./task5 producer >"$tmp_dir/producer_crash.log" 2>&1 &
producer_pid=$!
sleep 0.3
./task5 consumer >"$tmp_dir/consumer_crash.log" 2>&1 &
consumer_pid=$!
sleep 0.7
kill -KILL "$producer_pid"
wait "$producer_pid" 2>/dev/null || true
producer_pid=""
if ! timeout 8s tail --pid="$consumer_pid" -f /dev/null; then
    echo "Потребитель не завершился после аварии производителя" >&2
    exit 1
fi
wait "$consumer_pid"
consumer_pid=""
grep -q "аварийно завершился" "$tmp_dir/consumer_crash.log"

./task5 producer >"$tmp_dir/producer_consumer_loss.log" 2>&1 &
restart_producer_pid=$!
sleep 0.3
./task5 consumer >"$tmp_dir/consumer_loss.log" 2>&1 &
restart_consumer_pid=$!
sleep 0.8
kill -KILL "$restart_consumer_pid"
wait "$restart_consumer_pid" 2>/dev/null || true
restart_consumer_pid=""
if ! timeout 12s tail --pid="$restart_producer_pid" -f /dev/null; then
    echo "Производитель не завершился после потери всех потребителей" >&2
    exit 1
fi
wait "$restart_producer_pid"
restart_producer_pid=""
grep -q "Активные потребители отсутствуют" "$tmp_dir/producer_consumer_loss.log"

./task5 producer >"$tmp_dir/producer_normal.log" 2>&1 &
producer_pid=$!
sleep 0.3
./task5 consumer >"$tmp_dir/consumer_normal.log" 2>&1 &
consumer_pid=$!
sleep 0.5
kill -TERM "$producer_pid"
wait "$producer_pid"
producer_pid=""
wait "$consumer_pid"
consumer_pid=""

echo "Все проверки пройдены: POSIX shared memory, POSIX semaphore, авария производителя, потеря потребителей и штатное завершение"
