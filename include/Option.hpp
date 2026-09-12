#pragma once

enum class OptionType { Call, Put };

// Black-Scholes European option pricer and Greeks. Stateless with respect to
// time -- spot and time-to-expiry are passed in at evaluation time so the
// same Option can be repriced at every simulation step.
class Option {
public:
    Option(double strike, double riskFreeRate, double volatility, OptionType type);

    double price(double spot, double timeToExpiry) const;
    double delta(double spot, double timeToExpiry) const;
    double gamma(double spot, double timeToExpiry) const;
    double vega(double spot, double timeToExpiry) const;
    double theta(double spot, double timeToExpiry) const;

    double strike() const { return strike_; }
    double volatility() const { return sigma_; }

private:
    double strike_;
    double rate_;
    double sigma_;
    OptionType type_;

    static double normCdf(double x);
    static double normPdf(double x);
    void d1d2(double spot, double timeToExpiry, double& d1, double& d2) const;
};
