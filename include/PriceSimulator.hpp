#pragma once
#include <iostream>
#include <random>
#include <vector>

class PriceSimulator
{
    double S0;
    double mu;
    double sigma;
    double dt;
    std::mt19937 mtgen;
    std::normal_distribution<double> z;

public:
    PriceSimulator(double S0, double mu, double sigma, double dt, unsigned seed);
    double next();
    std::vector<double> simulatePath(size_t steps);
};
