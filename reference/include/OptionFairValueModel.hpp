#pragma once
#include "FairValueModel.hpp"
#include "PriceSimulator.hpp"
#include "Option.hpp"

// Fair value model where the market maker quotes a single option contract
// on the underlying instead of the underlying itself.
//
// Two different "time" quantities matter here and must not be conflated:
//  - sessionTimeRemaining: how much of the market maker's own quoting
//    session (e.g. one trading day) is left. This is what drives the
//    Avellaneda-Stoikov reservation price / spread formula.
//  - optionTimeToExpiry: how much of the option contract's own life is
//    left (e.g. 30-90 days). This drives the Black-Scholes price and
//    delta used to mark the position and approximate its volatility.
// A short session on a much longer-dated option is the normal case for an
// options market maker and is why these two clocks are tracked separately.
//
// The option's own price volatility is approximated with the delta-normal
// approximation: sigma_V ~= |delta| * sigma_S * S.
class OptionFairValueModel : public FairValueModel {
public:
    OptionFairValueModel(PriceSimulator& sim, Option option, double sigmaUnderlying,
                          double sessionTimeRemaining, double optionTimeToExpiry);

    double fairValue() const override;
    double volatility() const override;
    double timeRemaining() const override { return sessionTimeRemaining_; }
    void advance(double dt) override;

    double delta() const;
    double optionTimeToExpiry() const { return optionTimeToExpiry_; }

private:
    PriceSimulator& sim_;
    Option option_;
    double sigmaS_;
    double sessionTimeRemaining_;
    double optionTimeToExpiry_;
};
