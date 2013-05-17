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
#include <vector>
#include <ctime>
#include "stream.hpp"
#include "textutil.hpp"

#if !defined(COLUGO_LOGGER_HPP)
#define COLUGO_LOGGER_HPP

namespace colugo {

class Logger {

    public:
        enum class LoggingLevel {
            NOTSET=0,
            VVERBOSE=3,
            VERBOSE=6,
            DEBUG=10,
            INFO=20,
            WARNING=30,
            ERROR=40,
            CRITICAL=50,
            ABORTING=60,
        };

    public:
        Logger(const std::string& name)
                : name_(name) {
            this->level_descs_[Logger::LoggingLevel::VVERBOSE] = "VVERBOSE";
            this->level_descs_[Logger::LoggingLevel::VERBOSE]  = "VERBOSE";
            this->level_descs_[Logger::LoggingLevel::DEBUG]    = "DEBUG";
            this->level_descs_[Logger::LoggingLevel::INFO]     = "INFO";
            this->level_descs_[Logger::LoggingLevel::WARNING]  = "WARNING";
            this->level_descs_[Logger::LoggingLevel::ERROR]    = "ERROR";
            this->level_descs_[Logger::LoggingLevel::CRITICAL] = "CRITICAL";
            this->level_descs_[Logger::LoggingLevel::ABORTING] = "ABORTING";
        }

        void add_channel(std::ostream& dest,
                Logger::LoggingLevel logging_level,
                int timestamp=0,
                Logger::LoggingLevel decoration_level=Logger::LoggingLevel::NOTSET) {
            this->channels_[&dest] = logging_level;
            this->channel_time_decoration_[&dest] = timestamp;
            this->channel_decoration_level_[&dest] = decoration_level;
        }

        template <typename... Types>
        void abort(const Types&... args) {
            this->log(Logger::LoggingLevel::ABORTING, args...);
            exit(EXIT_FAILURE);
        }

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
                if (message_level >= ch_iter.second) {
                    auto & chout = *(ch_iter.first);
                    chout << "[" << this->name_ << "]";
                    if (this->channel_time_decoration_[ch_iter.first] > 0) {
                        std::time(&this->time_struct_);
                        std::strftime(this->time_str_buffer_, 20,
                                "%Y-%m-%d %H:%M:%S", std::localtime(&this->time_struct_));
                        chout << " - " << this->time_str_buffer_;
                    }
                    if (message_level == Logger::LoggingLevel::NOTSET || message_level >= this->channel_decoration_level_[ch_iter.first]) {
                        chout << " - " << this->level_descs_[message_level];
                    }
                    chout << " - ";
                    this->emit_(chout, args...);
                    chout << std::endl;
                }
            }
        }

    private:

        template <typename... Types>
        void emit_(std::ostream & out, const Types&... args) {
            colugo::stream::write(out, args...);
        }

    private:
        std::string                                      name_;
        std::map<std::ostream *, Logger::LoggingLevel>   channels_;
        std::map<std::ostream *, int>                    channel_time_decoration_;
        std::map<std::ostream *, Logger::LoggingLevel>   channel_decoration_level_;
        std::map<Logger::LoggingLevel, const char *>     level_descs_;
        std::time_t                                      time_struct_;
        char                                             time_str_buffer_[20];

}; // Logger

} // namespace colugo

#endif
