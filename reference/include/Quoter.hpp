#pragma once

struct Quote {
    double bid;
    double ask;
};

// Abstract base: given the current fair value, inventory position,
// volatility of the fair value, and time remaining in the trading horizon,
// produce a bid/ask quote pair.
class Quoter {
public:
    virtual ~Quoter() = default;
    virtual Quote computeQuote(double fairValue, int inventory, double sigma,
                                double timeRemaining) const = 0;
};
