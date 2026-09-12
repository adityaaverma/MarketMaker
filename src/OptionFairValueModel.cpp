#include "OptionFairValueModel.hpp"
#include <cmath>
#include <algorithm>
#include <utility>

OptionFairValueModel::OptionFairValueModel(PriceSimulator& sim, Option option,
                                            double sigmaUnderlying, double sessionTimeRemaining,
                                            double optionTimeToExpiry)
    : sim_(sim), option_(std::move(option)), sigmaS_(sigmaUnderlying),
      sessionTimeRemaining_(sessionTimeRemaining), optionTimeToExpiry_(optionTimeToExpiry) {}

double OptionFairValueModel::fairValue() const {
    return option_.price(sim_.price(), std::max(optionTimeToExpiry_, 1e-6));
}

double OptionFairValueModel::delta() const {
    return option_.delta(sim_.price(), std::max(optionTimeToExpiry_, 1e-6));
}

double OptionFairValueModel::volatility() const {
    return std::fabs(delta()) * sigmaS_ * sim_.price();
}

void OptionFairValueModel::advance(double dt) {
    sim_.step(dt);
    sessionTimeRemaining_ = std::max(sessionTimeRemaining_ - dt, 0.0);
    optionTimeToExpiry_ = std::max(optionTimeToExpiry_ - dt, 0.0);
}
