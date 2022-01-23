#include "Quantization.hpp"

#include <cmath>

namespace mipa{
    int Matrix::getWidth() const{
        return w;
    } 
    int Matrix::getHeight() const{
        return w;
    } 
    float Matrix::get(int r, int c) const{
        return elements.at(r*w+c);
    } 

    const Matrix Bayes2x2 = {
        2, 2,
        {
            0, 2,
            3, 1
        }
    };
    const Matrix Bayes4x4 = {
        4, 4,
        {
            0, 8, 2, 10,
            12, 4, 14, 6,
            3, 11, 1, 9,
            15, 7, 13, 5
        }
    };
    const Matrix Bayes8x8 = {
        8, 8,
        {
            0, 32, 8, 40, 2, 34, 10, 42,
            48, 16, 56, 24, 50, 18, 58, 26,
            12, 44, 4, 36, 14, 46, 6, 38,
            60, 28, 52, 20, 62, 30, 54, 22,
            3, 35, 11, 43, 1, 33, 9, 41,
            51, 19, 59, 27, 49, 17, 57, 25,
            15, 47, 7, 39, 13, 45, 5, 37,
            63, 31, 55, 23, 61, 29, 53, 21
        }
    };
    const Matrix Horizontal = {
        0,0,{}
    };
    const Matrix Vertical = {
        0,0,{}
    };
}