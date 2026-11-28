#!/usr/bin/env bash
set -euo pipefail
cd "$(dirname "$0")"
tmp_dir=$(mktemp -d)
producer_pid=""
consumer_pid=""
restart_producer_pid=""
restart_consumer_pid=""
cleanup() {
    for pid in "$producer_pid" "$consumer_pid" "$restart_producer_pid" "$restart_consumer_pid"; do
        if [[ -n "$pid" ]]; then kill -KILL "$pid" 2>/dev/null || true; fi
    done
    ./task4 cleanup >/dev/null 2>&1 || true
    rm -rf "$tmp_dir"
}
trap cleanup EXIT INT TERM
make clean
make all
make test
./task4 cleanup >/dev/null 2>&1 || true
./task4 producer >"$tmp_dir/producer_crash.log" 2>&1 &
producer_pid=$!
sleep 0.2
./task4 consumer >"$tmp_dir/consumer_crash.log" 2>&1 &
consumer_pid=$!
sleep 0.6
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
./task4 producer >"$tmp_dir/producer_consumer_loss.log" 2>&1 &
restart_producer_pid=$!
sleep 0.2
./task4 consumer >"$tmp_dir/consumer_loss.log" 2>&1 &
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
./task4 producer >"$tmp_dir/producer_normal.log" 2>&1 &
producer_pid=$!
sleep 0.2
./task4 consumer >"$tmp_dir/consumer_normal.log" 2>&1 &
consumer_pid=$!
kill -TERM "$producer_pid"
wait "$producer_pid"
producer_pid=""
wait "$consumer_pid"
consumer_pid=""
echo "Все проверки пройдены: авария производителя, потеря потребителей и штатное завершение"
