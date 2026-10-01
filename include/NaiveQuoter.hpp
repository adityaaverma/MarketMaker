#pragma once
class Quoter
{

    double fairValue;
    double halfSpread;

public:
    Quoter(double fairValue_, double halfSpread_);
    void update(double newPrice);
    double bidValue() const;
    double askValue() const;
};