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

#include <cassert>
#include <string>
#include <vector>
#include <iostream>
#include <sstream>
#include <cstdlib>
#include <map>
#include <utility>
#include <stdexcept>
#include <iomanip>
#include <string.h>
#include "textutil.hpp"
#include "filesys.hpp"

#if !defined(COLUGO_CMDOPTS_HPP)
#define COLUGO_CMDOPTS_HPP

namespace colugo {

const unsigned CMDOPTS_LINE_WIDTH = 78;
const unsigned CMDOPTS_OPTION_COL_WIDTH = 24;

///////////////////////////////////////////////////////////////////////////////
// OptionValueTypeError

/**
 * Thrown when types cannot be converted.
 */
class OptionValueTypeError : public std::runtime_error {
    public:
        OptionValueTypeError(const char * msg) : std::runtime_error(msg) {}
};

/**
 * Base class for an optional argument on the command line.
 */
class OptionArg {

    public:

        /**
         * Constructs argument handler with basic help info.
         *
         * @param help          help message
         * @param meta_var      string to display as value in help
         */
        OptionArg(const char * help=NULL, const char * meta_var=NULL)
            : is_switch_(false),
            is_set_(false) {
            if (help != NULL) {
                this->help_ = help;
            }
            if (meta_var != NULL) {
                this->set_meta_var(meta_var);
            }
        }

        /** Destructor. */
        virtual ~OptionArg() {}

        /**
         * Writes help message for this option out to given stream.
         * @param   out     stream to write message to
         * @return          output stream
         */
        std::ostream& write_help(std::ostream& out) const {
            std::string help_str;
            help_str += "  ";
            if (this->short_flag_.size() > 0) {
                help_str += this->short_flag_;
                if (!this->is_switch_) {
                    help_str += " ";
                    if (this->meta_var_.size() == 0) {
                        help_str += "VALUE";
                    } else {
                        help_str += this->meta_var_;
                    }
                }
                if (this->long_flag_.size() > 0) {
                    help_str += ", ";
                }
            }
            if (this->long_flag_.size() > 0) {
                help_str += this->long_flag_;
                if (!this->is_switch_) {
                    help_str += "=";
                    if (this->meta_var_.size() == 0) {
                        help_str += "VALUE";
                    } else {
                        help_str += this->meta_var_;
                    }
                }
            }
            if (this->help_.size() > 0) {
                if (help_str.size() > CMDOPTS_OPTION_COL_WIDTH-2) {
                    help_str += "\n";
                } else {
                    while (help_str.size() < CMDOPTS_OPTION_COL_WIDTH) {
                        help_str += " ";
                    }
                }
                std::string help_msg = this->help_;
                std::string::size_type defval = help_msg.find("%default");
                std::string replace_val = this->current_value_as_string();
                while (defval != std::string::npos) {
                    help_msg.replace(defval, 8, replace_val.c_str());
                    defval = help_msg.find("%default");
                }
                help_str += help_msg;
                std::string help_desc = textutil::textwrap(help_str, CMDOPTS_LINE_WIDTH, 0, CMDOPTS_OPTION_COL_WIDTH);
                help_str = help_desc;
            }
            out << help_str;
            return out;
        }

        /**
         * Sets the short flag bound to this argument.
         * @param   flag     short flag, including leading dash
         */
        void set_short_flag(const std::string flag) {
            this->short_flag_ = flag;
        }

        /**
         * Sets the long flag bound to this argument.
         * @param   flag     long flag, including leading dashes
         */
        void set_long_flag(const std::string flag) {
            this->long_flag_ = flag;
        }

        void set_meta_var(const char*  s) {
            this->meta_var_ = s;
        }

        /**
         * Returns <code>true</code> if this option is a switch (controls
         * a true/false option as indicated by its presence, and hence takes
         * no arguments).
         * @return      <code>true</code> if no arguments needed
         */
        bool is_switch() const {
            return this->is_switch_;
        }

        /**
         * Set to <code>true</code> if this option is a switch (controls
         * a true/false option as indicated by its presence, and hence takes
         * no arguments).
         * @param val      <code>true</code> if no arguments needed
         */
        void set_is_switch(bool val) {
            this->is_switch_ = val;
        }

        /**
         * Returns <code>true</code> if this option was set by user.
         * @return <code>true</code> if this option was set by user=
         */
        bool is_set() {
            return this->is_set_;
        }

        /**
         * Set to <code>true</code> if this option was set by user.
         * @param set <code>true</code> if this option was set by user
         */
        void set_is_set(bool set) {
            this->is_set_ = set;
        }

        /**
         * Processes string given as argument to this option.
         *
         * @param val_str       value passed to this option by user (as string)
         */
        virtual void process_value_string(const std::string& val_str) = 0;

        /**
         * Returns current value of argument as string.
         * @return current value of argument as string
         */
        virtual std::string current_value_as_string() const = 0;


    private:

        /** short flag string (single dash followed by single character) */
        std::string     short_flag_;
        /** long flag string (two dashes followed by one or more characters) */
        std::string     long_flag_;
        /** help message */
        std::string     help_;
        /** string to display as placeholder for value */
        std::string     meta_var_;
        /** <code>true</code> if this option takes no arguments */
        bool            is_switch_;
        /** <code>true</code> if value for this option was set by user */
        bool            is_set_;

};

/**
 * Templated derivation of OptionArg to handle various types of arguments.
 */
template <typename T>
class TypedOptionArg : public OptionArg {

    public:

        /**
         * Constructs argument handler with pointer to data store, flag and
         * help info.
         *
         * @param  store        pointer to data store that will hold argument
         * @param  short_flag   short flag, including leading dash
         * @param  long_flag    long flag, including leading dashes
         * @param  help         help message
         * @param  meta_var     string to display as value in help
         */
        TypedOptionArg(void * store,
                       const char * short_flag,
                       const char * long_flag,
                       const char * help=NULL,
                       const char * meta_var=NULL)
                : OptionArg(help, meta_var) {
            if (store != NULL) {
                this->set_store(store);
            }
            assert( short_flag != NULL || long_flag != NULL);
            if (short_flag != NULL) {
                this->set_short_flag(short_flag);
            }
            if (long_flag != NULL) {
                this->set_long_flag(long_flag);
            }
        }

        /** Destructor. */
        virtual ~TypedOptionArg() {}

        /**
         * Returns pointer to data store.
         * @return  pointer to data store
         */
        T * get_store() {
            return this->store_;
        }

        /**
         * Sets pointer to data store (explicit typing).
         * @param store  pointer to data store
         */
        void set_store(T * store) {
            this->store_ = store;
        }

        /**
         * Sets pointer to data store (void typing).
         * @param store  pointer to data store
         */
        void set_store(void * store) {
            this->store_ = static_cast<T *>(store);
        }

        /**
         * Handles typed value passed to this option.
         * @param val   value for this option
         */
        void process_value(const T& val) {
            *this->store_ = val;
        }

        /**
         * Handles value passed to this argument as tring
         *
         * @param val_str   string representation of the value for this option
         */
        virtual void process_value_string(const std::string& val_str) {
            std::istringstream istr(val_str);
            T temp;
            istr >> std::noskipws;
            istr >> temp;
            if (!istr.fail() && istr.eof()) {
                *this->store_ = temp;
                this->set_is_set(true);
            } else {
                std::string msg;
                msg = "failed to convert \"" + val_str + "\"";
                throw OptionValueTypeError(msg.c_str());
            }
        }

        /**
         * Returns current value of argument as string.
         * @return current value of argument as string
         */
        virtual std::string current_value_as_string() const {
            assert(this->store_ != NULL);
            std::ostringstream ostr;
            ostr << *this->store_;
            return ostr.str();
        }


    private:

        /** pointer to storage for value for this option */
        T *     store_;
};


///////////////////////////////////////////////////////////////////////////////
//! General option parser.
class OptionParser {

    public:

        typedef std::vector< std::string >  PosArgs;

        /** Constructor. */
        OptionParser(const char * version=NULL,
                const char * description=NULL,
                const char * usage=NULL)
                : show_help_(false)
                , show_version_(false) {
            if (usage != NULL) {
                this->usage_.assign(usage);
            } else {
                this->usage_ = "%prog [options] [args]";
            }
            if (description != NULL) {
                this->description_.assign(description);
            }
            if (version != NULL) {
                this->version_.assign(version);
            }
            this->version_option_ = this->add_switch(&this->show_version_, NULL, "--version", "show program's version number and exit");
            this->help_option_ = this->add_switch(&this->show_help_, "-h", "--help",  "show this help message and exit");
        }

        /** Destructor: frees memory associated with OptionArg objects. */
        virtual ~OptionParser() {
            for (std::vector<OptionArg *>::iterator oap = this->option_args_.begin();
                    oap != this->option_args_.end();
                    ++oap) {
                delete *oap;
            }
        }

        /**
         * Add an option to those supported by the program.
         * Must supply at least one of the following: short_flag or long_flag.
         * Short flags start with a dash and are followed by one character
         * (e.g., "-f").
         * Long flags start with two dashes and are followed by one or more
         * characters (e.g., "--filename").
         *
         * @param  store        pointer to data store that will hold argument
         * @param  short_flag   short flag, including leading dash
         * @param  long_flag    long flag, including leading dashes
         * @param  help         help message
         * @param  meta_var     string to display as value in help
         */
        template <typename T>
        OptionArg * add_option(void * store,
                               const char * short_flag=NULL,
                               const char * long_flag=NULL,
                               const char * help=NULL,
                               const char * meta_var=NULL) {
            OptionArg * oa;
            oa = new TypedOptionArg<T>(store, short_flag, long_flag, help, meta_var);
            assert ( oa );

            this->option_args_.push_back(oa);
            if (short_flag) {
                assert(short_flag[0] == '-' && short_flag[1] != 0 && short_flag[1] != '-');
                assert(this->key_opt_map_.find(short_flag) == this->key_opt_map_.end());
                this->key_opt_map_.insert(std::make_pair(short_flag, oa));
            }
            if (long_flag) {
                assert(long_flag[0] == '-' && long_flag[1] =='-' && long_flag[2] != '-');
                assert(this->key_opt_map_.find(long_flag) == this->key_opt_map_.end());
                this->key_opt_map_.insert(std::make_pair(long_flag, oa));
            }

            if (meta_var != NULL) {
                oa->set_meta_var(meta_var);
            } else if (long_flag != NULL) {
                oa->set_meta_var(long_flag + 2);
            } else if (short_flag != NULL) {
                oa->set_meta_var(short_flag + 1);
            }

            return oa;
        }

        /**
         * Add boolean (switch) option to those supported by the program.
         * Must supply least one of the following: short_flag or long_flag.
         * Short flags start with a dash and are followed by one character
         * (e.g., "-f").
         * Long flags start with two dashes and are followed by one or more
         * characters (e.g., "--filename").
         *
         * @param  store        pointer to data store that will hold argument
         * @param  short_flag   short flag, including leading dash
         * @param  long_flag    long flag, including leading dashes
         * @param  help         help message
         * @param  meta_var     string to display as value in help
         */
        OptionArg * add_switch(void * store,
                               const char * short_flag=NULL,
                               const char * long_flag=NULL,
                               const char * help=NULL,
                               const char * meta_var=NULL) {
            OptionArg * switch_arg = this->add_option<bool>(store, short_flag, long_flag, help, meta_var);
            switch_arg->set_is_switch(true);
            return switch_arg;
        }

        /**
         * Returns copy of the current program usage string.
         * @return  copy of the current program usage string
         */
         std::string get_usage() const {
            return this->usage_;
         }

        /**
         * Sets the current program usage string.
         * @param usage  program usage string
         */
         void set_usage(const char * usage) {
            this->usage_ = usage;
         }

        /**
         * Returns copy of the current program description string.
         * @return  copy of the current program description string
         */
         std::string get_description() const {
            return this->description_;
         }

        /**
         * Sets the current program description string.
         * @param description  program description string
         */
         void set_description(const char * description) {
            this->description_ = description;
         }

        /**
         * Returns copy of the current program version string.
         * @return  copy of the current program version string
         */
         std::string get_version() const {
            return this->version_;
         }

        /**
         * Sets the current program version string.
         * @param version  program version string
         */
         void set_version(const char * version) {
            this->version_ = version;
         }

        /**
         * Client must call this, passing in arguments from main().
         *
         * @param argc      number of argument strings in the command line
         * @param argv      array of argument strings
         */
        void parse(int argc, const char * argv[]) {
            this->prog_filename_ = filesys::get_path_leaf(argv[0]).c_str();
            for (int i = 1; i < argc; ++i) {
                if (argv[i][0] == '-') {
                    std::string arg_name;
                    std::string arg_value;

                    if (strncmp(argv[i], "--", 2) == 0) {
                        bool parsing_name = true;
                        for (const char *a = argv[i]; *a; ++a) {
                            if (parsing_name) {
                                if (*a == '=') {
                                    parsing_name = false;
                                } else {
                                    arg_name += *a;
                                }
                            } else {
                                arg_value += *a;
                            }
                        }
                    } else if (argv[i][0] == '-') {
                        std::string arg(argv[i]);
                        if (arg.size() < 2) {
                            std::cerr << "unrecognized or incomplete option \"" << arg << "\"" << std::endl;
                            exit(1);
                        }
                        if (arg.size() == 2) {
                            arg_name = arg;
                        } else {
                            arg_name = arg.substr(0, 2);
                            arg_value = arg.substr(2, arg.size());
                        }
                    }

        //             std::map< std::string, OptionArg * >::iterator oai = this->key_opt_map_.find(arg_name);
        //             if ( oai == this->key_opt_map_.end() ) {
        //                 std::cerr << "unrecognized option \"" << arg_name << "\"" << std::endl;
        //                 exit(1);
        //             }

                    std::vector< std::string > matches;
                    for (std::map< std::string, OptionArg * >::iterator oai = this->key_opt_map_.begin();
                        oai != this->key_opt_map_.end();
                        ++oai) {
                        const std::string& a = oai->first;
                        if (a == arg_name) {
                            matches.clear();
                            matches.push_back(a);
                            break;
                        }
                        if (a.compare(0, arg_name.size(), arg_name) == 0 ) {
                            matches.push_back(a);
                        }
                    }

                    if (matches.size() == 0) {
                        std::cerr << "unrecognized option \"" << arg_name << "\"" << std::endl;
                        exit(1);
                    } else if (matches.size() > 1) {
                        std::cerr << "multiple matches found for option beginning with \"" << arg_name << "\":" << std::endl;
                        for (std::vector<std::string>::iterator mi = matches.begin(); mi != matches.end(); ++mi) {
                            std::cerr << *mi << std::endl;
                        }
                        exit(1);
                    }

                    OptionArg& oa = *(this->key_opt_map_[matches[0]]);

                    if (!oa.is_switch()) {
                        if (arg_value.size() == 0) {
                            if (i == argc-1) {
                                std::cerr << "expecting value for option \"" << arg_name << "\"" << std::endl;
                                exit(1);
                            } else {
                                arg_value = argv[i+1];
                                i += 1;
                            }
                        }
                        try {
                            oa.process_value_string(arg_value);
                        } catch(OptionValueTypeError& e) {
                            std::cerr << "Invalid value passed to option " << arg_name << ": ";
                            std::cerr << "\"" << arg_value << "\"" << std::endl;
                            exit(1);
                        }
                    } else {
                        TypedOptionArg<bool>* bool_opt = static_cast< TypedOptionArg<bool> *>(&oa);
                        bool_opt->process_value(true);
                    }

                } else {
                    this->pos_args_.push_back(argv[i]);
                }


                // help option specified
                if (this->show_help_) {
                    this->write_help(std::cout);
                    exit(0);
                }

                // show version
                if (this->show_version_) {
                    this->write_version(std::cout);
                    exit(0);
                }
            }
        }

        /**
         * Checks to see if a particular OptionArg is set.
         * @param   flag    flag string for option (including leading dashes)
         * @return          <code>true</code> if option is set
         */
        bool is_set(const char * flag) {
            OptionArg& oa = this->get_option(flag);
            return oa.is_set();
        }

        /**
         * Copies value reference by pointer to data to OptionArg mapped to
         * given flag.
         *
         * @param flag      flag for option to be set
         * @param data      pointer to data to set option
         */
        void store_value(const char * flag, void * data);

        /**
         * Returns copy of vector of positional (non-flagged) arguments.
         * @return  vector of positional (non-flagged) arguments
         */
        std::vector< std::string > get_args() {
            return this->pos_args_;
        }

        /**
         * Writes out help for all options to given output stream.
         * @param out   output stream to which to write help message
         * @return      same output stream
         */
        std::ostream& write_help(std::ostream& out) const {
            this->write_usage(out);
            out << std::endl;
            this->write_description(out);
            out << "Options:" << std::endl;
            for (std::vector<OptionArg *>::const_iterator oa = this->option_args_.begin();
                    oa != this->option_args_.end();
                    ++oa) {
                (*oa)->write_help(out);
                out << std::endl;
            }
            return out;
        }

        /**
         * Writes usage info to the given output stream.
         * @param out   output stream to which to write usage info
         * @return      same output stream
         */
        std::ostream& write_usage(std::ostream& out) const {
            if (this->usage_.size() != 0) {
                std::string usage = "Usage: " + this->usage_;
                std::string::size_type pos = usage.find("%prog");
                while (pos != std::string::npos) {
                    usage.replace(pos, 5, this->prog_filename_);
                    pos = usage.find("%prog");
                }
                out << usage << std::endl;
            }
            return out;
        }

        /**
         * Writes description to the given output stream.
         * @param out   output stream to which to write description
         * @return      same output stream
         */
        std::ostream& write_description(std::ostream& out) const {
            if (this->description_.size() != 0) {
                out << textutil::textwrap(this->description_, CMDOPTS_LINE_WIDTH, 0, CMDOPTS_OPTION_COL_WIDTH) << "\n" << std::endl;
            }
            return out;
        }

        /**
         * Writes version to the given output stream.
         * @param out   output stream to which to write version
         * @return      same output stream
         */
        std::ostream& write_version(std::ostream& out) const {
            out << this->version_ << std::endl;
            return out;
        }

    private:
        /**
         * Searches for and returns pointer to OptionArg object correspondng to
         * given flag.
         * @param flag  flag for OptionArg to return
         */
        OptionArg* get_option_ptr(const char * flag) {
            std::map< std::string, OptionArg * >::iterator oai = this->key_opt_map_.find(flag);
            assert (oai != this->key_opt_map_.end() );
            return oai->second;
        }

        /**
         * Searches for and returns reference OptionArg object correspondng to
         * given flag.
         * @param flag  flag for OptionArg to return
         */
        OptionArg& get_option(const char * flag) {
            return *(get_option_ptr(flag));
        }

    private:
        /** stores value of help option arg switch */
        bool                                        show_help_;
        /** stores pointer to help option arg */
        OptionArg *                                 help_option_;
        /** stores value of help option arg switch */
        bool                                        show_version_;
        /** stores pointer to help option arg */
        OptionArg *                                 version_option_;
        /** usage string */
        std::string                                 usage_;
        /** program description */
        std::string                                 description_;
        /** program version */
        std::string                                 version_;
        /** collection of OptionArg objects */
        std::vector<OptionArg *>                    option_args_;
        /** positional arguments on the command line */
        PosArgs                                     pos_args_;
        /** map of flag strings to option arguments */
        std::map< std::string, OptionArg * >        key_opt_map_;
        /** the program file name */
        std::string                                 prog_filename_;
};


///////////////////////////////////////////////////////////////////////////////
// Specializations of TypedOptionArg

template <>
inline void TypedOptionArg<std::string>::process_value_string(const std::string& val_str) {
    *this->store_ = val_str;
    this->set_is_set(true);
}

} // namespace colugo

#endif
