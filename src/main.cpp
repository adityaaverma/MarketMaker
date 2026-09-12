#include <iostream>
#include <memory>
#include <string>

#include "ExperimentConfig.hpp"
#include "PriceSimulator.hpp"
#include "Option.hpp"
#include "FairValueModel.hpp"
#include "StockFairValueModel.hpp"
#include "OptionFairValueModel.hpp"
#include "Quoter.hpp"
#include "AvellanedaStoikovQuoter.hpp"
#include "NaiveQuoter.hpp"
#include "MarketMaker.hpp"
#include "OrderArrivalModel.hpp"
#include "Simulation.hpp"

int main(int argc, char** argv) {
    if (argc < 3) {
        std::cerr << "Usage: " << argv[0] << " <config.json> <output.csv>\n";
        return 1;
    }
    std::string configPath = argv[1];
    std::string outputPath = argv[2];

    ExperimentConfig cfg(configPath);

    // --- shared market parameters ---
    double s0 = cfg.getNumber("s0", 100.0);
    double mu = cfg.getNumber("mu", 0.0);
    double sigma = cfg.getNumber("sigma", 0.2);
    double dt = cfg.getNumber("dt", 0.005);
    int nSteps = static_cast<int>(cfg.getNumber("n_steps", 200));
    unsigned int seed = static_cast<unsigned int>(cfg.getNumber("seed", 42));
    double horizon = dt * nSteps;

    // --- order flow parameters ---
    double A = cfg.getNumber("arrival_A", 140.0);
    double k = cfg.getNumber("arrival_k", 1.5);
    unsigned int arrivalSeed = static_cast<unsigned int>(cfg.getNumber("arrival_seed", seed + 1));

    GBMPriceSimulator priceSim(s0, mu, sigma, seed);

    std::string mode = cfg.getString("mode", "stock");
    std::unique_ptr<FairValueModel> fvModel;
    if (mode == "option") {
        double strike = cfg.getNumber("strike", s0);
        double rate = cfg.getNumber("rate", 0.02);
        double optionVol = cfg.getNumber("option_vol", sigma);
        std::string typeStr = cfg.getString("option_type", "call");
        OptionType type = (typeStr == "put") ? OptionType::Put : OptionType::Call;
        double optionTimeToExpiry = cfg.getNumber("time_to_expiry", 0.25); // e.g. ~3 months
        Option option(strike, rate, optionVol, type);
        fvModel = std::make_unique<OptionFairValueModel>(priceSim, option, sigma, horizon,
                                                           optionTimeToExpiry);
    } else {
        fvModel = std::make_unique<StockFairValueModel>(priceSim, sigma, horizon);
    }

    std::string quoterType = cfg.getString("quoter", "as");
    std::unique_ptr<Quoter> quoter;
    if (quoterType == "naive") {
        double halfSpread = cfg.getNumber("naive_half_spread", 0.1);
        quoter = std::make_unique<NaiveQuoter>(halfSpread);
    } else {
        double gamma = cfg.getNumber("gamma", 0.1);
        double kappa = cfg.getNumber("kappa", 1.5);
        quoter = std::make_unique<AvellanedaStoikovQuoter>(gamma, kappa);
    }

    MarketMaker mm(*quoter,
                   static_cast<int>(cfg.getNumber("starting_inventory", 0)),
                   cfg.getNumber("starting_cash", 0.0));
    OrderArrivalModel arrivals(A, k, arrivalSeed);

    Simulation sim(*fvModel, mm, arrivals, dt, nSteps);
    SimulationResult result = sim.run(outputPath);

    std::cout << "mode=" << mode << " quoter=" << quoterType
              << " steps=" << result.steps
              << " final_inventory=" << result.finalInventory
              << " final_cash=" << result.finalCash
              << " final_mark_to_market_pnl=" << result.finalPnL << "\n";

    return 0;
}
