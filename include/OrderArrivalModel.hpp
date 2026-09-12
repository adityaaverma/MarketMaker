#pragma once
#include <random>

// Poisson order-arrival model: the probability that a resting quote is hit
// within dt decays exponentially with its distance from fair value,
// lambda(delta) = A * exp(-k * delta). This is the standard stylised model
// of limit order flow used in the Avellaneda-Stoikov paper.
class OrderArrivalModel {
public:
    OrderArrivalModel(double A, double k, unsigned int seed);

    // Returns true if a counterparty order arrives and hits this quote
    // within the interval dt.
    bool arrives(double distanceFromFair, double dt);

private:
    double A_;
    double k_;
    std::mt19937 rng_;
    std::uniform_real_distribution<double> uniform_;
};
