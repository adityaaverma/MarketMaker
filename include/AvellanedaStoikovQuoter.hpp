#pragma once
#include "Quoter.hpp"

// Classic Avellaneda & Stoikov (2008) reservation price + optimal spread
// quoter. Skews the reservation price away from fair value in proportion to
// inventory, and widens the spread with risk aversion, volatility and time
// remaining.
class AvellanedaStoikovQuoter : public Quoter {
public:
    AvellanedaStoikovQuoter(double gamma, double kappa);

    Quote computeQuote(double fairValue, int inventory, double sigma,
                        double timeRemaining) const override;

    double reservationPrice(double fairValue, int inventory, double sigma,
                             double timeRemaining) const;
    double optimalSpread(double sigma, double timeRemaining) const;

private:
    double gamma_; // risk aversion
    double kappa_; // order arrival intensity decay (order book liquidity proxy)
};
