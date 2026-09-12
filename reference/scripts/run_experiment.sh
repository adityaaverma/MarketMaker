#!/usr/bin/env bash
# Run one experiment and file it under experiments/YYYY-MM-DD_<name>/ with
# the config, raw output, a chart, and a notes.md stub -- the "build to
# learn" habit: one dated folder per experiment.
#
# Usage: scripts/run_experiment.sh <config.json> <short-name>
set -euo pipefail

if [ $# -lt 2 ]; then
  echo "Usage: $0 <config.json> <short-name>" >&2
  exit 1
fi

CONFIG="$1"
NAME="$2"
ROOT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
DATE=$(date +%Y-%m-%d)
DIR="$ROOT_DIR/experiments/${DATE}_${NAME}"

mkdir -p "$DIR"
cp "$CONFIG" "$DIR/config.json"

"$ROOT_DIR/build/market_maker" "$DIR/config.json" "$DIR/output.csv" | tee "$DIR/run.log"
python3 "$ROOT_DIR/scripts/plot_experiment.py" "$DIR/output.csv" | tee -a "$DIR/run.log"

if [ ! -f "$DIR/notes.md" ]; then
  cat > "$DIR/notes.md" <<NOTES
# ${NAME}

One-paragraph why: what changed vs the last experiment, what you expected,
and what actually happened.
NOTES
fi

echo ""
echo "Experiment written to $DIR"
echo "-> fill in $DIR/notes.md"
echo "-> append a one-line takeaway to INTUITION_LOG.md"
