#pragma once

// Abstract base: produces the reference "fair value" the market maker
// quotes around, plus an estimate of that fair value's own volatility, and
// advances by one time step. Letting this be polymorphic is what lets the
// same Quoter/MarketMaker/Simulation machinery drive either a plain stock
// market maker or an options market maker.
class FairValueModel {
public:
    virtual ~FairValueModel() = default;
    virtual double fairValue() const = 0;
    virtual double volatility() const = 0;
    virtual double timeRemaining() const = 0;
    virtual void advance(double dt) = 0;
};
