#pragma once
#include <gmock/gmock.h>

#include <expected>

namespace std {
template <class T, class E>
void PrintTo(const std::expected<T, E> &expected, ostream *os) {
  *os << "std::expected{";
  if (expected.has_value()) {
    if constexpr (std::is_same_v<void, T>) {
      *os << "value: void";
    } else {
      *os << "value:" << expected.value();
    }

  } else {
    *os << "error:" << expected.error();
  }
  *os << "}";
}
}  // namespace std

namespace match_search::testing {

/**
 * Check if a `std::expected` contains a value without investigating its
 * content.
 */
MATCHER(IsExpected, "expected has no error") { return arg.has_value(); }

/**
 * Verify if `std::expected` contains a value and its content matches `matcher`.
 */
MATCHER_P(IsExpected, matcher, "") {
  return arg.has_value() &&
         ExplainMatchResult(matcher, arg.value(), result_listener);
}

/**
 * Check if a `std::expected` contains an error without investigating its
 * content.
 */
MATCHER(HasError, "expected has an error") { return !arg.has_value(); }

/**
 * Verify if `std::expected` contains an and its content matches `matcher`.
 */
MATCHER_P(HasError, matcher, "") {
  return !arg.has_value() &&
         ExplainMatchResult(matcher, arg.error(), result_listener);
}
}  // namespace match_search::testing