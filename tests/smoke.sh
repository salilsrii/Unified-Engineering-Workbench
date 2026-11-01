#!/usr/bin/env bash
set -euo pipefail
ROOT="$(cd "$(dirname "$0")/.." && pwd)"
BIN="${TMPDIR:-/tmp}/pass-workbench-smoke"
"${CXX:-clang++}" -std=c++17 -O2 -Wall -Wextra -Wpedantic -pthread "$ROOT/src/cpp/main.cpp" -o "$BIN"
conv="$($BIN convergence)"
printf '%s\n' "$conv"
python3 - "$conv" <<'PY'
import sys
rows=[tuple(map(float,line.split(','))) for line in sys.argv[1].splitlines()]
assert [int(r[0]) for r in rows]==[10,20,40,80]
errors=[r[1] for r in rows]
assert all(errors[i+1] < errors[i] for i in range(3))
assert all(abs(errors[i]/errors[i+1]-4.0)<1e-10 for i in range(3))
PY
bp="$($BIN backprop)"
printf '%s\n' "$bp"
python3 - "$bp" <<'PY'
import re,sys
m=re.search(r'finite-difference max abs gradient delta=([0-9.eE+-]+)',sys.argv[1])
assert m and float(m.group(1)) < 1e-7
PY
"$BIN" thermal 80 "$ROOT/results/csv/smoke_thermal_80.csv"
test -s "$ROOT/results/csv/smoke_thermal_80.csv"
"$BIN" hardware | grep -q SIMULATED
echo 'SMOKE CHECKS PASS'
