#include "Simulation.hpp"
#include <fstream>
#include <stdexcept>

Simulation::Simulation(FairValueModel& fvModel, MarketMaker& mm, OrderArrivalModel& arrivalModel,
                        double dt, int nSteps)
    : fvModel_(fvModel), mm_(mm), arrivalModel_(arrivalModel), dt_(dt), nSteps_(nSteps) {}

SimulationResult Simulation::run(const std::string& outputCsvPath) {
    std::ofstream out(outputCsvPath);
    if (!out) {
        throw std::runtime_error("Simulation: could not open output file " + outputCsvPath);
    }
    out << "step,time,fair_value,sigma,bid,ask,inventory,cash,mark_to_market\n";

    for (int t = 0; t < nSteps_; ++t) {
        double fv = fvModel_.fairValue();
        double sigma = fvModel_.volatility();
        double timeRemaining = fvModel_.timeRemaining();

        Quote q = mm_.quote(fv, sigma, timeRemaining);

        // A counterparty selling to us lifts our bid; a counterparty buying
        // from us hits our ask. Each side is an independent Poisson draw
        // whose intensity decays with distance from fair value.
        if (arrivalModel_.arrives(fv - q.bid, dt_)) {
            mm_.onBuyFilled(q.bid);
        }
        if (arrivalModel_.arrives(q.ask - fv, dt_)) {
            mm_.onSellFilled(q.ask);
        }

        out << t << ','
            << (t * dt_) << ','
            << fv << ','
            << sigma << ','
            << q.bid << ','
            << q.ask << ','
            << mm_.inventory() << ','
            << mm_.cash() << ','
            << mm_.markToMarket(fv) << '\n';

        fvModel_.advance(dt_);
    }

    double finalFv = fvModel_.fairValue();
    return SimulationResult{nSteps_, mm_.markToMarket(finalFv), mm_.inventory(), mm_.cash()};
}
