#ifndef COLUGO_ASSERT_HPP
#define COLUGO_ASSERT_HPP

#include <iostream>
#include <cmath>
#include <iomanip>
#include "console.hpp"

namespace colugo {

inline void colugo_assertion_failed(char const * expr, char const * function, char const * file, long line) {
    console::err(
            "\nAssertion failed:",
            "\n  expr: " , expr,
            "\n  func: " , function,
            "\n  file: " , file,
            "\n  line: " , line,
            "\n");
#if defined(ASSERT_RAISES_EXCEPTION) && ASSERT_RAISES_EXCEPTION
    throw std::runtime_error("Assertion Error");
#else
    std::exit(EXIT_FAILURE);
#endif
}

inline void colugo_assert_approx_eq_failed(char const * x,
        double val_x,
        const char * y,
        double val_y,
        char const * function,
        char const * file,
        long line) {
    console::err(
            "\nApproximately equal assertion failed:",
            "\n  " , x , " (" , std::fixed , std::setprecision(20) , val_x , ") approximately equal to " , y , " (" , std::fixed , std::setprecision(20) , val_y , ")",
            "\n  func: " , function,
            "\n  file: " , file,
            "\n  line: " , line,
            "\n");
#if defined(ASSERT_RAISES_EXCEPTION) && ASSERT_RAISES_EXCEPTION
    throw std::runtime_error("Assertion Error");
#else
    std::exit(EXIT_FAILURE);
#endif
}

//////////////////////////////////////////////////////////////////////////////
// Asserts

#if defined(IGNORE_COLUGO_ASSERT) || defined(NDEBUG)
#   define COLUGO_ASSERT(expr)
#   define COLUGO_ASSERT_APPROX_EQUAL(x, y)
#else
#   define COLUGO_ASSERT(expr)  if (!(expr)) colugo::colugo_assertion_failed((const char *)#expr, (const char *)__FUNCTION__, __FILE__, __LINE__)
#   define COLUGO_ASSERT_APPROX_EQUAL(x, y)  if (fabs(((x)-(y))/(x)) > 1.0e-6) std::cerr << std::fixed << std::setprecision(20) << (x) << ' ' << std::fixed << std::setprecision(20) << (y) << '\n'; if (fabs(((x)-(y))/(x)) > 1.0e-6) colugo::colugo_assert_approx_eq_failed((const char *)#x, x, (const char *)#y, y, (const char *)__FUNCTION__, __FILE__, __LINE__)
#endif

#define COLUGO_NDEBUG_ASSERT(expr)  if (!(expr)) colugo::colugo_assertion_failed((const char *)#expr, (const char *)__FUNCTION__, __FILE__, __LINE__)
#define COLUGO_NDEBUG_ASSERT_APPROX_EQUAL(x, y)  if (fabs(((x)-(y))/(x)) > 1.0e-6) std::cerr << std::fixed << std::setprecision(20) << (x) << ' ' << std::fixed << std::setprecision(20) << (y) << '\n'; if (fabs(((x)-(y))/(x)) > 1.0e-6) colugo::colugo_assert_approx_eq_failed((const char *)#x, x, (const char *)#y, y, (const char *)__FUNCTION__, __FILE__, __LINE__)

} // colugo

#endif
