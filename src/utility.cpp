#include <stdexcept>
#include <iomanip>
#include <iostream>
#include "utility.hpp"

namespace colugo {

void colugo_assertion_failed(char const * expr, char const * function, char const * file, long line) {
    colugo_print(std::cerr,
            "\nAssertion failed:",
            "\n  expr: " , expr,
            "\n  func: " , function,
            "\n  file: " , file,
            "\n  line: " , line,
            "\n");
#if defined(ASSERT_RAISES_EXCEPTION) && ASSERT_RAISES_EXCEPTION
    throw std::runtime_error("Assertion Error");
#else
    std::exit(1);
#endif
}

void colugo_assert_approx_eq_failed(char const * x,
        double val_x,
        const char * y,
        double val_y,
        char const * function,
        char const * file,
        long line) {
    colugo_print(std::cerr,
            "\nApproximately equal assertion failed:",
            "\n  " , x , " (" , std::fixed , std::setprecision(20) , val_x , ") approximately equal to " , y , " (" , std::fixed , std::setprecision(20) , val_y , ")",
            "\n  func: " , function,
            "\n  file: " , file,
            "\n  line: " , line,
            "\n");
#if defined(ASSERT_RAISES_EXCEPTION) && ASSERT_RAISES_EXCEPTION
    throw std::runtime_error("Assertion Error");
#else
    std::exit(1);
#endif
}

} // namespace colugo

