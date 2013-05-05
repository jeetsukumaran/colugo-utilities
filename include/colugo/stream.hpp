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

inline void write(std::ostream & stream) {}

template <typename T>
inline void write(std::ostream & stream, const std::vector<T> & arg1) {
    for (unsigned long idx = 0; idx < arg1.size(); ++idx) {
        if (idx > 0) {
            stream << ", ";
        }
        stream << arg1[idx];
    }
}

template <typename T>
inline void write(std::ostream & stream, const T& arg1) {
    stream << arg1;
}

template <typename T, typename... Types>
inline void write(std::ostream & stream, const T& arg1, const Types&... args) {
    stream << arg1;
    write(stream, args...);
}

template <typename T, typename... Types>
inline void write(std::ostream & stream, const std::vector<T> & arg1, const Types&... args) {
    write(stream, arg1);
    write(stream, args...);
}

} } // colugo::stream

#endif
