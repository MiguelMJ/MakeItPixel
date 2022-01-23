#include "Color.hpp"

#include <cmath>

namespace mipa{
    float RED_BRIGHTNESS = .241;
    float GREEN_BRIGHTNESS = .601;
    float BLUE_BRIGHTNESS = .068;

    RGB toRGB(const HSV& hsv){
        RGB rgb;
        rgb.a = hsv.a;
        int max = hsv.v * 255;
        if(hsv.s <= 0){ // gray
            rgb.r = max;
            rgb.g = max;
            rgb.b = max;
            return rgb;
        }
        float hue = hsv.h;
        while(hue >= 360.0) hue -= 360.0;
        hue /= 60.0;
        
        long i = (long)hue;
        double ff = hue - i;
        float p = max * (1.0 - hsv.s);
        float q = max * (1.0 - (hsv.s * ff));
        float t = max * (1.0 - (hsv.s * (1.0 -ff)));
        switch(i) {
        case 0:
            rgb.r = max;
            rgb.g = t;
            rgb.b = p;
            break;
        case 1:
            rgb.r = q;
            rgb.g = max;
            rgb.b = p;
            break;
        case 2:
            rgb.r = p;
            rgb.g = max;
            rgb.b = t;
            break;
        case 3:
            rgb.r = p;
            rgb.g = q;
            rgb.b = max;
            break;
        case 4:
            rgb.r = t;
            rgb.g = p;
            rgb.b = max;
            break;
        case 5:
        default:
            rgb.r = max;
            rgb.g = p;
            rgb.b = q;
            break;
        }
        return rgb;
    }

    HSV toHSV(const RGB& rgb){
        HSV hsv;
        hsv.a = rgb.a;
        float r = (float)rgb.r / 255;
        float g = (float)rgb.g / 255;
        float b = (float)rgb.b / 255;
        double min = std::min(std::min(r, g), b);
        double max = std::max(std::max(r, g), b);
        double delta = max - min;
        hsv.v = max;

        if(delta < 0.00001 || max <= 0){ // gray or black
            hsv.s = 0;
            hsv.h = 0; // undefined
            return hsv;
        }
        hsv.s = delta / max;
        if(r >= max){
            hsv.h = (rgb.g - rgb.b) / delta;
        }else if(g >= max){
            hsv.h = 2.0 + (rgb.b - rgb.r) / delta;
        }else{
            hsv.h = 4.0 + (rgb.r - rgb.g) / delta;
        }
        hsv.h *= 60.0;
        if(hsv.h < 0){
            hsv.h += 360.0;
        }
        return hsv;
    }

    RGB shiftHue(const RGB& color, float angle){
        HSV hsv = toHSV(color);
        hsv.h += angle;
        return toRGB(hsv);
    }
    RGB saturation(const RGB& color, float t){
        HSV hsv = toHSV(color);
        hsv.s = t;
        return toRGB(hsv);
    }
    RGB normalized(const RGB& color, const RGB& min, const RGB& max){
        return RGB(
            (color.r - min.r) / (max.r - min.r),
            (color.g - min.g) / (max.g - min.g),
            (color.b - min.b) / (max.b - min.b)
        );
    }
    RGB grayScale(const RGB& color){
        int gray = std::sqrt(
            color.r * color.r * RED_BRIGHTNESS +
            color.g * color.g * GREEN_BRIGHTNESS +
            color.b * color.b * BLUE_BRIGHTNESS
        );
        return RGB(gray, gray, gray);
    }
    float grayValue(const RGB& color){
        return std::sqrt(
            color.r * color.r * RED_BRIGHTNESS +
            color.g * color.g * GREEN_BRIGHTNESS +
            color.b * color.b * BLUE_BRIGHTNESS
        ) / 255.0; 
    }

    float rgbDistance(const RGB& a, const RGB& b){
        return std::sqrt(rgbSquaredDistance(a, b));
    }
    float rgbSquaredDistance(const RGB& a, const RGB& b){
        int dr = (int)a.r - b.r;
        int dg = (int)a.g - b.g;
        int db = (int)a.b - b.b;
        return dr * dr + dg * dg + db * db;
    }
    float grayDistance(const RGB& a, const RGB& b){
        return std::abs(grayValue(a) - grayValue(b));
    }

    RGB lerp(const RGB& from, const RGB& to, float t){
        return RGB(
            from.r + ((int)to.r - from.r) * t,
            from.g + ((int)to.g - from.g) * t,
            from.b + ((int)to.b - from.b) * t,
            from.a + ((int)to.a - from.a) * t
        );
    }

    std::istream& operator >> (std::istream& in, RGB& color){
        std::string hexRepr;
        in >> hexRepr;
        color = RGB((std::stoi(hexRepr, nullptr, 16) << 8) | 0xff);
        return in;
    }

    std::ostream& operator << (std::ostream& out, const RGB& color){
        out << std::hex << (color.toInteger() >> 8) << std::dec;
        return out;
    }
}