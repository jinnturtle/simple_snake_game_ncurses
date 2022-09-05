#ifndef SRC_LOGS_HPP_
#define SRC_LOGS_HPP_

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>

extern "C" {
#include "timestamp.h"
}

namespace logs {
    static std::ostream* std_out = &std::cout;
    static std::ostream* err_out = &std::cerr;

    template<typename... Ts>
    auto print(std::ostream* out, Ts... args) -> void
    {
        std::stringstream buf;
        (buf << ... << args);
        (*out) << buf.str() << std::endl;
    }

    template<typename... Ts>
    auto dbg(Ts... args) -> void
    {
        print(std_out, "DEBUG ", timestamp_nano(), " ", args...);
    }

    // log into error output (stderr, should be configurable in the future)
    template<typename... Ts>
    auto err(Ts... args) -> void
    {
        print(err_out, "ERROR ", timestamp_nano(), " ", args...);
    }

} // namespace logs

#ifndef SRC_POS
    #define SRC_POS "[", __FILE__, ":", __LINE__, "]"
#else
    #error SRC_POS already defined
#endif

#ifdef DEBUG
    #define DBG(verbocity, ...) if ((DEBUG) >= (verbocity) || (DEBUG) == -1) {\
    logs::dbg(SRC_POS, " ", __VA_ARGS__);\
}

#else
#   define DBG(verbocity, ...) // nothing to do if not a debug build
#endif // DEBUG

#endif // SRC_LOGS_HPP_
