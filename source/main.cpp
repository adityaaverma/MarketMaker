#include <iostream>
#include <vector>
#include <fstream>
#include "PriceSimulator.hpp"
#include "NaiveQuoter.hpp"
#include "OrderArrivalModel.hpp"

int main()
{
    PriceSimulator p(100, 0.05, 0.2, 0.01, 42);
    std::vector<double> prices = p.simulatePath(200);

    double halfSpread = 0.5;
    Quoter q(prices[0], halfSpread);

    double A = 22.3;
    double k = 5.54;
    double dt = 0.01;
    OrderArrivalModel orderModel(A, k, dt, 123); // different seed from PriceSimulator's 42

    std::ofstream file("fills_test.csv");
    file << "step,price,bid,ask,bidFilled,askFilled\n";

    int inventory = 0;
    for (size_t i = 0; i < prices.size(); i++)
    {
        q.update(prices[i]);
        double bid = q.bidValue();
        double ask = q.askValue();

        double deltaBid = prices[i] - bid;
        double deltaAsk = ask - prices[i];

        bool bidFilled = orderModel.checkFill(deltaBid);
        bool askFilled = orderModel.checkFill(deltaAsk);

        if (bidFilled) inventory += 1;
        if (askFilled) inventory -= 1;

        file << i << ',' << prices[i] << ',' << bid << ',' << ask << ','
             << bidFilled << ',' << askFilled << '\n';
    }
    file.close();

    std::cout << "final inventory: " << inventory << "\n";
    return 0;
}