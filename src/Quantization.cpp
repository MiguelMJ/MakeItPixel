#include "Quantization.hpp"

#include <cmath>

namespace mipa{
    int Matrix::getWidth() const{
        return w;
    } 
    int Matrix::getHeight() const{
        return h;
    } 
    float Matrix::get(int r, int c) const{
        return elements.at(r*w+c);
    } 

    const Matrix Bayes2 = {
        2, 2,
        {
            0, 2,
            3, 1
        }
    };
    const Matrix Bayes4 = {
        4, 4,
        {
            0, 8, 2, 10,
            12, 4, 14, 6,
            3, 11, 1, 9,
            15, 7, 13, 5
        }
    };
    const Matrix Bayes8 = {
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
    const Matrix Horizontal2 = {
        2,2,
        {
            0,1,
            3,3
        }
    };
    const Matrix Horizontal4 = {
        4,4,
        {
            0, 5, 0, 5,
            5, 5, 5, 5,
            15, 10, 15, 10,
            15, 15, 15, 15
        }
    };
    const Matrix Vertical2 = {
        2,2,
        {
            0,3,
            1,3
        }
    };
    const Matrix Vertical4 = {
        4,4,
        {
            0,5,15,15,
            5,5,10,15,
            0,5,15,15,
            5,5,10,15
        }
    };
    const Matrix Heart = {
        8, 8, {
            00,63,63,00,63,63,00,00,
            63,30,30,63,30,30,63,00,
            63,30,15,30,15,30,63,00,
            63,30,15,15,15,30,63,00,
            00,63,30,15,30,63,00,00,
            01,00,63,30,63,00,00,01,
            02,04,00,63,00,00,04,02,
            03,05,06,00,00,06,05,03,
        }
    };
}