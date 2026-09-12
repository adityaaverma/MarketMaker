#include "StockFairValueModel.hpp"
#include <algorithm>

StockFairValueModel::StockFairValueModel(PriceSimulator& sim, double sigma, double horizon)
    : sim_(sim), sigma_(sigma), timeRemaining_(horizon) {}

double StockFairValueModel::fairValue() const {
    return sim_.price();
}

double StockFairValueModel::volatility() const {
    return sigma_;
}

void StockFairValueModel::advance(double dt) {
    sim_.step(dt);
    timeRemaining_ = std::max(timeRemaining_ - dt, 0.0);
}
