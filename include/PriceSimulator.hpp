#pragma once
#include <random>

// Abstract base: generates the underlying asset's price path one time step
// at a time.
class PriceSimulator {
public:
    explicit PriceSimulator(double s0) : price_(s0) {}
    virtual ~PriceSimulator() = default;

    // Advance the price by one time step of length dt (in years) and return
    // the new price.
    virtual double step(double dt) = 0;

    double price() const { return price_; }

protected:
    double price_;
};

// Geometric Brownian Motion: dS = mu*S*dt + sigma*S*dW
class GBMPriceSimulator : public PriceSimulator {
public:
    GBMPriceSimulator(double s0, double mu, double sigma, unsigned int seed);
    double step(double dt) override;

private:
    double mu_;
    double sigma_;
    std::mt19937 rng_;
    std::normal_distribution<double> normal_;
};
