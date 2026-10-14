# 0002 — normalized event schema

**Date:** 2026-10-14

## Decision
Shared schema for the engine's `event_reader.hpp` and the recorder's
`normalize.py`, so both Binance-recorded and Tardis historical data feed
the engine identically.

```
Depth rows:  ts_exchange_ns, ts_local_ns, U, u, pu, side, price_ticks, qty_lots, is_snapshot
Trade rows:  ts_exchange_ns, ts_local_ns, price_ticks, qty_lots, aggressor_side
Ticker rows: ts_exchange_ns, ts_local_ns, bid_ticks, bid_qty, ask_ticks, ask_qty
```

Prices and quantities are integer fixed-point (`price_ticks`, `qty_lots`),
converted from Binance's decimal strings — never floats, per `CLAUDE.md`'s
engineering conventions.

## Known gap: perpetual-specific metrics
Funding rate, mark price, index price, open interest — none are captured
yet. None are equity/options concepts (no equivalent in the original
Avellaneda-Stoikov setting), and none are needed for Phase 1 (order book
reconstruction doesn't use them). Relevant later:
- Funding rate: Phase 3's `pnl.hpp` needs it for correct inventory PnL.
- Open interest / funding rate: potentially useful Phase 2 features (regime
  / positioning signals).

Two separate fixes needed, both REST polls (not fixing any websocket
stream):
- Funding rate + mark price + index price all come from one endpoint,
  `fapi/v1/premiumIndex` — same underlying data as the `markPrice` websocket
  stream that failed to deliver in the `aggTrade` investigation (see
  `0001-aggtrade-stream-not-delivering.md`). Changes slowly (every 8h), so a
  periodic poll is the right approach anyway, not a workaround.
- Open interest: `fapi/v1/openInterest`, no websocket stream exists for it
  at all. Separate poll, coarser cadence (minutes, not seconds) is fine.

Revisit before Phase 3; open interest also worth adding before Phase 2
feature work starts if time allows.
