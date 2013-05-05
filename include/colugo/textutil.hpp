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

#include <ctime>
#include <string>
#include <sstream>
#include <vector>
#include <algorithm>

#if !defined(COLUGO_TEXTUTIL_HPP)
#define COLUGO_TEXTUTIL_HPP

namespace colugo {
namespace textutil {

/**
 * Returns copy of given string with specified characters stripped from
 * beginning and end.
 *
 * @param s         source string
 * @param to_trim  characters to remove
 * @return          copy of source with specified characters removed from
 *                  beginning and end
 */
inline std::string trim(const std::string & s, const std::string & to_trim=" \t\n\r") {
    if (!s.empty()) {
        std::size_t start = s.find_first_not_of(to_trim);
        if (start == std::string::npos) {
            return "";
        }
        std::size_t end = s.find_last_not_of(to_trim);
        return s.substr(start, end-start+1);
    } else {
        return s;
    }
}

/**
 * Converts a string to lower case.
 *
 * @param s         source string
 * @return          copy of string in all lower case characters.
 */
inline std::string lower(const std::string & s) {
    std::string result = s;
    std::transform(s.begin(), s.end(), result.begin(), (int(*)(int))std::tolower);
    return result;
}

/**
 * Converts a string to upper case.
 *
 * @param s         source string
 * @return          copy of string in all upper case characters.
 */
inline std::string upper(const std::string & s) {
    std::string result = s;
    std::transform(s.begin(), s.end(), result.begin(), (int(*)(int))std::toupper);
    return result;
}

/**
 * Returns <code>true</code> if the second string is equal to the first n
 * characters of the first string, where n = length of the second string.
 *
 * @param s1        first string
 * @param s2        second string
 * @return          <code>true</code> if the first string begins with the
 *                  second
 */
inline bool startswith(const std::string & s1, const std::string & s2) {
    return s1.compare(0, s2.size(), s2) == 0;
}

/**
 * Wraps a line of text to specified width.
 *
 * @param  source                   text to be wrapped
 * @param  line_width               width of wrapping
 * @param  first_line_indent        amount to indent first line
 * @param  subsequent_line_indent   amount to indent remaining lines
 * @return                          wrapped text (line breaks="\n")
 */
inline std::string textwrap(const std::string & source,
        unsigned line_width=78,
        unsigned first_line_indent=0,
        unsigned subsequent_line_indent=0) {
    std::string wrapped;
    unsigned col_count = 1;
    unsigned line_count = 1;
    std::string subsequent_line_indent_str(subsequent_line_indent, ' ');
    for (std::string::const_iterator s = source.begin();
            s != source.end();
            ++s, ++col_count) {
        if (*s == '\n') {
            wrapped += "\n";
            col_count = 0;
            line_count += 1;
            continue;
        }
        if (col_count > line_width) {
            std::string::size_type last_break = wrapped.find_last_of("\n");
            std::string::size_type wrap_pos = wrapped.find_last_of(" ");
            if (wrap_pos == std::string::npos || ((last_break != std::string::npos) && (last_break > wrap_pos))) {
                wrapped += "\n";
                col_count = 1;
            } else {
                wrapped.replace(wrap_pos, 1, "\n" + subsequent_line_indent_str);
                col_count = wrapped.size() - wrap_pos;
            }
        }
        if (col_count == 1 && line_count == 1 && first_line_indent > 0) {
            for (unsigned i = 0; i < first_line_indent; ++i) {
                wrapped += ' ';
            }
            col_count += first_line_indent;
        } else if (col_count == 1 && line_count > 1) {
            wrapped += subsequent_line_indent_str;
            col_count += subsequent_line_indent;
        }
        wrapped += *s;

    }
    return wrapped;
}

/**
 * Splits a std::string source string into tokens as delimited by
 * <code>sep</code>.
 *
 * @param src                    source string to be split
 * @param sep                    separator token (single or multiple characters)
 * @param max_splits             maximum number of splits (0 = no limit)
 * @param trim_tokens            strip leading and trailing whitespace from each token
 * @param include_empty_tokens   treat consecutive delimiters as valid (separating empty fields)?
 * @return                       vector of std::string tokens
 */
inline std::vector<std::string> split(
        const std::string & src,
        const std::string & sep = " ",
        unsigned max_splits=0,
        bool trim_tokens=true,
        bool include_empty_tokens=true) {
    std::vector< std::string > v;
    std::string::size_type start_pos = 0;
    std::string::size_type end_pos = src.find(sep, start_pos);
    unsigned num_splits = 0;
    while (end_pos != std::string::npos && (max_splits == 0 || num_splits < max_splits)) {
        std::string result = src.substr(start_pos, end_pos-start_pos);
        if (trim_tokens) {
            result = trim(result, " \t\n\r");
        }
        if (result.size() != 0 || include_empty_tokens) {
            num_splits += 1;
            v.push_back(result);
        }
        start_pos = end_pos+1;
        end_pos = src.find(sep, start_pos);
    }
    std::string result = src.substr(start_pos, std::string::npos);
    if (result.size() != 0 || include_empty_tokens) {
        v.push_back(result);
    }
    return v;
}

/**
 * Splits a const char source string into tokens as delimited by any character
 * given in <code>sep</code>.
 *
 * @param src                    source string to be split
 * @param sep                    separator token (single or multiple characters)
 * @param max_splits             maximum number of splits (0 = no limit)
 * @param trim_tokens            strip leading and trailing whitespace from each token
 * @param include_empty_tokens   treat consecutive delimiters as valid (separating empty fields)?
 * @return                       vector of std::string tokens
 */
inline std::vector<std::string> split_on_any(
        const std::string & src,
        const std::string & sep,
        unsigned max_splits=0,
        bool trim_tokens=true,
        bool include_empty_tokens=true) {
    std::vector< std::string > v;
    std::string::size_type start_pos = 0;
    std::string::size_type end_pos = src.find_first_of(sep, start_pos);
    unsigned num_splits = 0;
    while (end_pos != std::string::npos && (max_splits == 0 || num_splits < max_splits)) {
        std::string result = src.substr(start_pos, end_pos-start_pos);
        if (trim_tokens) {
            result = trim(result, " \t\n\r");
        }
        if (result.size() != 0 || include_empty_tokens) {
            num_splits += 1;
            v.push_back(result);
        }
        start_pos = end_pos+1;
        end_pos = src.find_first_of(sep, start_pos);
    }
    std::string result = src.substr(start_pos, std::string::npos);
    if (result.size() != 0 || include_empty_tokens) {
        v.push_back(result);
    }
    return v;
}

/////////////////////////////////////////////////////////////////////////
// utility to join elements to string

template <typename D, typename S, typename T>
inline void join_to_stream(const D& separator, S& stream, const T& arg1) {
    stream << arg1;
}

template <typename D, typename S, typename T, typename... Types>
inline void join_to_stream(const D& separator, S& stream, const T& arg1, const Types&... args) {
    stream << arg1 << separator;          // print first argument
    join_to_stream(separator, stream, args...);   // call join_to_stream() for remaining arguments }
}

template <typename D, typename... Types>
inline std::string join(const D& separator, const Types&... args) {
    std::ostringstream out;
    concatenate(separator, out, args...);
    return out.str();
}

/////////////////////////////////////////////////////////////////////////
// miscellaneous

inline std::string get_time_string(std::time_t * t = nullptr) {
    std::time_t local_time_struct_;
    char tbuffer[20];
    if (t == nullptr) {
        t = &local_time_struct_;
        std::time(t);
    }
    std::strftime(tbuffer, 20, "%Y-%m-%d %H:%M:%S", std::localtime(t));
    return tbuffer;
}

/////////////////////////////////////////////////////////////////////////

} // namespace textutil
} // namespace colugo

#endif
