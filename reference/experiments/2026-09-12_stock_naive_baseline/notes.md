# stock_naive_baseline

Same price path (seed=1) and same average spread (naive half-spread=0.845,
i.e. full spread 1.69, matched to the AS quoter's t=0 spread) but with zero
inventory awareness, to isolate what the AS skew/inventory term actually
buys you.

Observed: mark-to-market PnL std=91.17 vs AS's 52.19 on the identical price
path -- roughly 43% higher variance with a naive constant spread despite a
similar (slightly wider) average spread. This is the expected AS result:
inventory-aware skewing reduces PnL variance for a similar spread, at the
cost of walking away from the naive strategy's higher realized PnL on this
one lucky seed (286.6 vs 145.9 -- a single-seed number, not evidence naive
is "better," since it's also the higher-variance strategy).

Caveat to self: single seed, single price path. Next step should be a
multi-seed sweep (20-50 seeds) comparing mean and std of mark-to-market PnL
properly before drawing real conclusions.
