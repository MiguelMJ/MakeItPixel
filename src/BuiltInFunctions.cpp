#include "BuiltInFunctions.hpp"

#include <iostream>

#include "Quantization.hpp"
#include "ProgramState.hpp"

namespace mipa{
    typedef std::stack<Value*> argstack;
    
    std::map<ValueType, std::string> typenames{
        {COLOR, "Color"},
        {PALETTE, "Palette"},
        {IMAGE, "Image"},
        {QUANTIZER, "Color Quantizer"},
        {COLORSTRATEGY, "Color Strategy"},
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
            throw std::runtime_error("Incorrect type: expected "+typenames[type]+", got: "+typenames[val.type]);
        }
    }
    

    Value* env(argstack& args){
        assert_arity(args, 0);
        for(auto& pair: ProgramState::symbolTable){
            std::cout << pair.first << " = "  
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

    Value* show(argstack& args){
        assert_arity(args, 1);
        assert_type(*args.top(), IMAGE);
        ProgramState::for_display = &((ImageValue*)args.top())->image;
        ProgramState::should_refresh = true;
        return args.top();
    }

    Value* rgb(argstack& args){
        if(args.size() != 3 && args.size() != 4){
            throw std::runtime_error("Incorrect number of arguments: expected 3 or 4, got: "+std::to_string(args.size()));
        }
        RGB rgb;
        assert_type(*args.top(), NUMBER);
        rgb.r = ((NumberValue*)args.top())->number;
        args.pop();
        assert_type(*args.top(), NUMBER);
        rgb.g = ((NumberValue*)args.top())->number;
        args.pop();
        assert_type(*args.top(), NUMBER);
        rgb.b = ((NumberValue*)args.top())->number;
        args.pop();
        if(!args.empty()){
            assert_type(*args.top(), NUMBER);
            rgb.a = ((NumberValue*)args.top())->number;
        }
        return new ColorValue(rgb);        
    }

    Value* hsv(argstack& args){
        if(args.size() != 3 && args.size() != 4){
            throw std::runtime_error("Incorrect number of arguments: expected 3 or 4, got: "+std::to_string(args.size()));
        }
        HSV hsv;
        assert_type(*args.top(), NUMBER);
        hsv.h = ((NumberValue*)args.top())->number;
        args.pop();
        assert_type(*args.top(), NUMBER);
        hsv.s = ((NumberValue*)args.top())->number;
        args.pop();
        assert_type(*args.top(), NUMBER);
        hsv.v = ((NumberValue*)args.top())->number;
        args.pop();
        if(!args.empty()){
            assert_type(*args.top(), NUMBER);
            hsv.a = ((NumberValue*)args.top())->number;
        }
        return new ColorValue(toRGB(hsv));        
    }

    Value* palette(argstack& args){
        Palette palette;
        while(!args.empty()){
            Value* val = args.top();
            args.pop();
            assert_type(*val, COLOR);
            palette.push_back(((ColorValue*)val)->color);
        }
        return new PaletteValue(palette);        
    }

    Value* push(argstack& args){
        while(!args.empty()){
            Value* arg = args.top();
            args.pop();
            assert_type(*arg, STRING);
            std::string varname = ((StringValue*)arg)->string;
            Value* val = ProgramState::get(varname);
            ProgramState::symbolStack[varname].push(val->copy());
        }
        return nullptr;        
    }

    Value* pop(argstack& args){
        while(!args.empty()){
            Value* arg = args.top();
            args.pop();
            assert_type(*arg, STRING);
            std::string varname = ((StringValue*)arg)->string;
            auto it = ProgramState::symbolStack.find(varname);
            if(it == ProgramState::symbolStack.end() || it->second.empty()){
                throw std::runtime_error(varname+" has no stacked value");
            }
            ProgramState::set(varname, it->second.top());
            it->second.pop();
        }
        return nullptr;
    }

    Value* stack(argstack& ){
        for(auto& pair: ProgramState::symbolStack){
            if(pair.second.empty()) continue;
            auto it = ProgramState::symbolTable.find(pair.first);
            std::cout << pair.first << " = " << (it == ProgramState::symbolTable.end()? "null": it->second->toString()) << std::endl;
            argstack tmp;
            while(!pair.second.empty()){
                std::cout << "    = " << pair.second.top()->toString() << std::endl;
                tmp.push(pair.second.top());
                pair.second.pop();
            }
            while(!tmp.empty()){
                pair.second.push(tmp.top());
                tmp.pop();
            }
        }
        return nullptr;
    }
    Value* closest_rgb(argstack& args){
        assert_arity(args, 1);
        assert_type(*args.top(), PALETTE);
        auto pcsv = new PaletteColorStrategyValue;
        pcsv->palette = ((PaletteValue*)args.top())->palette;
        pcsv->picker = [](const Palette& p, const RGB& rgb)->RGB{
                return closestByColor(p, rgb)[0];
            };
        return pcsv;
    }
    Value* direct(argstack& args){
        assert_arity(args, 0);
        return new DirectQuantizerValue();
    }
    Value* dither_ord(argstack& args){
        if(args.size() > 3){
            throw std::runtime_error("Incorrect number of arguments: got: "+std::to_string(args.size()));
        }
        std::string matrix="Bayes2";
        float sparsity = 3;
        float threshold = 0;
        if(!args.empty()){
            assert_type(*args.top(), STRING);
            matrix = ((StringValue*)args.top())->string;
            args.pop();
        }
        if(!args.empty()){
            assert_type(*args.top(), NUMBER);
            sparsity = ((NumberValue*)args.top())->number;
            args.pop();
        }
        if(!args.empty()){
            assert_type(*args.top(), NUMBER);
            threshold = ((NumberValue*)args.top())->number;
        }
        auto it = matrices.find(matrix);
        if(it == matrices.end()) throw std::runtime_error(matrix+" is not an ordered dither matrix");
        auto dither = new OrderedDitherQuantizerValue();
        dither->matrixName = matrix;
        dither->sparsity = sparsity;
        dither->threshold = threshold;
        return dither;
    }
    Value* quantize(argstack& args){
        assert_arity(args, 3);
        assert_type(*args.top(), IMAGE);
        ImageValue *img =(ImageValue*)args.top();
        args.pop();
        assert_type(*args.top(), COLORSTRATEGY);
        ColorStrategyValue *strategy =(ColorStrategyValue*)args.top();
        args.pop();
        assert_type(*args.top(), QUANTIZER);
        QuantizerValue *quant =(QuantizerValue*)args.top();
        std::cout << "..." << std::endl;
        quant->apply(img->image, strategy);
        std::cout << "done" << std::endl;
        ProgramState::maybeRefresh(img);
        return img;
    }

    const std::map<std::string, BuiltInFunction> BuiltInFunctions = {
        {"closest_rgb", closest_rgb},
        {"direct", direct},
        {"dither_ord", dither_ord},
        {"quantize", quantize},
        {"stack", stack},
        {"push", push},
        {"pop", pop},
        {"palette", palette},
        {"hsv", hsv},
        {"rgb", rgb},
        {"show", show},
        {"save", save},
        {"load", load},
        {"print", print},
        {"env", env},
        {"quit", exit},
        {"exit", exit}
    };
}