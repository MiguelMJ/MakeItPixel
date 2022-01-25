#ifndef __MIPA_BUILTINFUNCTIONS_HPP__
#define __MIPA_BUILTINFUNCTIONS_HPP__

#include "Value.hpp"
#include <map>
#include <stack>

namespace mipa{
    typedef Value* (*BuiltInFunction)(std::stack<Value*>&);
    extern const std::map<std::string, BuiltInFunction> BuiltInFunctions;
}

#endif