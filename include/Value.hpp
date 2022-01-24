#ifndef __MIPA_VALUE__
#define __MIPA_VALUE__

#include <string>

namespace mipa{
    typedef enum {COLOR, PALETTE, IMAGE, QUANTIZER, COLORER, NUMBER, STRING} ValueType;
    struct Value{
        ValueType type;
        inline Value(ValueType t): type(t){}
    };
    struct NumberValue: public Value{
        float number;
        inline NumberValue(float n): Value(NUMBER), number(n){}
    };
    struct StringValue: public Value{
        std::string string;
        inline StringValue(std::string s):Value(STRING), string(s){}
    };
}

#endif