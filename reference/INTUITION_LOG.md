# Intuition Log

Rolling one-line takeaway per experiment. Full detail lives in each
experiment's `notes.md`; this file is the fast-scan summary.

- 2026-09-12 `stock_as_baseline`: AS spread formula matches theory exactly
  (1.29-1.69 over the session, floor = `(2/gamma)ln(1+gamma/kappa)`) --
  engine is correct before touching options.
- 2026-09-12 `stock_naive_baseline`: same price path, spread matched to AS
  at t=0, but naive (no inventory skew) had 43% higher mark-to-market PnL
  std (91.17 vs 52.19). Inventory-aware skewing genuinely reduces variance,
  it isn't just theoretical -- but this is one seed, needs a multi-seed
  sweep to trust the number.
- 2026-09-12 `option_as_smoketest`: conflating an option's real
  time-to-expiry with the AS session-horizon parameter blows up the spread
  formula (sigma_V is squared, and sigma_V for an option is way bigger than
  a stock's raw sigma). Stock-calibrated gamma does not transfer to options
  -- sigma_V = |delta|*sigma_S*S moves with the underlying, so effective
  risk aversion isn't constant like it is for a stock. This is the real
  open problem for Milestone 2, not a one-time rescale of gamma.
