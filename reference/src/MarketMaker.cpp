#include "MarketMaker.hpp"

MarketMaker::MarketMaker(const Quoter& quoter, int startingInventory, double startingCash)
    : quoter_(quoter), inventory_(startingInventory), cash_(startingCash) {}

Quote MarketMaker::quote(double fairValue, double sigma, double timeRemaining) const {
    return quoter_.computeQuote(fairValue, inventory_, sigma, timeRemaining);
}

void MarketMaker::onBuyFilled(double price) {
    inventory_ += 1;
    cash_ -= price;
}

void MarketMaker::onSellFilled(double price) {
    inventory_ -= 1;
    cash_ += price;
}

double MarketMaker::markToMarket(double fairValue) const {
    return cash_ + inventory_ * fairValue;
}
