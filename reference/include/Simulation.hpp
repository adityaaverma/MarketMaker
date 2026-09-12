#pragma once
#include <string>
#include "FairValueModel.hpp"
#include "MarketMaker.hpp"
#include "OrderArrivalModel.hpp"

struct SimulationResult {
    int steps;
    double finalPnL;
    int finalInventory;
    double finalCash;
};

// Orchestrates one run: at each step it reads the fair value model, asks the
// market maker for a quote, samples order arrivals against that quote,
// updates inventory/cash on a fill, logs a CSV row, then advances the fair
// value model by one time step.
class Simulation {
public:
    Simulation(FairValueModel& fvModel, MarketMaker& mm, OrderArrivalModel& arrivalModel,
               double dt, int nSteps);

    SimulationResult run(const std::string& outputCsvPath);

private:
    FairValueModel& fvModel_;
    MarketMaker& mm_;
    OrderArrivalModel& arrivalModel_;
    double dt_;
    int nSteps_;
};
