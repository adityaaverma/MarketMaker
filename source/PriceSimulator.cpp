#include "PriceSimulator.hpp"
#include <iostream>
#include <cmath>
#include <random>

using namespace std;

PriceSimulator::PriceSimulator(double S0, double mu, double sigma, double dt, unsigned seed)
    : S0{S0}, mu{mu}, sigma{sigma}, dt{dt},
      mtgen{seed},
      z(0, 1) {}

double PriceSimulator::next()
{
    S0 *= exp((mu - pow(sigma, 2) / 2) * dt + sigma * sqrt(dt) * z(mtgen));
    return S0;
}

vector<double> PriceSimulator::simulatePath(size_t steps)
{
    if (steps == 0)
        throw logic_error("Price Cannot have 0 steps");
    vector<double> prices;
    prices.reserve(steps);
    prices.push_back(S0);
    double drift = (mu - pow(sigma, 2) / 2) * dt;
    double vol = sigma * sqrt(dt);

    generate_n(back_inserter(prices), steps - 1, [this, drift, vol, curr_S = prices.back()]() mutable
               { curr_S*=exp(drift+vol*z(mtgen));
                return curr_S; });
    return prices;
}