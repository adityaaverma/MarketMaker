# ROADMAP — crypto-mm-research

**Goal:** a flagship QR project that holds up to a 30-minute interview drill.
**Central question:** *Which order-book signals predict short-horizon price moves in crypto perpetuals, how fast do they decay, and how much of that predictive power survives when a market maker actually trades on it?*

**Timeline:** 8 Oct 2026 → 27 Jan 2027 (ends just before the Jan–Feb career checkpoint).
**Weighting:** Phase 2 (research) is the core. Phase 1 is kept lean: correct and tested, not hyper-optimised.
**If coursework or exams collide:** slip Phase 3. Never cut rigour in Phase 2.

---

## Repository structure

```
crypto-mm-research/
├── CLAUDE.md                     # rules + context for Claude Code
├── ROADMAP.md                    # this file
├── README.md                     # architecture, results, how to reproduce (Phase 4)
├── .gitignore                    # data/, build/, .venv/, *.zst, *.parquet
│
├── docs/
│   ├── research_log.md           # dated entries: what was run, result, config
│   ├── decisions/                # one short .md per design decision (ADR-lite)
│   │   └── 0001-normalized-event-format.md
│   ├── ideas.md                  # parking lot for scope creep — not built until Phase 4 is done
│   ├── INTERVIEW_PREP.md         # Phase 4
│   └── writeup/                  # final research note
│
├── data/                         # gitignored
│   ├── raw/{exchange}/{symbol}/{stream}/YYYY-MM-DD/HH.jsonl.zst
│   ├── raw/tardis/...            # free first-day-of-month samples
│   ├── normalized/               # engine input (one schema for all sources)
│   └── processed/                # Parquet for research
│
├── recorder/                     # Python — Phase 0
│   ├── pyproject.toml
│   ├── config.yaml               # symbols, streams, output dir, snapshot interval
│   ├── src/recorder/
│   │   ├── main.py               # entrypoint, asyncio loop
│   │   ├── ws_client.py          # connect, reconnect w/ backoff, heartbeat
│   │   ├── snapshots.py          # periodic + on-reconnect REST depth snapshots
│   │   ├── writer.py             # hourly-rotated, zstd-compressed JSONL
│   │   ├── normalize.py          # raw Binance / Tardis → normalized schema (Phase 1)
│   │   └── health.py             # msgs/hour, pu/u breaks, disconnect report
│   └── tests/
│
├── engine/                       # C++20 — Phases 1 & 3
│   ├── CMakeLists.txt
│   ├── include/engine/
│   │   ├── types.hpp             # Side, Price(ticks), Qty(lots), Timestamp(ns), decimal→ticks
│   │   ├── order_book.hpp        # L2 book: apply, remove, best, top-N, mid, spread
│   │   ├── event_reader.hpp      # streams normalized events from disk
│   │   ├── book_sync.hpp         # Binance snapshot+diff state machine
│   │   ├── replay.hpp            # merges depth/trade streams by local time, dispatches callbacks
│   │   ├── sampler.hpp           # exports top-N snapshots for research
│   │   └── mm/
│   │       ├── strategy.hpp          # interface: on_book, on_trade, on_fill, on_timer
│   │       ├── avellaneda_stoikov.hpp
│   │       ├── signal_skew.hpp
│   │       ├── order_sim.hpp         # latency, place/cancel delays
│   │       ├── queue_model.hpp       # queue-position fill model
│   │       └── pnl.hpp               # accounting + PnL decomposition + mark-outs
│   ├── src/
│   ├── apps/
│   │   ├── replay_main.cpp
│   │   ├── validate_main.cpp     # reconstructed top-of-book vs recorded bookTicker
│   │   ├── sample_main.cpp       # export snapshots for Phase 2
│   │   └── backtest_main.cpp     # Phase 3
│   ├── tests/                    # GoogleTest
│   └── bench/                    # Google Benchmark
│
├── research/                     # Python — Phase 2
│   ├── pyproject.toml
│   ├── src/research/
│   │   ├── io.py                 # load processed Parquet, filter gap windows
│   │   ├── qa.py                 # data quality checks
│   │   ├── labels.py             # forward mid returns (event-time + clock-time)
│   │   ├── features.py           # OFI, multi-level OFI, imbalance, microprice, trade imbalance
│   │   ├── evaluate.py           # IC, decay curves, regime splits, walk-forward OOS
│   │   ├── costs.py              # fees, half-spread, breakeven thresholds
│   │   └── plots.py
│   ├── notebooks/                # exploration only, numbered: 01_qa.ipynb, ...
│   └── tests/
│
├── experiments/                  # existing habit, one folder per day
│   ├── INTUITION_LOG.md
│   └── YYYY-MM-DD_short-name/{config.json, output.csv, notes.md}
│
└── scripts/
    └── plot_experiment.py
```

---

## Phase 0 — Data foundation · 8 Oct → 14 Oct

Data accumulates with time, so this starts immediately and runs in the background for the whole project.

**Symbols:** BTCUSDT, ETHUSDT, plus one mid-liquidity perp (volume rank ~20–40) for contrast.
**Streams per symbol:** `@depth@100ms` (L2 diffs), `@aggTrade` (trades), `@bookTicker` (real-time top of book, used to validate the engine).

- [ ] **Access check from Singapore.** Hit the REST snapshot endpoint and open a websocket from your machine. If blocked, decide between a VPS in an allowed region or switching to Bybit/OKX. Record the decision in `docs/decisions/`.
- [ ] Download Tardis.dev free samples (first day of each month) for BTCUSDT perp — L2 incremental, trades, quotes — for ~3 months. Develop against these while your own data accumulates.
- [ ] Recorder: combined websocket stream; store each raw message **unchanged** plus local receive time in ns; hourly file rotation; zstd compression.
- [ ] Reconnect with exponential backoff; log every disconnect with timestamps to a gaps file.
- [ ] REST depth snapshot hourly **and** after every reconnect, stored alongside the raw stream (the engine needs these to sync).
- [ ] `health.py`: messages per hour per stream, `pu ≠ previous u` breaks, disconnect summary.
- [ ] Run it 24/7 (always-on machine or small VPS).

**Definition of done:** 48 hours of continuous recording for all three symbols, plus a health report that lists every gap.

---

## Phase 1 — C++ order book engine · 15 Oct → 4 Nov

Lean, correct, tested. This replaces the existing LOB line on the resume.

- [ ] **Decision doc 0001:** normalized event schema, shared by Binance and Tardis inputs.
  Depth rows: `ts_exchange_ns, ts_local_ns, U, u, pu, side, price_ticks, qty_lots, is_snapshot`.
  Trade rows: `ts_exchange_ns, ts_local_ns, price_ticks, qty_lots, aggressor_side`.
  Ticker rows: `ts_exchange_ns, ts_local_ns, bid_ticks, bid_qty, ask_ticks, ask_qty`.
  Start with CSV; switch to a flat binary format only if parsing dominates the benchmark.
- [ ] `normalize.py`: raw Binance JSONL → normalized; Tardis CSV → same schema (Tardis has no U/u/pu — engine runs in "pre-sequenced" mode for it).
- [ ] `types.hpp`: decimal string → integer ticks without going through `double`. Unit-test tricky inputs.
- [ ] `order_book.hpp` using `std::map` per side: absolute-quantity updates, remove on zero, best bid/ask, top-N, mid, spread, crossed-book detection.
- [ ] Tests: empty book, insert/update/remove, removing a missing level (must be a no-op), crossed book, top-N ordering.
- [ ] `book_sync.hpp`: state machine `WAITING_SNAPSHOT → SYNCING → LIVE`, implementing the Binance rules in `CLAUDE.md`; resync on `pu` break.
- [ ] Sync tests with synthetic sequences: clean run, stale events before snapshot, snapshot landing mid-stream, `pu` gap → resync.
- [ ] `replay.hpp`: merge depth + trade streams by local timestamp; dispatch `on_book_update` / `on_trade` callbacks. Strategies and samplers plug into this interface.
- [ ] `validate_main`: compare reconstructed best bid/ask against recorded bookTicker (nearest prior timestamp). Report match rate and classify mismatches as timing vs. bug.
- [ ] `sample_main`: export top-10 levels on every book update to `data/processed/` for Phase 2.
- [ ] Benchmarks on one full day of BTC: updates/sec, ns per update (p50/p99).
- [ ] *Stretch, optional:* array-based price ladder vs `std::map` — benchmark both and write up the trade-off.

**Definition of done:** all tests pass under ASan/UBSan; validation report written with match rate explained; benchmark numbers in the README.
**Resume checkpoint:** replace the existing LOB line with this engine.

---

## Phase 2 — Signal research · 5 Nov → 9 Dec  ★ core

- [ ] **Data QA** (`qa.py`): gap windows, crossed books, stale periods, outliers. Excluded windows are listed explicitly.
- [ ] **Split** by date: train / validation / test. Write the dates in `research_log.md` before running anything.
- [ ] **Labels:** forward mid-price returns at 1s, 5s, 30s, 1m, 5m (clock time) and at k book updates (event time).
- [ ] **Features:**
  - [ ] L1 order flow imbalance (Cont–Kukanov–Stoikov), aggregated over a window
  - [ ] Multi-level OFI (levels 1–5)
  - [ ] Queue imbalance at L1 and L1–5
  - [ ] Microprice minus mid (Stoikov)
  - [ ] Signed trade-flow imbalance from aggTrades
- [ ] **Univariate evaluation:** Spearman IC and regression slope/R² per feature per horizon → **decay curves**.
- [ ] **Regime splits:** realized-vol terciles; Asia / Europe / US sessions; weekday vs weekend (crypto-specific).
- [ ] **Cross-symbol:** BTC vs ETH vs mid-cap. Does predictability rise as liquidity falls?
- [ ] **Combined model:** ridge regression on all features; walk-forward out-of-sample on validation.
- [ ] **Economics:** how often does the predicted move exceed half-spread + taker fee? (Taker economics vs. maker skew — this motivates Phase 3.)
- [ ] **Test period:** run once, at the end. Record the result whatever it is.
- [ ] Count the variants tried; record it for the write-up.
- [ ] Draft write-up sections: data, method, results.

**Definition of done:** decay plot, out-of-sample table, regime table, a **one-sentence central finding**, and a limitations list (include: Binance `@depth@100ms` is aggregated, so true event-level OFI is not observable).
**Resume checkpoint:** add the central finding.

---

## Phase 3 — Market maker on real replay · 10 Dec → 13 Jan

- [ ] `strategy.hpp` interface: `on_book`, `on_trade`, `on_fill`, `on_timer` → quote intents.
- [ ] `order_sim.hpp`: configurable latency (fixed + jitter) for placements and cancels.
- [ ] `queue_model.hpp`: on placement, queue-ahead = displayed size at that price; decrement by trades at that price; cancellations handled under an explicit assumption. Implement **pessimistic and optimistic** variants and report both.
- [ ] Fees from config (maker / taker). Check current Binance fee tiers; never hard-code.
- [ ] **Decision doc:** Avellaneda–Stoikov assumes a finite horizon; perps don't have one. Choose rolling horizon vs. the Guéant–Lehalle–Fernandez-Tapia stationary approximation, and justify.
- [ ] Baseline: port AS from your existing options market-maker project (re-derive it for perps — don't copy blindly).
- [ ] Signal skew: shift the reservation price by β × predicted move from the Phase 2 model (trained on the train period only).
- [ ] `pnl.hpp`: inventory, realized and mark-to-market PnL; **decomposition** into spread capture, adverse selection (mark-outs at 1s / 5s / 30s after each fill), and inventory PnL.
- [ ] Experiments: baseline vs. skew × symbols × regimes × latency × queue assumption. Use the daily `experiments/` habit.

**Definition of done:** comparison table; mark-out curves for baseline vs. skew; a clear answer to *"what fraction of the signal survives execution"*, with sensitivity to fill assumptions.

---

## Phase 4 — Write-up & interview readiness · 14 Jan → 27 Jan

- [ ] Research note, 10–15 pages: question, data, methodology, results, limitations, what next.
- [ ] README: architecture diagram, key figures, exact commands to reproduce.
- [ ] Final resume line (formal English, numbers filled in).
- [ ] `docs/INTERVIEW_PREP.md`: ~25 likely questions with your own answers. Must include:
  - How does book desync happen and how did you detect/recover?
  - Why fixed-point prices?
  - Define OFI. Why should it predict returns? Why does it decay?
  - How did you prevent look-ahead bias?
  - Which AS assumptions break in crypto, and what did you do about it?
  - What does your queue model assume, and how wrong could it be?
  - What was your most surprising result? What failed?
  - What would you do with more data or more time?
- [ ] Mock-explain the whole project out loud in 5 minutes, then in 30.

---

## Weekly rhythm
- **Sunday, 30 min:** tick boxes here, update `research_log.md`, update "Current phase" in `CLAUDE.md`.
- New ideas go to `docs/ideas.md`, not into the code.

## Risks
| Risk | Mitigation |
|---|---|
| Binance blocked from Singapore | VPS in an allowed region, or Bybit/OKX (decided in Phase 0) |
| Recorder downtime | VPS + health report; Tardis samples as backup |
| Exam season | Slip Phase 3, protect Phase 2 |
| Scope creep | `ideas.md` parking lot; nothing new until Phase 4 is done |
| Signal turns out weak | That is a valid finding — the write-up explains why |

## Reading list (only what the project uses)
- Cont, Kukanov & Stoikov (2014), *The Price Impact of Order Book Events* — OFI.
- Stoikov (2018), *The Micro-Price* — microprice.
- Avellaneda & Stoikov (2008), *High-Frequency Trading in a Limit Order Book* — baseline market maker.
- Guéant, Lehalle & Fernandez-Tapia (2013), *Dealing with the Inventory Risk* — stationary approximation.
- Cartea, Jaimungal & Penalva (2015), *Algorithmic and High-Frequency Trading* — reference chapters on market making and order flow.
- Binance USDⓈ-M docs: *How to manage a local order book correctly*.
