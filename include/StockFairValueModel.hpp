#pragma once
#include "FairValueModel.hpp"
#include "PriceSimulator.hpp"

// Fair value model where the market maker quotes the underlying directly
// (the textbook single-asset Avellaneda-Stoikov setup).
class StockFairValueModel : public FairValueModel {
public:
    StockFairValueModel(PriceSimulator& sim, double sigma, double horizon);

    double fairValue() const override;
    double volatility() const override;
    double timeRemaining() const override { return timeRemaining_; }
    void advance(double dt) override;

private:
    PriceSimulator& sim_;
    double sigma_;
    double timeRemaining_;
};
