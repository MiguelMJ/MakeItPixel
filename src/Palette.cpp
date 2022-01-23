#include "Palette.hpp"

#include <fstream>
#include <iostream>
#include <algorithm>

namespace mipa{
    Palette graySorted(Palette palette){
        std::sort(palette.begin(), palette.end(), [](RGB a, RGB b) -> bool {
            return grayValue(a) < grayValue(b);
        });
        return palette;
    }
    Palette gradient(Palette from, const Palette& to, int steps){
        Palette& palette = from;
        RGB last = *from.rbegin();
        float step = 1.f / (steps+1);
        for(int i = 1; i <= steps; i++){
            palette.push_back(lerp(last, to[0], step * i));
        }
        palette.insert(palette.end(), to.begin(), to.end());
        return palette;
    }
    Palette loadPalette(const std::string& filename){
        std::ifstream fin(filename);
        if(!fin.good()){
            std::string msg = "loadPalette: can't open file: " + filename;
            fin.close();
            throw std::runtime_error(msg);
        }
        Palette palette;
        RGB color;
        fin >> color;
        while(!fin.eof()){
            palette.push_back(color);
            fin.ignore(100, '\n');
            fin >> color;
        }
        fin.close();
        return palette;
    }
    void savePalette(const Palette &palette, const std::string& filename){
        std::ofstream fout(filename);
        if(!fout.good()){
            std::string msg = "savePalette: can't open file: " + filename;
            fout.close();
            throw std::runtime_error(msg);
        }
        for(const auto& color: palette){
            fout << color << std::endl;
        }
        fout.close();
    }
    Palette closestByColor(Palette palette, const RGB& color){
        std::sort(palette.begin(), palette.end(), [color](const RGB& a, const RGB& b) -> bool {
            auto c = color;
            return rgbSquaredDistance(c, a) < rgbSquaredDistance(c, b);
        });
        return palette;
    }
    Palette closestByBrightness(Palette palette, const RGB& color){
        std::sort(palette.begin(), palette.end(), [color](const RGB& a, const RGB& b) -> bool {
            auto c = color;            
            auto key = grayValue(c);
            return std::abs(key - grayValue(a)) < std::abs(key - grayValue(b));
        });
        return palette;
    }
}