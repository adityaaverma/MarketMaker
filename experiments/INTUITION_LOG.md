# Intuition Log

One-line takeaway per experiment, newest first.

- 2026-10-09: Wi-Fi blip (~11-16s) didn't lose data — bookTicker `u` stayed continuous, messages arrived in a burst on reconnect. Real guarantee still needs depth `pu`/`u` resync check (Phase 1).
- 2026-10-09: `aggTrade` stream accepts subscription but delivers zero messages (bookTicker/depth work fine) — see `docs/decisions/0001-aggtrade-stream-not-delivering.md`.
- 2026-10-13: Writer leaked a file handle + zstd buffer per hour-rollover (dict keyed by rotating path, never evicted) — 45MB -> 1.5GB over 48h on the VM, ~17h from OOM on the 2GB instance. Fixed by keying open handles by stream identity instead of path. Caught via routine VM status check, not a test — worth adding a memory-bound regression test later.
