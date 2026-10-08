#pragma once

// TODO (Phase 1):
// - Side (buy/sell)
// - Price: int64_t ticks — never a double as the book's price key
// - Qty: int64_t lots
// - Timestamp: int64_t nanoseconds
// - decimal string -> integer ticks conversion (unit-test tricky inputs:
//   trailing zeros, negative exponents, values that don't divide evenly by
//   tick size)
