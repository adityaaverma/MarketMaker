# 0001 — aggTrade stream accepted but delivers no data; proceed without it for now

**Date:** 2026-10-09

## What happened
During the Phase 0 1-hour recorder test (`run_hour_test.py`), `data/raw/binance/`
had files for `bookTicker` and `depth@100ms` for both symbols, but none for
`aggTrade` — zero messages over the full test window.

## Ruled out
- Stream name: confirmed `{symbol}@aggTrade` is correct against Binance's own
  official Python futures connector source.
- URL/path pattern: the same combined-stream URL pattern works for
  `bookTicker` and `depth@100ms`.
- Subscription rejection: sending an explicit `SUBSCRIBE` message for
  `btcusdt@aggTrade` on `wss://fstream.binance.com/ws` got back
  `{"result":null,"id":1}` (Binance's success ack) — the server accepted the
  subscription. No trade messages followed in 30+ seconds, which is not
  plausible for BTCUSDT's real trade frequency.
- Not environment-specific: reproduced both on Aditya's machine (the full
  hour-long run) and in a separate sandbox session.

## Decision
Proceed with Phase 0 using only `bookTicker` and `depth@100ms` for now —
these are sufficient for Phase 1 (order book engine). Treat the `aggTrade`
gap as an open issue to investigate separately (check Binance status,
community reports, retry later), not a blocker for Phase 0's definition of
done.

## Revisit when
Needed for Phase 2 (signed trade-flow imbalance feature) — must be resolved
before that feature can be built.
