#include<iostream>

#include <SFML/Graphics.hpp>
#include "Quantization.hpp"

using namespace std;
using namespace mipa;

int main(){

    sf::Image image, image2;
    if(!image.loadFromFile("image.jpg")){
        throw "mec";
    }
    image2 = image;

    Palette palette1 = {RGB::Black, RGB::White};
    Palette palette2 = mipa::gradient({RGB::Magenta, RGB::Red},{RGB::Yellow, RGB::Green, RGB::Black},3);
    Palette palette3 = mipa::gradient({RGB::Magenta, RGB::Red},{RGB::Yellow, RGB::Cyan, RGB::Blue, mipa::lerp(RGB::Cyan, RGB::Blue, 0.5), mipa::lerp(RGB::Red, RGB::Black, 0.85)},3);

    Palette palette = palette1;
    
    auto quant = [&palette](mipa::RGB c)->mipa::RGB{
        // mipa::Palette palette = {mipa::MAGENTA, mipa::GREEN, mipa::BLUE, mipa::WHITE};
        return closestByColor(palette, c)[0];
    };

    mipa::ditherFloydSteinberg(image, quant, 0);
    image.saveToFile("output.png");
    mipa::ditherOrdered(image2, quant, mipa::Bayes2x2, 1.7, 0);
    image2.saveToFile("output2.png"); 
    
    /* mipa::RGB c = mipa::BLACK;
    for(float i = 0.f; i <= 1.f; i+= 1.f/5){
        c = mipa::BLACK;
        auto cc = c;
        cc.lerp(mipa::WHITE, i);
        cout << c << " " << cc << endl;
    } */
    
    return 0;
}