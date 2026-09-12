#include "AvellanedaStoikovQuoter.hpp"
#include <cmath>

AvellanedaStoikovQuoter::AvellanedaStoikovQuoter(double gamma, double kappa)
    : gamma_(gamma), kappa_(kappa) {}

double AvellanedaStoikovQuoter::reservationPrice(double fairValue, int inventory,
                                                  double sigma, double timeRemaining) const {
    return fairValue - inventory * gamma_ * sigma * sigma * timeRemaining;
}

double AvellanedaStoikovQuoter::optimalSpread(double sigma, double timeRemaining) const {
    return gamma_ * sigma * sigma * timeRemaining
         + (2.0 / gamma_) * std::log(1.0 + gamma_ / kappa_);
}

Quote AvellanedaStoikovQuoter::computeQuote(double fairValue, int inventory, double sigma,
                                             double timeRemaining) const {
    double r = reservationPrice(fairValue, inventory, sigma, timeRemaining);
    double halfSpread = optimalSpread(sigma, timeRemaining) / 2.0;
    return Quote{r - halfSpread, r + halfSpread};
}
