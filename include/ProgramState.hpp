#ifndef __MIPA_PROGRAMSTATE_HPP__
#define __MIPA_PROGRAMSTATE_HPP__

#include <map>
#include <stack>
#include <string>

#include "Palette.hpp"
#include "Color.hpp"

namespace mipa{
    namespace ProgramState{
        extern std::map<std::string, std::stack> symbolTable;
        
    }
}

#endif