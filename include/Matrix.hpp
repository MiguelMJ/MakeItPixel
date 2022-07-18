/**
 * @file
 * @author Miguel Mejía Jiménez
 * @copyright MIT License
 * @brief This file contains a basic Matrix struct.
 */
#ifndef __MIPA_MATRIX_HPP__
#define __MIPA_MATRIX_HPP__

#include <vector>

namespace mipa {
    struct Matrix {
        int h, w;
        std::vector<float> elements;
        int getHeight() const;
        int getWidth() const;
        float get(int r, int c) const;
    };
}

#endif