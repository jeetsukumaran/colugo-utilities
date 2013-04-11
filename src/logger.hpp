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

#include <map>
#include <iostream>
#include <string>

#if !defined(COLUGO_LOGGER_HPP)
#define COLUGO_LOGGER_HPP

namespace colugo {

class Logger {

    public:
        enum class LoggingLevel {
            CRITICAL=1,
            ERROR,
            WARNING,
            INFO,
            DEBUG,
            VERBOSE,
            VVERBOSE,
            NOT_SET=999,
        };

    public:
        Logger(const std::string& name);
        void add_channel(std::ostream& dest, Logger::LoggingLevel logging_level);

        template <typename... Types>
        void critical(const Types&... args) {
            this->log(Logger::LoggingLevel::CRITICAL, args...);
        }

        template <typename... Types>
        void error(const Types&... args) {
            this->log(Logger::LoggingLevel::ERROR, args...);
        }

        template <typename... Types>
        void warning(const Types&... args) {
            this->log(Logger::LoggingLevel::WARNING, args...);
        }

        template <typename... Types>
        void info(const Types&... args) {
            this->log(Logger::LoggingLevel::INFO, args...);
        }

        template <typename... Types>
        void debug(const Types&... args) {
            this->log(Logger::LoggingLevel::DEBUG, args...);
        }

        template <typename... Types>
        void verbose(const Types&... args) {
            this->log(Logger::LoggingLevel::VERBOSE, args...);
        }

        template <typename... Types>
        void vverbose(const Types&... args) {
            this->log(Logger::LoggingLevel::VVERBOSE, args...);
        }

        template <typename... Types>
        void log(const Logger::LoggingLevel& message_level, const Types&... args) {
            for (auto & ch_iter : this->channels_) {
                if (ch_iter.second >= message_level) {
                    *(ch_iter.first) << this->name_ << ": [" << this->level_descs_[message_level] << "] ";
                    this->emit_(*(ch_iter.first), args...);
                    *(ch_iter.first) << std::endl;
                }
            }
        }

    private:

        template <typename S>
        void emit_(S&) {}

        template <typename S, typename T, typename... Types>
        void emit_(S& stream, const T& arg1, const Types&... args) {
            stream << arg1;
            this->emit_(stream, args...);
        }

    private:
        std::string                                         name_;
        std::map<std::ostream *, Logger::LoggingLevel>      channels_;
        std::map<Logger::LoggingLevel, const char *>        level_descs_;

}; // Logger


} // namespace colugo

#endif
