#pragma once
#include <random>

class OrderArrivalModel
{
    double A;
    double k;
    double dt;
    std::mt19937 mtgen;
    std::uniform_real_distribution<double> u;

public:
    OrderArrivalModel(double A_, double k_,double dt_,unsigned seed_);

    double fillProbability(double delta) const;
    bool checkFill(double delta);
};
