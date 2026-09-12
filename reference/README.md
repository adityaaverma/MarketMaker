# Options Market Maker (Avellaneda-Stoikov)

A C++ simulation of an inventory-aware market maker, built to develop
real market-making intuition (spread/inventory tradeoffs, risk-aversion
calibration) rather than just to reproduce a paper's plots. Sequenced after
the Python vol trading backtest (SPY IV vs realized vol) and aligned with
Mini-Term 2 OOP coursework.

## Architecture

Small, swappable pieces, wired together in `main.cpp` from a JSON config:

- `PriceSimulator` (abstract) / `GBMPriceSimulator` -- generates the
  underlying's price path.
- `Option` -- Black-Scholes price + Greeks (delta, gamma, vega, theta),
  evaluated fresh at each time step from the current spot and remaining
  time-to-expiry.
- `FairValueModel` (abstract) -- the reference price being quoted, plus an
  estimate of its own volatility and time remaining:
  - `StockFairValueModel` -- fair value = spot, quoting the underlying
    directly (the textbook single-asset AS setup).
  - `OptionFairValueModel` -- fair value = Black-Scholes price of a single
    option contract; volatility is approximated via the delta-normal
    approximation `sigma_V ~= |delta| * sigma_S * S`. Tracks **two separate
    clocks**: the market maker's own short session horizon (drives the AS
    formula) and the option's real time-to-expiry (drives BS pricing) --
    conflating these two was the first real bug this project caught (see
    `INTUITION_LOG.md`).
- `Quoter` (abstract) -- turns (fair value, inventory, sigma, time
  remaining) into a bid/ask:
  - `AvellanedaStoikovQuoter` -- classic reservation price + optimal spread.
  - `NaiveQuoter` -- constant symmetric spread, no inventory awareness.
    Baseline for every AS experiment.
- `OrderArrivalModel` -- Poisson fill probability decaying exponentially
  with distance from fair value (`lambda(delta) = A * exp(-k*delta)`).
- `MarketMaker` -- owns a `Quoter` reference, tracks inventory/cash/PnL.
- `Simulation` -- the run loop: quote -> sample arrivals -> fill -> log a
  CSV row -> advance the fair value model. Runs the same loop regardless of
  which `FairValueModel`/`Quoter` are plugged in.
- `ExperimentConfig` -- tiny hand-rolled flat-JSON parser (no external
  dependency) so experiments are driven by a `config.json` file rather than
  recompiling.

## Build & run

```
make                                   # builds build/market_maker
./build/market_maker <config.json> <output.csv>
```

Or, to log it properly as an experiment (recommended -- this is the daily
habit):

```
./scripts/run_experiment.sh configs/stock_as.json my_experiment_name
```

This creates `experiments/YYYY-MM-DD_my_experiment_name/` containing the
config used, the raw `output.csv`, a `chart.png` (fair value/quotes,
inventory, mark-to-market PnL over time), a `run.log`, and a `notes.md`
stub to fill in with the one-paragraph why/what-happened. Add a one-line
takeaway to `INTUITION_LOG.md` after.

## Config fields

| field | meaning | default |
|---|---|---|
| `mode` | `"stock"` or `"option"` | `stock` |
| `quoter` | `"as"` or `"naive"` | `as` |
| `s0`, `mu`, `sigma` | underlying GBM start price, drift, annualized vol | 100, 0, 0.2 |
| `dt`, `n_steps` | time step and step count (session horizon = dt*n_steps) | 0.005, 200 |
| `seed` | RNG seed for the price path | 42 |
| `arrival_A`, `arrival_k` | order arrival intensity params | 140, 1.5 |
| `gamma`, `kappa` | AS risk aversion, AS liquidity param (quoter=as only) | 0.1, 1.5 |
| `naive_half_spread` | half-spread (quoter=naive only) | 0.1 |
| `strike`, `rate`, `option_type`, `option_vol`, `time_to_expiry` | option params (mode=option only) | s0, 0.02, call, sigma, 0.25 |

`configs/` holds starter templates: `stock_as.json`, `stock_naive.json`
(spread-matched baseline), `option_as.json`.

## Roadmap

- **Milestone 1 (done):** single-asset AS quoter, validated against the
  closed-form spread formula and against a spread-matched naive baseline.
- **Milestone 2 (in progress):** options extension via the delta-normal
  volatility approximation. Open problem surfaced on day 1: gamma/kappa
  calibrated for a stock don't transfer to an option book because
  `sigma_V = |delta| * sigma_S * S` moves with the underlying -- effective
  risk aversion isn't constant. Next experiments should sweep gamma against
  moneyness/delta to find a calibration that holds up as delta moves.
- **Future:** delta-hedging the underlying to isolate pure market-making
  PnL from directional option PnL; multi-seed statistics instead of
  single-path snapshots; a real limit order book instead of the Poisson
  arrival approximation; replacing the constant-vol GBM with a stochastic
  vol or jump model.

See `INTUITION_LOG.md` for the running list of what each experiment
actually taught.
