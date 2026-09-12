#include "PriceSimulator.hpp"
#include <cmath>

GBMPriceSimulator::GBMPriceSimulator(double s0, double mu, double sigma, unsigned int seed)
    : PriceSimulator(s0), mu_(mu), sigma_(sigma), rng_(seed), normal_(0.0, 1.0) {}

double GBMPriceSimulator::step(double dt) {
    double z = normal_(rng_);
    double drift = (mu_ - 0.5 * sigma_ * sigma_) * dt;
    double diffusion = sigma_ * std::sqrt(dt) * z;
    price_ *= std::exp(drift + diffusion);
    return price_;
}
