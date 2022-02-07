/**
 * @file
 * @author Miguel Mejía Jiménez
 * @copyright MIT License
 * @brief This file contains basic color operations.
 * 
 * The functions in this file extend the operations directly available for
 * the sf::Color class from the SFML library. To perform some of them, a
 * HSV representation is used, but all the returned and received values
 * are represented in RGB space. This might result in unnecessary intermediate
 * conversions between different operations, but it shouldn't affect performance
 * significatively.
 * 
 * It provides also three global parameters used in the grayscale calculations.
 * It is not realistic to weight each channel in the RGB space equally to
 * compute the gray value, as yellow is perceived brighter and blue, darker.
 * 
 * These functions not only include color operations but also implementations
 * for stream read and write operators. Note that, given the purpose of this
 * whole software package, the alpha value is ignored for input/output.
 * 
 */
#ifndef __MIPA_COLOR_HPP__
#define __MIPA_COLOR_HPP__

#include <iostream>
#include <SFML/Graphics.hpp>

namespace mipa{
    /**
     * @brief Alias to simplify and ease possible future migration from SFML. 
     */
    typedef sf::Color RGB;
    
    /**
     * @brief Auxiliar structure to represent color in HSV space.
     */
    typedef struct {
        float h=0;
        float s=0;
        float v=0;
        int a=0x255;
    } HSV;  

    /**
     * @brief Factor to multiply the red value of color to compute the grayscale.
     */
    extern float RED_BRIGHTNESS;
    
    /**
     * @brief Factor to multiply the green value of color to compute the grayscale.
     */
    extern float GREEN_BRIGHTNESS;
    
    /**
     * @brief Factor to multiply the blue value of color to compute the grayscale.
     */
    extern float BLUE_BRIGHTNESS;

    /**
     * @brief Convert color from HSV space to RGB. Keep the alpha value.
     * 
     * @param hsv Input color in HSV space.
     * @return RGB
     * @see toHSV
     */
    RGB toRGB(const HSV& hsv);

    /**
     * @brief Convert color from RGB space to HSV. Keep the alpha value.
     * 
     * @param rgb Input color in RGB space.
     * @return HSV
     * @see toRGB 
     */
    HSV toHSV(const RGB& rgb);

    /**
     * @brief Copy a color with an offset in the hue.
     * 
     * - Offset ± 180º to obtain the complementary color.
     * - Offset ± 30º to obtain analogous colors.
     * - Offset ± 160º to obtain split complementary colours.
     * - Offset ± 120º to obtain triadic colors.
     * 
     * @param color Input color.
     * @param angle Offset in the chromatic wheel.
     * @return RGB 
     */
    RGB shiftHue(const RGB& color, float angle);

    /**
     * @brief Copy the color with a different saturation.
     * 
     * @param color Input color.
     * @param saturation New saturation.
     * @return RGB 
     */
    RGB saturation(const RGB& color, float s);

    /**
     * @brief Copy the color normalized between a minimum and a maximum.
     * 
     * Normalize each channel separately.
     * 
     * @param color Input color.
     * @param min Color with minimum values for each channel.
     * @param max Color with maximum values for each channel.
     * @return RGB 
     */
    RGB normalized(const RGB& color, const RGB& min, const RGB& max);

    /**
     * @brief Return the gray color corresponding to the input color.
     * 
     * @param color Input color.
     * @return RGB
     * @see RED_BRIGHTNESS 
     * @see BLUE_BRIGHTNESS 
     * @see GREEN_BRIGHTNESS 
     */
    RGB grayScale(const RGB& color);

    /**
     * @brief Return the normalized gray value of the input color.
     * 
     * @param color 
     * @return float
     * @see RED_BRIGHTNESS 
     * @see BLUE_BRIGHTNESS 
     * @see GREEN_BRIGHTNESS 
     */
    float grayValue(const RGB& color);

    /**
     * @brief Euclidean distance in the RGB space between two colors.
     * 
     * @param a 
     * @param b 
     * @return float 
     */
    float rgbDistance(const RGB& a, const RGB& b);

    /**
     * @brief Squared euclidean distance in the RGB space between two colors.
     * 
     * @param a 
     * @param b 
     * @return float 
     */
    float rgbSquaredDistance(const RGB& a, const RGB& b);

    /**
     * @brief Absolute difference between the normalized gray values of each 
     * input color.
     * 
     * @param a 
     * @param b 
     * @return float 
     */
    float grayDistance(const RGB& a, const RGB& b);
    RGB lerp(const RGB& from, const RGB& to, float t);

    /**
     * @brief Read from the stream the hex value of the color. Ignore the alpha 
     * channel.
     * 
     * @param in 
     * @param color 
     * @return std::istream& 
     */
    std::istream& operator >> (std::istream& in, RGB& color);

    /**
     * @brief Write to the stream the hex value of the color. Ignore the alpha
     * channel.
     * 
     * @param out 
     * @param color 
     * @return std::ostream& 
     */
    std::ostream& operator << (std::ostream& out, const RGB& color);
}

#endif