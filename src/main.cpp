#include<iostream>

#include <SFML/Graphics.hpp>
#include "Quantization.hpp"

using namespace std;
using namespace mipa;

int main(){

    sf::Image image0, image1, image2, image3, image4;
    if(!image0.loadFromFile("foto.jpg")){
        throw "mec";
    }
    image1 = image0;
    image2 = image0;
    image3 = image0;
    image4 = image0;

    Palette palette1 = {RGB::Black, RGB::White};
    Palette palette2 = mipa::gradient({RGB::Magenta, RGB::Red},{RGB::Yellow, RGB::Green, RGB::Black},3);
    Palette palette3 = mipa::gradient({RGB::Magenta, RGB::Red},{RGB::Yellow, RGB::Cyan, RGB::Blue, mipa::lerp(RGB::Cyan, RGB::Blue, 0.5), mipa::lerp(RGB::Red, RGB::Black, 0.85)},3);
    Palette neonPalette = {RGB::Magenta, RGB::Blue, RGB::Yellow, RGB::Red, RGB::Black, RGB::White};

    Palette palette = neonPalette;
    
    auto quant = [&palette](mipa::RGB c)->mipa::RGB{
        // mipa::Palette palette = {mipa::MAGENTA, mipa::GREEN, mipa::BLUE, mipa::WHITE};
        return closestByColor(palette, c)[0];
    };

    mipa::directQuantize(image0, quant);
    image0.saveToFile("output0.png");
    mipa::ditherOrdered(image1, quant, mipa::Bayes2, 4);
    image1.saveToFile("output1.png");
    mipa::ditherOrdered(image2, quant, mipa::Vertical2, 4);
    image2.saveToFile("output2.png"); 
    mipa::ditherOrdered(image3, quant, mipa::Vertical4, 4);
    image3.saveToFile("output3.png"); 
    mipa::ditherOrdered(image4, quant, mipa::Heart, 5);
    image4.saveToFile("output4.png"); 
    
    /* mipa::RGB c = mipa::BLACK;
    for(float i = 0.f; i <= 1.f; i+= 1.f/5){
        c = mipa::BLACK;
        auto cc = c;
        cc.lerp(mipa::WHITE, i);
        cout << c << " " << cc << endl;
    } */
    
    return 0;
}