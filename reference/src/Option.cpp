#include "Option.hpp"
#include <cmath>
#include <algorithm>

Option::Option(double strike, double riskFreeRate, double volatility, OptionType type)
    : strike_(strike), rate_(riskFreeRate), sigma_(volatility), type_(type) {}

double Option::normCdf(double x) {
    return 0.5 * std::erfc(-x / std::sqrt(2.0));
}

double Option::normPdf(double x) {
    static const double invSqrt2Pi = 0.3989422804014327;
    return invSqrt2Pi * std::exp(-0.5 * x * x);
}

void Option::d1d2(double spot, double timeToExpiry, double& d1, double& d2) const {
    double t = std::max(timeToExpiry, 1e-8);
    double sqrtT = std::sqrt(t);
    d1 = (std::log(spot / strike_) + (rate_ + 0.5 * sigma_ * sigma_) * t) / (sigma_ * sqrtT);
    d2 = d1 - sigma_ * sqrtT;
}

double Option::price(double spot, double timeToExpiry) const {
    double d1, d2;
    d1d2(spot, timeToExpiry, d1, d2);
    double t = std::max(timeToExpiry, 1e-8);
    double discountedStrike = strike_ * std::exp(-rate_ * t);
    if (type_ == OptionType::Call) {
        return spot * normCdf(d1) - discountedStrike * normCdf(d2);
    }
    return discountedStrike * normCdf(-d2) - spot * normCdf(-d1);
}

double Option::delta(double spot, double timeToExpiry) const {
    double d1, d2;
    d1d2(spot, timeToExpiry, d1, d2);
    if (type_ == OptionType::Call) return normCdf(d1);
    return normCdf(d1) - 1.0;
}

double Option::gamma(double spot, double timeToExpiry) const {
    double d1, d2;
    d1d2(spot, timeToExpiry, d1, d2);
    double t = std::max(timeToExpiry, 1e-8);
    return normPdf(d1) / (spot * sigma_ * std::sqrt(t));
}

double Option::vega(double spot, double timeToExpiry) const {
    double d1, d2;
    d1d2(spot, timeToExpiry, d1, d2);
    double t = std::max(timeToExpiry, 1e-8);
    return spot * normPdf(d1) * std::sqrt(t);
}

double Option::theta(double spot, double timeToExpiry) const {
    double d1, d2;
    d1d2(spot, timeToExpiry, d1, d2);
    double t = std::max(timeToExpiry, 1e-8);
    double term1 = -(spot * normPdf(d1) * sigma_) / (2.0 * std::sqrt(t));
    if (type_ == OptionType::Call) {
        double term2 = rate_ * strike_ * std::exp(-rate_ * t) * normCdf(d2);
        return term1 - term2;
    }
    double term2 = rate_ * strike_ * std::exp(-rate_ * t) * normCdf(-d2);
    return term1 + term2;
}
