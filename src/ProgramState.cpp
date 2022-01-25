#include "ProgramState.hpp"

namespace mipa{
    namespace ProgramState{
        std::map<std::string, std::stack<Value*>> symbolStack;
        std::map<std::string, Value*> symbolTable;
        std::stack<Value*> constants;
        std::unordered_map<Value*, int> pointerCounter;

        bool finished = false;
        bool should_refresh = false;
        sf::Image* for_display = nullptr;   

        void countPointer(Value* val){
            auto it = pointerCounter.find(val);
            if(it != pointerCounter.end()){
                it->second++;
            }else{
                pointerCounter[val] = 1;
            }
        }

        void uncountPointer(Value* val){
            pointerCounter[val]--;
            if(pointerCounter[val] == 0){
                pointerCounter.erase(val);
                delete val;
            }
        }
        void setConstant(Value* val){
            countPointer(val);
            constants.push(val);
        }
        void set(const std::string var, Value* val){
            auto it = symbolTable.find(var);
            if(it != symbolTable.end() && it->second->type == IMAGE){
                auto imgVal = (ImageValue*)it->second;
                if(&imgVal->image == for_display){
                    if(val->type != IMAGE){
                        throw std::runtime_error(var+" is currently displayed and must store an image");
                    }
                    for_display = &((ImageValue*)val)->image;
                    should_refresh = true;
                }
            }
            countPointer(val);
            unset(var);
            symbolTable[var]=val;
        }
        Value* get(const std::string var){
            auto it = symbolTable.find(var);
            if(it == symbolTable.end()) throw std::runtime_error("Undefined variable "+var);
            return it->second;
        }
        void unset(const std::string var){
            auto it = symbolTable.find(var);
            if(it != symbolTable.end()){
                uncountPointer(it->second);
                symbolTable.erase(var);
            }
        }
        void gb_constants(){
            while(!constants.empty()){ 
                uncountPointer(constants.top());
                constants.pop();
            }
        }
    }
}