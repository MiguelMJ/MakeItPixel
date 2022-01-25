#ifndef __MIPA_VALUE__
#define __MIPA_VALUE__

#include <string>
#include <sstream>

#include <SFML/Graphics.hpp>

#include "Color.hpp"
#include "Palette.hpp"
#include "Quantization.hpp"

namespace mipa{
    typedef enum {
        COLOR = 1, 
        PALETTE = 2,
        IMAGE = 4, 
        QUANTIZER = 8, 
        COLORSTRATEGY = 16, 
        NUMBER = 32, 
        STRING = 64
    } ValueType;
    struct Value{
        ValueType type;
        inline Value(ValueType t): type(t){}
        virtual std::string toString() const = 0;
        virtual Value* copy() const = 0;
        virtual ~Value(){}
    };
    struct NumberValue: public Value{
        float number;
        inline NumberValue(float n): Value(NUMBER), number(n){}
        inline std::string toString() const override{
            return std::to_string(number);
        }
        inline Value* copy() const override{
            return new NumberValue(number);
        }
    };
    struct StringValue: public Value{
        std::string string;
        inline StringValue(std::string s):Value(STRING), string(s){}
        inline std::string toString() const override{
            return string;
        }
        inline Value* copy() const override{
            return new StringValue(string);
        }
    };
    struct ColorValue: public Value{
        RGB color;
        inline ColorValue(const RGB& rgb):Value(COLOR),color(rgb){}
        inline std::string toString() const override{
            std::stringstream ss;
            ss << color;
            return ss.str();
        }        
        inline Value* copy() const override{
            return new ColorValue(color);
        }
    };
    struct PaletteValue: public Value{
        Palette palette;
        inline PaletteValue(const Palette& p):Value(PALETTE),palette(p){}
        inline std::string toString() const override{
            std::stringstream ss;
            ss << "[";
            for(uint i=0; i < palette.size(); i++){
                ss << palette[i];
                if(i < palette.size()-1){
                    ss << ", ";
                }
            }
            ss << "]";
            return ss.str();
        }
        inline Value* copy() const override{
            return new PaletteValue(palette);
        }
    };
    struct ImageValue: public Value{
        sf::Image image;
        inline ImageValue():Value(IMAGE){}
        inline ImageValue(const sf::Image& i):Value(IMAGE),image(i){}
        inline std::string toString() const override{
            sf::Vector2u imgSize = image.getSize();
            return "{Image: "+std::to_string(imgSize.x)+"x"+std::to_string(imgSize.y)+"}";
        }
        inline Value* copy() const override{
            return new ImageValue(image);
        }
    };
    struct ColorStrategyValue: public Value{
        inline ColorStrategyValue(): Value(COLORSTRATEGY){}
        virtual RGB operator()(const RGB& rgb) const{
            return rgb;
        };
        virtual std::string toString() const{
            return "{Empty Color Strategy}";
        };
        virtual Value* copy() const override{
            return new ColorStrategyValue;
        }
    };
    struct PaletteColorStrategyValue: public ColorStrategyValue{
        Palette palette;
        RGB (*picker)(const Palette& p, const RGB& in);
        inline RGB operator()(const RGB& rgb) const override{
            return picker(palette, rgb);
        }
        inline std::string toString() const override{
            std::stringstream ss;
            ss << "{Palette Picker " << PaletteValue(palette).toString() << "}";
            return ss.str();
        };
        inline Value* copy() const override{
            auto pcsv = new PaletteColorStrategyValue;
            pcsv->palette = palette;
            pcsv->picker = picker;
            return pcsv;
        }
    };
    struct QuantizerValue: public Value{
        inline QuantizerValue(): Value(QUANTIZER){}
        virtual void apply(sf::Image& img, ColorStrategyValue* strategy) const = 0;
        virtual std::string toString() const = 0;
        virtual Value* copy() const = 0;
    };
    struct DirectQuantizerValue: public QuantizerValue{
        void apply(sf::Image& img, ColorStrategyValue* strategy) const override{
            directQuantize(img, strategy);
        }
        inline std::string toString() const override{
            return "{Direct Quantizer}";
        }
        inline Value* copy() const override{
            return new DirectQuantizerValue();
        }
    };
    struct OrderedDitherQuantizerValue: public QuantizerValue{
        std::string matrixName;
        float sparsity, threshold;
        void apply(sf::Image& img, ColorStrategyValue* strategy) const override{
            ditherOrdered(img, strategy, matrices.at(matrixName), sparsity, threshold);
        }
        inline std::string toString() const override{
            return "{Ordered Dither: "+matrixName+"}";
        }
        inline Value* copy() const override{
            return new DirectQuantizerValue();
        }
    };

}

#endif