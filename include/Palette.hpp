/**
 * @file Palette.hpp
 * @copyright MIT License
 * @author Miguel Mejía Jiménez
 * @brief This file contains the functions to manage color palettes.
 * 
 * As a palette is just a vector of colors, they are easy to manipulate with
 * any standard function and to hardcode as bracket enclosed lists.
 * 
 */
#ifndef __MIPA_PALETTE_HPP__
#define __MIPA_PALETTE_HPP__

#include <vector>
#include <string>

#include "Color.hpp"

namespace mipa{
    /**
     * @brief A palette is a collection of colors.
     */
    typedef std::vector<RGB> Palette;

    /**
     * @brief Return a copy of the palette, sorted by their gray value.
     * Darker colors first.
     * @param palette 
     * @return Palette
     */
    Palette graySorted(Palette palette);

    /**
     * @brief Append two palettes, adding an number of intermediate colors
     * interpolated between the last color of the first palette, and the
     * first color of the second one.
     * 
     * If @p steps is 0, just append both palettes.
     * 
     * @param from First palette
     * @param to Second palette
     * @param steps Number of intermediate colors
     * @return Palette 
     */
    Palette gradient(Palette from, const Palette& to, int steps);

    /**
     * @brief Load a palette from a text file.
     * 
     * The format of the palette is an hex value per line.
     * 
     * @param filename Path of the file
     * @return Palette 
     */
    Palette loadPalette(const std::string& filename);

    /**
     * @brief Save a palette into a text file.
     * 
     * The format of the palette is an hex value per line.
     * 
     * @param palette Palette to save
     * @param filename Path of the file
     */
    void savePalette(const Palette& palette, const std::string& filename);

    /**
     * @brief Return a copy of the input palette, with the colors sorted by 
     * similarity to a given color.
     * 
     * @param palette Input palette
     * @param color Reference color
     * @return Palette 
     */
    Palette closestByColor(Palette palette, const RGB& color);

    /**
     * @brief Return a copy of the input palette, with the colors sorted by
     * similarity of their gray value to the gray value of a given color.
     * 
     * @param palette Input palette
     * @param color Reference color
     * @return Palette 
     */
    Palette closestByBrightness(Palette palette, const RGB& color);
}

#endif