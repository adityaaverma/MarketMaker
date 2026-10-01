#include "OrderArrivalModel.hpp"
#include <cmath>

using namespace std;

OrderArrivalModel::OrderArrivalModel(double A_, double k_,
                                     double dt_, unsigned seed_)
    : A{A_}, k{k_},
      dt{dt_}, mtgen{seed_}, u(0, 1) {}

double OrderArrivalModel::fillProbability(double delta) const
{
    double lambda = A * exp(-k * delta);
    return 1 - exp(-lambda * dt);
}

bool OrderArrivalModel::checkFill(double delta)
{
    return u(mtgen) < fillProbability(delta);
}