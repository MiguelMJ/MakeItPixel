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
            throw std::runtime_error("Incorrect type: expected "+typenames[type]+", got: "+typenames[val.type] + " " + val.toString());
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
            switch(val->type){
                case COLOR:
                    palette.push_back(((ColorValue*)val)->color);
                break;
                case PALETTE:{
                    auto& subpal = ((PaletteValue*)val)->palette;
                    palette.insert(palette.end(), subpal.begin(), subpal.end());
                }break;
                default:
                    throw std::runtime_error("Incorrect type: expected Color or Palette, got: "+typenames[val->type] + " " + val->toString());
            }
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
    
    Value* closest_gray(argstack& args){
        assert_arity(args, 1);
        assert_type(*args.top(), PALETTE);
        auto pcsv = new PaletteColorStrategyValue;
        pcsv->palette = ((PaletteValue*)args.top())->palette;
        pcsv->picker = [](const Palette& p, const RGB& rgb)->RGB{
                return closestByBrightness(p, rgb)[0];
            };
        return pcsv;
    }
    
    Value* closest_hue(argstack& args){
        assert_arity(args, 1);
        assert_type(*args.top(), PALETTE);
        auto pcsv = new PaletteColorStrategyValue;
        pcsv->palette = ((PaletteValue*)args.top())->palette;
        pcsv->picker = [](const Palette& p, const RGB& rgb)->RGB{
                float mindiff = 360.f;
                int index = -1;
                float h = toHSV(rgb).h;
                for(uint i = 0; i < p.size(); i++){
                    if(std::abs(toHSV(p[i]).h - h) < mindiff){
                        index = i;
                    }
                }
                return p.at(index);
            };
        return pcsv;
    }
    
    Value* direct(argstack& args){
        assert_arity(args, 0);
        return new DirectQuantizerValue();
    }
    
    Value* dither_ord(argstack& args){
        if(args.size() > 3){
            throw std::runtime_error("Incorrect number of arguments. Expected 0, 1, 2 or 3: got "+std::to_string(args.size()));
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
    
    Value* dither_fs(argstack& args){
        if(args.size() > 1){
            throw std::runtime_error("Incorrect number of arguments. Expected 0 or 1: got "+std::to_string(args.size()));
        }
        float threshold = 0;
        if(!args.empty()){
            assert_type(*args.top(), NUMBER);
            threshold = ((NumberValue*)args.top())->number;
        }
        auto dither = new FSDitherQuantizerValue();
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
    
    Value* shiftHueRightOperator(argstack& args){
        assert_type(*args.top(), NUMBER);
        float angle = ((NumberValue*)args.top())->number;
        args.pop();
        Value* op = args.top();
        switch(op->type){
            case COLOR:{
                auto ret = new ColorValue(shiftHue(((ColorValue*)op)->color, angle));
                return ret;
                }
            case PALETTE:{
                auto pal = (PaletteValue*)op->copy();
                for(auto& c: pal->palette){
                    c = shiftHue(c, angle);
                }
                return pal;
            }
            break;
            default:
            throw std::runtime_error("Incorrect type: expected color or palette, got: "+typenames[op->type]);
            break;                
        }
        return nullptr;
    }
    
    Value* shiftHueLeftOperator(argstack& args){
        assert_type(*args.top(), NUMBER);
        float angle = -((NumberValue*)args.top())->number;
        args.pop();
        Value* op = args.top();
        switch(op->type){
            case COLOR:{
                auto ret = new ColorValue(shiftHue(((ColorValue*)op)->color, angle));
                return ret;
                }
            case PALETTE:{
                auto pal = (PaletteValue*)op->copy();
                for(auto& c: pal->palette){
                    c = shiftHue(c, angle);
                }
                return pal;
            }
            break;
            default:
            throw std::runtime_error("Incorrect type: expected color or palette, got: "+typenames[op->type]);
            break;                
        }
        return nullptr;
    }
    
    Value* lightenOperator(argstack& args){
        assert_type(*args.top(), NUMBER);
        float factor = ((NumberValue*)args.top())->number;
        args.pop();
        Value* op = args.top();
        switch(op->type){
            case COLOR:{
                auto ret = new ColorValue(lerp(((ColorValue*)op)->color, RGB(0xffffffff), factor));
                return ret;
                }
            case PALETTE:{
                auto pal = (PaletteValue*)op->copy();
                for(auto& c: pal->palette){
                    c = lerp(c, RGB(0xffffffff), factor);
                }
                return pal;
            }
            break;
            default:
            throw std::runtime_error("Incorrect type: expected color or palette, got: "+typenames[op->type]);
            break;                
        }
        return nullptr;
    }
    
    Value* darkenOperator(argstack& args){
        assert_type(*args.top(), NUMBER);
        float factor = ((NumberValue*)args.top())->number;
        args.pop();
        Value* op = args.top();
        switch(op->type){
            case COLOR:{
                auto ret = new ColorValue(lerp(((ColorValue*)op)->color, RGB(0xff), factor));
                return ret;
                }
            case PALETTE:{
                auto pal = (PaletteValue*)op->copy();
                for(auto& c: pal->palette){
                    c = lerp(c, RGB(0xff), factor);
                }
                return pal;
            }
            break;
            default:
            throw std::runtime_error("Incorrect type: expected color or palette, got: "+typenames[op->type]);
            break;                
        }
        return nullptr;
    }
    
    Value* saturateOperator(argstack& args){
        assert_type(*args.top(), COLOR);
        return new ColorValue( saturation( ((ColorValue*)args.top())->color, 0.9999999 ) );
    }
    
    Value* desaturateOperator(argstack& args){
        assert_type(*args.top(), COLOR);
        return new ColorValue( grayScale ( ((ColorValue*)args.top())->color) );
    }
    
    Value* accessPaletteOperator(argstack& args){
        assert_type(*args.top(), NUMBER);
        uint index = ((NumberValue*)args.top())->number;
        args.pop();
        assert_type(*args.top(), PALETTE);
        auto& pal = ((PaletteValue*)args.top())->palette;
        if(pal.size() <= index) throw std::runtime_error("Out of range");
        return new ColorValue(pal[index]);
    }
    
    Value* gradientOperator(argstack& args){
        Value* op2 = args.top();
        args.pop();
        assert_type(*args.top(), NUMBER);
        uint steps = ((NumberValue*)args.top())->number;
        args.pop();
        Value* op1 = args.top();
        PaletteValue* ret;
        if(op1->type == op2->type){
            if(op1->type == COLOR){
                ret = new PaletteValue(gradient(
                    {((ColorValue*)op1)->color},
                    {((ColorValue*)op2)->color},
                    steps
                ));
            }else{
                ret = new PaletteValue(gradient(
                    ((PaletteValue*)op1)->palette,
                    ((PaletteValue*)op2)->palette,
                    steps
                ));
            }
        }else{
            if(op1->type == COLOR){
                ret = new PaletteValue(gradient(
                    {((ColorValue*)op1)->color},
                    ((PaletteValue*)op2)->palette,
                    steps
                ));
            }else{
                ret = new PaletteValue(gradient(
                    ((PaletteValue*)op1)->palette,
                    {((ColorValue*)op2)->color},
                    steps
                ));
            }
        }
        return ret;
    }

    Value* display(argstack& args){
        if(args.size() != 1 && args.size() != 2){
            throw std::runtime_error("Incorrect number of arguments: expected 1 or 2, got: " + std::to_string(args.size()));
        }
        assert_type(*args.top(), PALETTE);
        auto& p = ((PaletteValue*)args.top())->palette;
        args.pop();
        uint rows = 1;
        if(!args.empty()){
            assert_type(*args.top(), NUMBER);
            rows = ((NumberValue*)args.top())->number;
        }
        uint cols = p.size()/rows;
        uint width = cols * 50;
        uint height = rows * 100;
        sf::Image img;
        img.create(width, height);
        for(uint j = 0; j < height; j++){
            for(uint i = 0; i < width; i++){
                RGB color;
                try{
                    color = p[(j/100)*(p.size()/rows)+i/50];
                }catch(const std::out_of_range& err){
                    color = RGB(0);
                }
                img.setPixel(i,j,color);
            }
        }
        return new ImageValue(img);
    }


    const std::map<std::string, BuiltInFunction> BuiltInFunctions = {
        {"display", display},
        {"accessPaletteOperator", accessPaletteOperator},
        {"gradientOperator", gradientOperator},
        {"desaturateOperator", desaturateOperator},
        {"saturateOperator", saturateOperator},
        {"darkenOperator", darkenOperator},
        {"lightenOperator", lightenOperator},
        {"shiftHueLeftOperator", shiftHueLeftOperator},
        {"shiftHueRightOperator", shiftHueRightOperator},
        {"closest_rgb", closest_rgb},
        {"direct", direct},
        {"dither_fs", dither_fs},
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