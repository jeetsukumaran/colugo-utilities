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

#include "logger.hpp"

namespace colugo {

Logger::Logger(const std::string& name)
        : name_(name) {
    this->level_descs_[Logger::LoggingLevel::CRITICAL] = "CRITICAL";
    this->level_descs_[Logger::LoggingLevel::ERROR]    = "ERROR";
    this->level_descs_[Logger::LoggingLevel::WARNING]  = "WARNING";
    this->level_descs_[Logger::LoggingLevel::INFO]     = "INFO";
    this->level_descs_[Logger::LoggingLevel::DEBUG]    = "DEBUG";
    this->level_descs_[Logger::LoggingLevel::VERBOSE]  = "VERBOSE";
    this->level_descs_[Logger::LoggingLevel::VVERBOSE] = "VVERBOSE";
}

void Logger::add_channel(std::ostream& dest, Logger::LoggingLevel logging_level) {
    this->channels_[&dest] = logging_level;
}

} // colugo
