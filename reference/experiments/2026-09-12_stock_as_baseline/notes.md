# stock_as_baseline

First real run of the Avellaneda-Stoikov quoter on a single stock, using the
parameters from the original 2008 AS paper (sigma=2, gamma=0.1, kappa=1.5,
A=140, T=1 session, dt=0.005, 200 steps, seed=1) to sanity-check the engine
against known behaviour before touching options.

Expected: spread should shrink as t -> T (the `(2/gamma)ln(1+gamma/kappa)`
floor term dominates near expiry), and the reservation price should skew
away from fair value in the direction that reduces inventory.

Observed: avg_spread=1.49, ranging 1.29-1.69 exactly as the closed-form
spread formula predicts (1.69 at t=0 decaying to 1.29 at t=1). Inventory
stayed small (-2 to +3) and mark-to-market PnL std was 52.19 over the run.
Engine matches theory -- good baseline to compare the naive quoter against.
