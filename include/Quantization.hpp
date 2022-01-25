#ifndef __MIPA_QUANTIZATION_HPP__
#define __MIPA_QUANTIZATION_HPP__

#include <cmath>

#include <SFML/Graphics.hpp>

#include "Palette.hpp"

namespace mipa{
    template <typename F>
    void directQuantize(sf::Image& image, F quant){
        sf::Vector2u imgSize = image.getSize();        
        for(uint y = 0; y < imgSize.y; y++){
            for(uint x = 0; x < imgSize.x; x++){
                image.setPixel(x, y, (*quant)(image.getPixel(x, y)));
            }
        }
    }
    template <typename F>
    void ditherFloydSteinberg(sf::Image& image, F quant, float threshold = 0){
        sf::Vector2u imgSize = image.getSize();        
        for(uint y = 0; y < imgSize.y; y++){
            for(uint x = 0; x < imgSize.x; x++){
                RGB oldColor = image.getPixel(x,y);
                RGB newColor = (*quant)(oldColor);
                image.setPixel(x,y,newColor); 
                float err = rgbSquaredDistance(oldColor, newColor);
                if (err > threshold * threshold){
                    float rErr = (float)oldColor.r - newColor.r;
                    float gErr = (float)oldColor.g - newColor.g;
                    float bErr = (float)oldColor.b - newColor.b;
                    auto updatePixel = [&](uint xi, uint yi, float t){
                        if(xi >= imgSize.x || yi >= imgSize.y) return; // unsigned so negative overflow
                        RGB p = image.getPixel(xi, yi);
                        p.r = std::max(0.f, std::min(255.f, (float)p.r + (rErr * t)));
                        p.g = std::max(0.f, std::min(255.f, (float)p.g + (gErr * t)));
                        p.b = std::max(0.f, std::min(255.f, (float)p.b + (bErr * t)));
                        image.setPixel(xi, yi, p);
                    };
                    updatePixel(x+1, y+1, 1.f/16);
                    updatePixel(x-1, y+1, 3.f/16);
                    updatePixel(x, y+1, 5.f/16);
                    updatePixel(x+1, y, 7.f/16);
                }
            }
        }
    }

    typedef struct{
        int h, w;
        std::vector<float> elements;
        int getHeight() const;
        int getWidth() const;
        float get(int r, int c) const;
    } Matrix;

    extern const std::map<std::string, Matrix> matrices;

    template <typename F>
    void ditherOrdered(sf::Image& image, F quant, const Matrix& m, float sparsity, float threshold=0){
        int N = m.getHeight() * m.getWidth();
        sf::Vector2u imgSize = image.getSize();
        for(uint y = 0; y < imgSize.y; y++){
            for(uint x = 0; x < imgSize.x; x++){
                RGB oldColor = image.getPixel(x,y);
                float mij = m.get(y % m.getHeight(), x % m.getWidth()) / N - 0.5f;
                auto clamp = [](int x)->int{return std::min(255,std::max(0,x));};
                int interR = clamp((int)oldColor.r + (255.f/sparsity) * mij);
                int interG = clamp((int)oldColor.g + (255.f/sparsity) * mij);
                int interB = clamp((int)oldColor.b + (255.f/sparsity) * mij);
                RGB newColor = (*quant)(RGB(interR, interG, interB));
                oldColor = (*quant)(oldColor);
                float err = rgbDistance(oldColor, newColor);
                if(err > threshold * threshold){
                    image.setPixel(x,y, newColor);
                }else{
                    image.setPixel(x,y, oldColor);
                }

            }
        }
    }
}

#endif