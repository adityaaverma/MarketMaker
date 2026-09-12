#include "OrderArrivalModel.hpp"
#include <cmath>

OrderArrivalModel::OrderArrivalModel(double A, double k, unsigned int seed)
    : A_(A), k_(k), rng_(seed), uniform_(0.0, 1.0) {}

bool OrderArrivalModel::arrives(double distanceFromFair, double dt) {
    double lambda = A_ * std::exp(-k_ * std::fabs(distanceFromFair));
    double pArrival = 1.0 - std::exp(-lambda * dt);
    return uniform_(rng_) < pArrival;
}
