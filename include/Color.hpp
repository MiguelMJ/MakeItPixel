#ifndef __MIPA_COLOR_HPP__
#define __MIPA_COLOR_HPP__

#include <iostream>
#include <SFML/Graphics.hpp>

namespace mipa{
    typedef sf::Color RGB;
    typedef struct {
        float h=0;
        float s=0;
        float v=0;
        int a=0x255;
    } HSV;  

    extern float RED_BRIGHTNESS;
    extern float GREEN_BRIGHTNESS;
    extern float BLUE_BRIGHTNESS;

    RGB toRGB(const HSV& hsv);
    HSV toHSV(const RGB& rgb);

    RGB shiftHue(const RGB& color, float angle);
    RGB saturation(const RGB& color, float t);
    RGB normalized(const RGB& color, const RGB& min, const RGB& max);
    RGB grayScale(const RGB& color);
    float grayValue(const RGB& color);

    float rgbDistance(const RGB& a, const RGB& b);
    float rgbSquaredDistance(const RGB& a, const RGB& b);
    float grayDistance(const RGB& a, const RGB& b);
    RGB lerp(const RGB& from, const RGB& to, float t);

    std::istream& operator >> (std::istream& in, RGB& color);
    std::ostream& operator << (std::ostream& out, const RGB& color);
}

#endif