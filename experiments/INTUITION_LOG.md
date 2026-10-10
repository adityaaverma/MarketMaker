# Intuition Log

One-line takeaway per experiment, newest first.

- 2026-10-09: Wi-Fi blip (~11-16s) didn't lose data — bookTicker `u` stayed continuous, messages arrived in a burst on reconnect. Real guarantee still needs depth `pu`/`u` resync check (Phase 1).
- 2026-10-09: `aggTrade` stream accepts subscription but delivers zero messages (bookTicker/depth work fine) — see `docs/decisions/0001-aggtrade-stream-not-delivering.md`.
