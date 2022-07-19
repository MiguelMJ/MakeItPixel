#ifndef __MIPA_IMAGE_PROCESSING_HPP__
#define __MIPA_IMAGE_PROCESSING_HPP__

#include <SFML/Graphics.hpp>
#include "Matrix.hpp"
#include "Palette.hpp"

namespace mipa {
    template <typename F>
    void apply(sf::Image& image, const F& fun) {
        for(int i=0; i < image.getSize().x; i++){
            for(int j=0; j < image.getSize().y; j++){
                image.setPixel(i,j,fun(image.getPixel(i, j)));
            }
        }
    }

    template <typename F>
    sf::Image copyAndApply(sf::Image image, const F& fun);

    sf::Image copyAndApply(sf::Image image, void (*fun)(sf::Image&));

    void normalize(sf::Image& image);

    sf::Image pixelize(const sf::Image& image, uint max_width, uint max_height, const std::string& selector="avg");

    sf::Image apply_convolution(sf::Image image, const Matrix& m);

    sf::Image sobel(sf::Image image, float threshold=0.f);

    Palette extractPaletteSimple(const sf::Image& image, int factor=3, bool allow_unexistent=true);

    Palette extractPaletteMedianCut(const sf::Image& image, int cuts=3, bool allow_unexistent=true);
}

#endif