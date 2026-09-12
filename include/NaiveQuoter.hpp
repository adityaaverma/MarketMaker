#pragma once
#include "Quoter.hpp"

// Baseline quoter: symmetric constant half-spread around fair value,
// completely ignoring inventory and volatility. Used as the control against
// which the Avellaneda-Stoikov quoter is benchmarked.
class NaiveQuoter : public Quoter {
public:
    explicit NaiveQuoter(double halfSpread);
    Quote computeQuote(double fairValue, int inventory, double sigma,
                        double timeRemaining) const override;

private:
    double halfSpread_;
};
