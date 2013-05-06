///////////////////////////////////////////////////////////////////////////////
//
// Copyright 2013 Jeet Sukumaran.
//
// This program is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 3 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License along
// with this program. If not, see <http://www.gnu.org/licenses/>.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef COLUGO_CONSOLE_HPP
#define COLUGO_CONSOLE_HPP

#include <iostream>
#include "textutil.hpp"
#include "stream.hpp"

namespace colugo { namespace console {

///////////////////////////////////////////////////////////////////////////////
// Printing

template <typename... Types>
inline void out(const Types&... args) {
    colugo::stream::write(std::cout, args...);
}

template <typename... Types>
inline void out_line(const Types&... args) {
    colugo::console::out(args...);
    std::cout << std::endl;
}

template <typename... Types>
inline void out_wrapped(const Types&... args) {
    std::ostringstream oss;
    colugo::stream::write(oss, args...);
    auto text = colugo::textutil::textwrap(oss.str(), 78);
    std::cout << text;
    std::cout << std::endl;
}

template <typename... Types>
inline void err(const Types&... args) {
    colugo::stream::write(std::cerr, args...);
}

template <typename... Types>
inline void err_line(const Types&... args) {
    colugo::console::err(args...);
    std::cerr << std::endl;
}

template <typename... Types>
inline void err_wrapped(const Types&... args) {
    std::ostringstream oss;
    colugo::stream::write(oss, args...);
    auto text = colugo::textutil::textwrap(oss.str(), 78);
    std::cerr << text;
    std::cerr << std::endl;
}

///////////////////////////////////////////////////////////////////////////////
// Program control

template <typename... Types>
inline void abort(const Types&... args) {
    colugo::console::err(std::cerr, args...);
    std::cerr << std::endl;
    exit(EXIT_FAILURE);
}

} } // colugo::console

#endif
