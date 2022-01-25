#include "BuiltInFunctions.hpp"

#include <iostream>

#include "ProgramState.hpp"

namespace mipa{
    typedef std::stack<Value*> argstack;
/* 
        COLOR = 1, 
        PALETTE = 2,
        IMAGE = 4, 
        QUANTIZER = 8, 
        COLORER = 16, 
        NUMBER = 32, 
        STRING = 64 */
    
    std::map<ValueType, std::string> typenames{
        {COLOR, "Color"},
        {PALETTE, "Palette"},
        {IMAGE, "Image"},
        {QUANTIZER, "Color Quantizer"},
        {COLORER, "Color Strategy"},
        {NUMBER, "Number"},
        {STRING, "String"},
    };

    void assert_arity(argstack& args, uint n){
        if(args.size() != n){
            throw std::runtime_error("Incorrect number of arguments: expected "
                +std::to_string(n)+", got: "+std::to_string(args.size()));
        }
    }
    
    void assert_type(const Value& val, ValueType type){
        if(val.type != type){
            throw std::runtime_error("Incorrect type: expected "+typenames[type]+", got: "+typenames[type]);
        }
    }
    

    Value* env(argstack& args){
        assert_arity(args, 0);
        for(auto& pair: ProgramState::symbolTable){
            std::cout << pair.first << '=' 
                << (pair.second == nullptr ? "null" : pair.second->toString()) 
                << std::endl;
        }
        return nullptr;
    }

    Value* print(argstack& args){
        argstack aux;
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
    
    Value* exit(argstack& args){
        assert_arity(args, 0);
        ProgramState::finished = true;
        throw 0;
        return nullptr;
    }

    Value* load(argstack& args){
        assert_arity(args, 1);
        assert_type(*args.top(), STRING);
        std::string filename = ((StringValue*)args.top())->string;
        auto imgVal = new ImageValue();
        if(!imgVal->image.loadFromFile(filename)){
            throw std::runtime_error("Not able to load image: "+filename);
        }
        return imgVal;
    }

    Value* save(argstack& args){
        assert_arity(args, 2);
        assert_type(*args.top(), IMAGE);
        ImageValue* val = ((ImageValue*)args.top());
        args.pop();
        assert_type(*args.top(), STRING);
        std::string filename = ((StringValue*)args.top())->string;
        val->image.saveToFile(filename);
        return val;
    }

    const std::map<std::string, BuiltInFunction> BuiltInFunctions = {
        {"save", save},
        {"load", load},
        {"print", print},
        {"env", env},
        {"quit", exit},
        {"exit", exit}
    };
}