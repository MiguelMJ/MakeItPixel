#include "ProgramState.hpp"

namespace mipa{
    namespace ProgramState{
        std::map<std::string, std::stack<Value*>> symbolStack;
        std::map<std::string, Value*> symbolTable;
        std::stack<Value*> constants;
        std::unordered_map<Value*, int> pointerCounter;

        std::vector<Value*> garbage;

        bool finished = false;
        bool should_refresh = false;
        sf::Image* for_display = nullptr;   

        void countPointer(Value* val){
            std::cout << "COUNT " << val << std::endl;
            auto it = pointerCounter.find(val);
            if(it != pointerCounter.end()){
                it->second++;
            }else{
                pointerCounter[val] = 1;
            }
        }

        void uncountPointer(Value* val){
            std::cout << "UNCOUNT " << val << std::endl;
            pointerCounter[val]--;
            if(pointerCounter[val] == 0){
                pointerCounter.erase(val);
                garbage.push_back(val);
            }
        }
        void setConstant(Value* val){
            countPointer(val);
            constants.push(val);
        }
        void set(const std::string var, Value* val, bool inner){
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
            }else if(!inner && var[0] == '_'){
                if(it == symbolTable.end()){
                    throw std::runtime_error(var+" is not an existent configuration variable");
                }else if(it->second->type != val->type){
                    throw std::runtime_error("Configuration variables must keep their type: "+var);
                }
            }
            countPointer(val);
            unset(var);
            symbolTable[var]=val;
            if(var == "_scale" && for_display != nullptr){
                should_refresh = true;
            }
        }
        Value* get(const std::string var){
            auto it = symbolTable.find(var);
            if(it == symbolTable.end()) throw std::runtime_error("Undefined variable "+var);
            return it->second;
        }
        void maybeRefresh(ImageValue* val){
            if(&val->image == for_display){
                should_refresh = true;
            }
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
        void gb(){
            for(auto& v: garbage){
                delete v; 
            }
            garbage.clear();
        }
    }
}