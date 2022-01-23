#ifndef __MIPA_PALETTE_HPP__
#define __MIPA_PALETTE_HPP__

#include <vector>
#include <string>

#include "Color.hpp"

namespace mipa{
    typedef std::vector<RGB> Palette;

    Palette graySorted(Palette palette);
    Palette gradient(Palette from, const Palette& to, int size);
    Palette loadPalette(const std::string& filename);
    void savePalette(const Palette& palette, const std::string& filename);
    Palette closestByColor(Palette palette, const RGB& color);
    Palette closestByBrightness(Palette palette, const RGB& color);
}

#endif