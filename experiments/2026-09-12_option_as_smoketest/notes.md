# option_as_smoketest

First run of the options extension: quoting a 3-month ATM call (S0=100,
K=100, r=2%, sigma=20%) with AS, session horizon T=1 (one quoting session,
same dimensionless clock as the stock experiments) decoupled from the
option's own 0.25y time-to-expiry.

Bug caught before this run: originally fed the option's real time-to-expiry
(2 years, from reusing dt*n_steps) into the AS spread formula. Since the
option's price volatility proxy (sigma_V = |delta|*sigma_S*S ~= 10.8, ~25x
the raw stock's dimensionless sigma=2 used in stock_as_baseline) gets
squared in the spread formula, this produced negative bids (quotes crossing
below zero) with gamma=0.1. Fixed by separating session-time (drives the AS
formula) from option-time-to-expiry (drives Black-Scholes pricing), and by
cutting gamma to 0.005 to compensate for sigma_V being ~25x larger than the
stock case.

Observed: avg_spread=1.49 (comparable to the stock baseline, by design),
but inventory drifted to +7 (vs -2..+3 for the stock case) and max_spread
spiked to 2.67 intra-run when inventory grew. Real finding: naively porting
stock-calibrated gamma/kappa to an option book doesn't work -- sigma_V
depends on delta, which moves with the underlying, so effective risk
aversion is not constant the way it is for a stock. Calibrating gamma
against a *time-varying* sigma_V is the real Milestone 2 problem, not a
fixed rescale.
