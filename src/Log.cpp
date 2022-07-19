#include "Log.hpp"

#include <iostream>

/*
 * LOG FUNCTIONS
 */
namespace mipa{
    void log(LogType level, std::string msg_pre, std::string end){
        std::cerr << "\r\x1b[2K";
        switch(level){
            case LogType::INFO:
            std::cerr << "- ";
            break;
            case LogType::WARNING:
            std::cerr << "\x1b[35mW ";
            break;
            case LogType::ERROR:
            std::cerr << "\x1b[1;31m\u2716 ";
            break;
            case LogType::IMPORTANT:
            std::cerr << "\x1b[1m> ";
            break;
            case LogType::SUCCESS:
            std::cerr << "\x1b[32m\u2714 ";
            break;
        }
        std::cerr << msg_pre << "\x1b[0m" << end;
    }
}