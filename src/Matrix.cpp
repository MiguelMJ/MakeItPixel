#include "Matrix.hpp"

namespace mipa {
    int Matrix::getWidth() const{
        return w;
    } 
    int Matrix::getHeight() const{
        return h;
    } 
    float Matrix::get(int r, int c) const{
        return elements.at(r*w+c);
    } 
}