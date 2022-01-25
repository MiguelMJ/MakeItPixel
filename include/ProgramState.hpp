#ifndef __MIPA_PROGRAMSTATE_HPP__
#define __MIPA_PROGRAMSTATE_HPP__

#include <unordered_map>
#include <map>
#include <stack>
#include <string>

#include "Value.hpp"
#include "Palette.hpp"
#include "Color.hpp"

namespace mipa{
    namespace ProgramState{
        extern std::map<std::string, std::stack<Value*>> symbolStack;
        extern std::map<std::string, Value*> symbolTable;
        extern std::stack<Value*> constants;
        extern std::unordered_map<Value*, int> pointerCounter;
        extern sf::Image* for_display;

        extern bool finished;
        void setConstant(Value* val);
        Value* get(const std::string var);
        void set(const std::string var, Value* val);
        void unset(const std::string var);
        void gb_constants();
    }
}

#endif