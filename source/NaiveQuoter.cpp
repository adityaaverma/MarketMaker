#include <iostream>
#include "NaiveQuoter.hpp"

Quoter::Quoter(double fairValue_, double halfSpread_) : fairValue{fairValue_}, halfSpread{halfSpread_} {}

double Quoter::askValue() const
{
    return fairValue + halfSpread / 2.0;
}

double Quoter::bidValue() const
{
    return fairValue - halfSpread / 2.0;
}

void Quoter::update(double newPrice)
{
    fairValue = newPrice;
}