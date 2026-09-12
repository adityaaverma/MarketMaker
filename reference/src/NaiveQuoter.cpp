#include "NaiveQuoter.hpp"

NaiveQuoter::NaiveQuoter(double halfSpread) : halfSpread_(halfSpread) {}

Quote NaiveQuoter::computeQuote(double fairValue, int /*inventory*/, double /*sigma*/,
                                 double /*timeRemaining*/) const {
    return Quote{fairValue - halfSpread_, fairValue + halfSpread_};
}
