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

#ifndef COLUGO_STREAM_HPP
#define COLUGO_STREAM_HPP

#include <ctime>
#include <iostream>

namespace colugo { namespace stream {

inline void write(std::ostream & out) {}

template <typename T>
inline void write(std::ostream & out, const std::vector<T> & arg1) {
    for (unsigned long idx = 0; idx < arg1.size(); ++idx) {
        if (idx > 0) {
            out << ", ";
        }
        out << arg1[idx];
    }
}

template <typename T>
inline void write(std::ostream & out, const T& arg1) {
    out << arg1;
}

template <typename T, typename... Types>
inline void write(std::ostream & out, const T& arg1, const Types&... args) {
    out << arg1;
    write(out, args...);
}

template <typename T, typename... Types>
inline void write(std::ostream & out, const std::vector<T> & arg1, const Types&... args) {
    write(out, arg1);
    write(out, args...);
}

} } // colugo::stream

#endif
