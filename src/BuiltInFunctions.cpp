#include "BuiltInFunctions.hpp"

#include <iostream>

#include "ProgramState.hpp"

namespace mipa{
    Value* env(std::stack<Value*>){
        for(auto& pair: ProgramState::symbolTable){
            std::cout << pair.first << "?" << std::endl;
            std::cout << pair.first << '=' 
                << (pair.second == nullptr ? "null" : pair.second->toString()) 
                << std::endl;
        }
        return nullptr;
    }

    Value* print(std::stack<Value*> args){
        std::stack<Value*> aux;
        while(! args.empty()){
            aux.push(args.top());
            args.pop();
        }
        while(!aux.empty()){
            Value* v = aux.top();
            std::cout << (v == nullptr ? "null" : v->toString());
            aux.pop();
            if(!aux.empty()) std::cout << ' ';
        }
        std::cout << std::endl;
        return nullptr;
    }
    
    Value* exit(std::stack<Value*>){
        ProgramState::finished = true;
        throw 0;
        return nullptr;
    }

    const std::map<std::string, BuiltInFunction> BuiltInFunctions = {
        {"print", print},
        {"env", env},
        {"quit", exit},
        {"exit", exit}
    };
}