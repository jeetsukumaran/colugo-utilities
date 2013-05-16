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

#ifndef COLUGO_SUBPROCESS_HPP
#define COLUGO_SUBPROCESS_HPP

#include <iostream>
#include <string>
#include <sstream>
#include <stdexcept>
#include "pstream.h"

namespace colugo {

class SubprocessException : public std::exception {
    public:
        SubprocessException(
                const std::string & filename,
                unsigned long line_num,
                const std::string & message)
            : filename_(filename)
            , line_num_(line_num)
            , message_(message) {
        }
        virtual ~SubprocessException() throw() { }
        virtual const char * what() const throw() {
            std::ostringstream o;
            o << "File: " << this->filename_ << std::endl;
            o << "Line: " << this->line_num_ << std::endl;
            o << "Error: " << this->message_ << std::endl;
            return o.str().c_str();
        }

    private:
        std::string     filename_;
        unsigned long   line_num_;
        std::string     message_;
};

class SubprocessFailedToOpenChildProcessError : public SubprocessException {
    public:
        SubprocessFailedToOpenChildProcessError(
                const std::string & filename,
                unsigned long line_num,
                const std::string & message)
            : SubprocessException(filename, line_num, "Child process failed: " + message) {
        }
};

class SubprocessClosedChildProcessError : public SubprocessException {
    public:
        SubprocessClosedChildProcessError(
                const std::string & filename,
                unsigned long line_num,
                const std::string & message)
            : SubprocessException(filename, line_num, "Child process exited: " + message) {
        }
};

class Subprocess {

    public:
        typedef redi::pstream   pstream;

    public:

        /**
         * Opens a stream initialized to execute `cmd`:
         *
         *      Subprocess ps({"python", "script.py", "--src", ".", "-d"});
         *      stdout_str = ps.communicate("the quick brown fox");
         *
         * @param cmd
         *   A vector of strings representing the command to execute.
         * @param pipe_stdin
         *   Whether or not the standard input of the child process is
         *   connected.
         * @param pipe_stdout
         *   Whether or not the standard output of the child process is
         *   connected.
         * @param pipe_stderr
         *   Whether or not the standard error of the child process is
         *   connected.
         */
        Subprocess(const std::vector<std::string> & cmd,
                bool pipe_stdin=true,
                bool pipe_stdout=true,
                bool pipe_stderr=true)
                : command_(cmd)
                  , process_handle_(cmd, Subprocess::get_process_mode(pipe_stdin, pipe_stdout, pipe_stderr)) {
            if (!this->process_handle_.is_open()) {
                std::ostringstream o;
                for (auto & c : this->command_) {
                    o << " " << c;
                }
                throw SubprocessFailedToOpenChildProcessError(__FILE__, __LINE__, o.str());
            }
        }

        std::pair<const std::string, const std::string> communicate(const std::string & process_stdin="") {
            // this->verify_open_process();
            if (!process_stdin.empty()) {
                this->process_handle_ << process_stdin << redi::peof;
            }
            this->wait();
            return std::make_pair(this->process_stdout_, this->process_stderr_);
        }

        int wait() {
            while (!this->process_handle_.rdbuf()->exited()) {
                // wait for child to exit
            }
            std::ostringstream out_ss;
            std::ostringstream err_ss;
            Subprocess::read_pipes_non_blocking(this->process_handle_, out_ss, err_ss);
            this->process_stdout_ = out_ss.str();
            this->process_stderr_ = err_ss.str();
            this->process_returncode_ = Subprocess::get_process_returncode(this->process_handle_);
            return this->process_returncode_;
        }

        int returncode() const {
            return this->process_returncode_;
        }

        const std::string & get_stdout() const {
            return this->process_stdout_;
        }

        const std::string & get_stderr() const {
            return this->process_stderr_;
        }

        std::string get_command_string() const {
            std::ostringstream o;
            for (auto & arg : this->command_) {
                o << arg << " ";
            }
            return o.str();
        }

    private:

        // process mode
        static redi::pstreambuf::pmode get_process_mode(
                bool pipe_stdin=true,
                bool pipe_stdout=true,
                bool pipe_stderr=true) {
            redi::pstreambuf::pmode m;
            if (pipe_stdin) {
                m |= redi::pstreambuf::pstdin;
            }
            if (pipe_stdout) {
                m |= redi::pstreambuf::pstdout;
            }
            if (pipe_stderr) {
                m |= redi::pstreambuf::pstderr;
            }
            return m;
        }

        // non-blocking reading of process streams until they are cleared
        static void read_pipes_non_blocking(pstream & ps, std::ostream & stdout_stream, std::ostream & stderr_stream) {
            char buf[1024];
            std::streamsize n;
            bool finished[2] = { false, false };
            while (!finished[0] || !finished[1]) {
                if (!finished[0]) {
                    while ((n = ps.err().readsome(buf, sizeof(buf))) > 0) {
                        stderr_stream.write(buf, n).flush();
                    }
                    if (ps.eof()) {
                        finished[0] = true;
                        if (!finished[1]) {
                            ps.clear();
                        }
                    }
                }
                if (!finished[1]) {
                    while ((n = ps.out().readsome(buf, sizeof(buf))) > 0) {
                        stdout_stream.write(buf, n).flush();
                    }
                    if (ps.eof()) {
                        finished[1] = true;
                        if (!finished[0]) {
                            ps.clear();
                        }
                    }
                }
            }
        }

        // process exit code
        int get_process_returncode(pstream & ps) {
            return ps.rdbuf()->status();
        }

    private:
        std::vector<std::string>    command_;
        pstream                     process_handle_;
        std::string                 process_stdout_;
        std::string                 process_stderr_;
        int                         process_returncode_;

}; // Subprocess

} // namespace colugo

#endif
