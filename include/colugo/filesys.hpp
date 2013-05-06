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

#if !defined(COLUGO_FILESYS_HPP)
#define COLUGO_FILESYS_HPP

#include <cstdlib>
#include <string>
#include <stdexcept>

#include <sys/stat.h>
#include <limits.h>
#include <cstdlib>
#ifdef WINDOWS
    #include <direct.h>
    #define COLUGO_FILESYS_GETCWD _getcwd
    #define COLUGO_FILESYS_STAT _stat
    static const char * COLUGO_FILESYS_PATH_SEPARATOR = "\\";
#else
    #include <unistd.h>
    #define COLUGO_FILESYS_GETCWD getcwd
    #define COLUGO_FILESYS_STAT stat
    static const char * COLUGO_FILESYS_PATH_SEPARATOR = "/";
 #endif
#include <string.h> // for strerror

#include "textutil.hpp"

namespace colugo { namespace filesys {

//////////////////////////////////////////////////////////////////////////////
// Path manipulation

/**
 * Returns filename (and extension) from supplied path.
 *
 * Technically, returns the final element of a "/"-separated path.
 * @param  path path to file
 * @return      filename and extension
 */
inline std::string get_path_leaf(const std::string& path) {
    std::string::size_type last_path_char = path.find_last_of(COLUGO_FILESYS_PATH_SEPARATOR);
    if (last_path_char == std::string::npos) {
        return path;
    } else if (last_path_char == path.size()-1) {
        return get_path_leaf(path.substr(0, path.size()-1).c_str());
    } else {
        if (last_path_char >= path.size()) {
            return std::string("");
        } else {
            return path.substr(last_path_char+1);
        }
    }
}

/**
 * Returns filename (and extension) from supplied path.
 *
 * Technically, returns the final element of a "/"-separated path.
 * @param  path path to file
 * @return      filename and extension
 */
inline std::string get_path_parent(const std::string& path) {
    std::string::size_type last_path_char = path.find_last_of(COLUGO_FILESYS_PATH_SEPARATOR);
    if (last_path_char == std::string::npos) {
        return std::string("");
    } else if (last_path_char == path.size()-1) {
        return get_path_parent(path.substr(0, path.size()-1).c_str());
    } else {
        if (last_path_char >= path.size()) {
            return std::string("");
        } else {
            return path.substr(0, last_path_char);
        }
    }
}

/**
 * Joins elements of a path.
 */
template <typename... Types>
inline std::string path_join(const Types&... args) {
    return colugo::textutil::join(COLUGO_FILESYS_PATH_SEPARATOR, args...);
}

/**
 * Returns <code>true</code> if path is absolute.
 *
 * @param path      file path
 * @return          <code>true</code> if path is absolute
 */
inline bool is_abs_path(const std::string& path) {
    if (path.size() > 0) {
        return COLUGO_FILESYS_PATH_SEPARATOR[0] == path[0];
    } else {
        return false;
    }
}

//////////////////////////////////////////////////////////////////////////////
// Filesystem queries

inline std::string get_cwd() {
    std::string path(1024,'\0');
    while( COLUGO_FILESYS_GETCWD(&path[0], path.size()) == 0) {
        if( errno != ERANGE ) {
            throw std::runtime_error(strerror(errno));
        }
        path.resize(path.size()*2);
    }
    path.resize(path.find('\0'));
    return path;
}

inline bool exists(const std::string & path) {
  struct COLUGO_FILESYS_STAT buffer;
  return (COLUGO_FILESYS_STAT(path.c_str(), &buffer) == 0);
}

std::string absolute_path(const std::string & path) {
#ifdef PATH_MAX
    char buffer[PATH_MAX];
    realpath(path.c_str(), buffer);
    return std::string(buffer);
#else
    auto actualpath = realpath(symlinkpath, NULL);
    if (actualpath != NULL) {
        std::string resolved_path = actual_path;
        free(actualpath);
        return resolved_path;
    } else {
        throw std::runtime_error("Failed to get absolute path");
    }
#endif
}

}} // namespace colugo::filesys

#endif
