#ifndef __MIPA_LOG_HPP__
#define __MIPA_LOG_HPP__

#include <string>

namespace mipa{
    typedef enum {PLAIN, INFO, WARNING, ERROR, IMPORTANT, SUCCESS}  LogType;
    void log(LogType level, std::string msg_pre, std::string end="\n");
}
#endif