#ifndef COLUGO_UTILITY_HPP
#define COLUGO_UTILITY_HPP

#include <iostream>
#include <cmath>

namespace colugo {

//////////////////////////////////////////////////////////////////////////////
// Asserts

void colugo_assertion_failed(char const * expr, char const * function, char const * file, long line);
void colugo_assert_approx_eq_failed(char const * x, double val_x, const char * y, double val_y, char const * function, char const * file, long line);

#if defined(IGNORE_COLUGO_ASSERT) || defined(NDEBUG)
#   define COLUGO_ASSERT(expr)
#   define COLUGO_ASSERT_APPROX_EQUAL(x, y)
#else
#   define COLUGO_ASSERT(expr)  if (!(expr)) colugo::colugo_assertion_failed((const char *)#expr, (const char *)__FUNCTION__, __FILE__, __LINE__)
#   define COLUGO_ASSERT_APPROX_EQUAL(x, y)  if (fabs(((x)-(y))/(x)) > 1.0e-6) std::cerr << std::fixed << std::setprecision(20) << (x) << ' ' << std::fixed << std::setprecision(20) << (y) << '\n'; if (fabs(((x)-(y))/(x)) > 1.0e-6) colugo::colugo_assert_approx_eq_failed((const char *)#x, x, (const char *)#y, y, (const char *)__FUNCTION__, __FILE__, __LINE__)
#endif

#define COLUGO_NDEBUG_ASSERT(expr)  if (!(expr)) colugo::colugo_assertion_failed((const char *)#expr, (const char *)__FUNCTION__, __FILE__, __LINE__)
#define COLUGO_NDEBUG_ASSERT_APPROX_EQUAL(x, y)  if (fabs(((x)-(y))/(x)) > 1.0e-6) std::cerr << std::fixed << std::setprecision(20) << (x) << ' ' << std::fixed << std::setprecision(20) << (y) << '\n'; if (fabs(((x)-(y))/(x)) > 1.0e-6) colugo::colugo_assert_approx_eq_failed((const char *)#x, x, (const char *)#y, y, (const char *)__FUNCTION__, __FILE__, __LINE__)

//////////////////////////////////////////////////////////////////////////////
// Messaging

template <typename S>
void colugo_print(S&) {}

template <typename S, typename T, typename... Types>
void colugo_print(S& stream, const T& arg1, const Types&... args) {
        stream << arg1;          // print first argument
        colugo_print(stream, args...);   // call colugo_stderr() for remaining arguments }
}

template <typename... Types>
void colugo_abort(const Types&... args) {
    colugo_print(std::cerr, args...);
}

} // colugo

#endif
