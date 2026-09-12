#pragma once
#include "Quoter.hpp"

// Tracks a market maker's inventory, cash and mark-to-market PnL as it
// quotes against arriving order flow. Holds a reference to a Quoter so the
// quoting strategy (Avellaneda-Stoikov, naive, or any future strategy) is
// swappable without changing this class.
class MarketMaker {
public:
    MarketMaker(const Quoter& quoter, int startingInventory = 0, double startingCash = 0.0);

    Quote quote(double fairValue, double sigma, double timeRemaining) const;

    void onBuyFilled(double price);   // a counterparty sold to us at `bid`
    void onSellFilled(double price);  // a counterparty bought from us at `ask`

    int inventory() const { return inventory_; }
    double cash() const { return cash_; }
    double markToMarket(double fairValue) const;

private:
    const Quoter& quoter_;
    int inventory_;
    double cash_;
};
