#include "ImageProcessing.hpp"

#include <algorithm>
#include <cmath>

#include <SFML/Graphics.hpp>

#include "Color.hpp"

namespace mipa {
    
    sf::Image copyAndApply(sf::Image image, void (*fun)(sf::Image&)){
        fun(image);
        return image;
    }

    void normalize(sf::Image& image){
        sf::Vector2u imgSize = image.getSize();
        sf::Uint8 minR = 0xff, maxR = 0;
        sf::Uint8 minG = 0xff, maxG = 0;
        sf::Uint8 minB = 0xff, maxB = 0;
        for(int r = 0; r < imgSize.y; r++){
            for(int c = 0; c < imgSize.x; c++){
                sf::Color pixel_color = image.getPixel(c, r);
                minR = std::min(minR, pixel_color.r);
                minG = std::min(minG, pixel_color.g);
                minB = std::min(minB, pixel_color.b);
                maxR = std::max(maxR, pixel_color.r);
                maxG = std::max(maxG, pixel_color.g);
                maxB = std::max(maxB, pixel_color.b);
            }
        }
        int dr = maxR - minR;
        int dg = maxG - minG;
        int db = maxB - minB;
        for(int r = 0; r < imgSize.y; r++){
            for(int c = 0; c < imgSize.x; c++){
                sf::Color pixel_color = image.getPixel(c, r);
                pixel_color.r = 255 * ((float)pixel_color.r - minR)/dr;
                pixel_color.g = 255 * ((float)pixel_color.g - minG)/dg;
                pixel_color.b = 255 * ((float)pixel_color.b - minB)/db;
                image.setPixel(c, r, pixel_color);
            }
        }
    }

    sf::Image pixelize(const sf::Image& image, uint max_width, uint max_height, const std::string& selector){
        sf::Vector2u origImgSize = image.getSize();
        float ratio = (float)origImgSize.y/origImgSize.x;
        uint width, height;
        if(origImgSize.x > origImgSize.y){
            width = max_width;
            height = width * ratio;
        }else{
            height = max_height;
            width = height / ratio;
        }
        
        RGB (*selectorfun)(const Palette&);        
        if(selector == "avg"){
            selectorfun = [](const Palette& p)->RGB{
                uint sr=0, sg=0, sb=0, sa=0;
                for(auto& c: p){
                    sr += c.r;
                    sg += c.g;
                    sb += c.b;
                    sa += c.a;
                }
                uint n = p.size();
                return RGB(sr/n, sg/n, sb/n, sa/n);
            };
        }else if(selector == "med"){
            selectorfun = [](const Palette& p)->RGB{
                return graySorted(p)[p.size()/2];
            };
        }else if(selector == "min"){
            selectorfun = [](const Palette& p)->RGB{
                return graySorted(p)[0];
            };
        }else if(selector == "max"){
            selectorfun = [](const Palette& p)->RGB{
                return graySorted(p)[p.size()-1];
            };
        }else{
            throw std::runtime_error("Unkown pixel selector: "+selector);
        }
        sf::Image newimg;
        float blockwidth = (float)origImgSize.x / width;
        float blockheight = (float)origImgSize.y / height;
        newimg.create(width, height);
        for(uint j = 0; j <= height; j++){
            for(uint i = 0; i <= width; i++){
                std::vector<RGB> block;
                for(uint bj = 0; bj < blockheight; bj++){
                    uint y = j * blockheight + bj;
                    if(y >= origImgSize.y) break;
                    for(uint bi = 0; bi < blockwidth; bi++){
                        uint x = i * blockwidth + bi;
                        if(x >= origImgSize.x) break;
                        block.push_back(image.getPixel(x,y));
                    }
                }
                if(!block.empty()){
                    newimg.setPixel(i,j,selectorfun(block));
                }
            }
        }
        return newimg;
    }

    sf::Image apply_convolution(sf::Image image, const Matrix& m){
        auto imageSize = image.getSize();
        int half_w = std::floor((float)m.getWidth() / 2.f);
        int half_h = std::floor((float)m.getHeight() / 2.f);
        sf::Image new_img;
        new_img.create(imageSize.x, imageSize.y);
        for(int r = 0; r < imageSize.y; r++){
            for(int c = 0; c < imageSize.x; c++){
                float finalR = 0, finalG = 0, finalB = 0;
                for(int mr = 0; mr < m.getHeight(); mr++){
                    for(int mc = 0; mc < m.getWidth(); mc++){
                        int pixel_r = r - half_h + mr;
                        int pixel_c = c - half_w + mc;
                        if(
                            pixel_r < imageSize.y && pixel_r >= 0 
                            && pixel_c < imageSize.x && pixel_c >= 0
                        ){
                            RGB rgb  = image.getPixel(pixel_c, pixel_r);
                            float factor = m.get(mr, mc);
                            finalR += factor * rgb.r;
                            finalG += factor * rgb.g;
                            finalB += factor * rgb.b;
                        }
                    }
                }
                new_img.setPixel(c, r, RGB(std::abs(finalR), std::abs(finalG), std::abs(finalB)));
            }
        }
        return new_img;
    }

    sf::Image sobel(sf::Image image, float threshold){
        for(int i=0; i<image.getSize().x; i++){
            for(int j=0; j<image.getSize().y; j++){
                image.setPixel(i,j,grayScale(image.getPixel(i,j)));
            }   
        }
        sf::Image edgesY = apply_convolution(image, {3,3,{
            1,  1,  1,
            0,  0,  0,
            -1, -1, -1
        }});

        sf::Image edgesX = apply_convolution(image, {3,3, {
            -1, 0, 1,
            -1, 0, 1,
            -1, 0, 1
        }});
        sf::Image edges;
        edges.create(image.getSize().x, image.getSize().y);
        for(int i=0; i < image.getSize().x; i++){
            for(int j=0; j < image.getSize().y; j++){
                int g = std::sqrt(
                    std::pow(edgesX.getPixel(i, j).r, 2) +
                    std::pow(edgesY.getPixel(i, j).r, 2)
                );
                if(g >= threshold*255){
                    edges.setPixel(i, j, RGB(g,0, 0, 0));
                }else{
                    edges.setPixel(i,j,RGB(0x0));
                }
            }
        }
        return edges;
    }

    Palette extractPaletteSimple(const sf::Image& image, int factor, bool allow_unexistent){
        Palette ret;
        // create subspaces
        std::vector<std::vector<std::vector<Palette>>> subSpaces;
        subSpaces.resize(factor);
        for(int i=0; i < factor; i++){
            subSpaces[i].resize(factor);
            for(int j=0; j < factor; j++){
                subSpaces[i][j].resize(factor);
                
            }
        }
        // fill subspaces
        auto space = [factor](int x){ return std::floor(factor * (float) x / 256); };
        for(int i=0; i < image.getSize().x; i++){
            for(int j=0; j < image.getSize().y; j++){
                RGB pixel = image.getPixel(i,j);
                std::stringstream ss;
                ss << pixel;
                subSpaces.at(space(pixel.r)).at(space(pixel.g)).at(space(pixel.b)).push_back(pixel);
            }
        }
        // average each subspace
        for(int rspace_i = 0; rspace_i < factor; rspace_i++){
            for(int gspace_i = 0; gspace_i < factor; gspace_i++){
                for(int bspace_i = 0; bspace_i < factor; bspace_i++){
                    auto& subspace = subSpaces.at(rspace_i).at(gspace_i).at(bspace_i);
                    if(subspace.size() > 0){
                        RGB avg = average(subspace);
                        if(allow_unexistent){
                            ret.push_back(avg);
                        }else{
                            ret.push_back(closestByColor(subspace, avg)[0]);
                        }
                    }
                }
            }
        }
        return ret;
    }

    std::pair<Palette, Palette> medianCut(const Palette& palette){
        std::pair<Palette, Palette> divided;
        std::vector<std::pair<int, int>> reds;
        std::vector<std::pair<int, int>> greens;
        std::vector<std::pair<int, int>> blues;
        int n = palette.size();
        for(int i=0; i < n; i++){
            RGB col = palette[i];
            reds.emplace_back(col.r, i);
            blues.emplace_back(col.g, i);
            greens.emplace_back(col.b, i);
        }
        std::sort(reds.begin(), reds.end());
        std::sort(greens.begin(), greens.end());
        std::sort(blues.begin(), blues.end());
        int rdelta = reds[n-1].first - reds[0].first;
        int gdelta = greens[n-1].first - greens[0].first;
        int bdelta = blues[n-1].first - blues[0].first;
        std::vector<std::pair<int,int>> * wider;
        if(rdelta >= gdelta && rdelta >= bdelta){
            wider = &reds; 
        }else if(gdelta >= bdelta){
            wider = &greens; 
        }else{
            wider = &blues; 
        }
        int median = wider->at(n/2).first;
        for(int i=0; i < n/2; i++){
            divided.first.push_back(palette[wider->at(i).second]);
        }
        for(int i=n/2; i < n; i++){
            divided.second.push_back(palette[wider->at(i).second]);
        }
        return divided;
    }

    Palette extractPaletteMedianCut(const sf::Image& image, int cuts, bool allow_unexistent){
        std::vector<Palette> subSpaces;
        subSpaces.resize(1);
        for(int i=0; i < image.getSize().x; i++){
            for(int j=0; j < image.getSize().y; j++){
                subSpaces.at(0).push_back(image.getPixel(i,j));
            }
        }
        subSpaces[0] = closestByBrightness(subSpaces.at(0), RGB(0xff));
        auto last = std::unique(subSpaces.at(0).begin(), subSpaces.at(0).end());
        subSpaces.at(0).erase(last, subSpaces.at(0).end());
        for(int cut_i = 0; cut_i < cuts; cut_i++){
            std::vector<Palette> newSubSpaces;
            for(const auto& subSpace: subSpaces){
                auto divided = medianCut(subSpace);
                newSubSpaces.push_back(divided.first);
                newSubSpaces.push_back(divided.second);
            }
            subSpaces = newSubSpaces;
        }
        Palette final;
        for(const auto& subSpace: subSpaces){
            RGB avg = average(subSpace);
            if(allow_unexistent){
                final.push_back(avg);
            }else{
                final.push_back(closestByColor(subSpace,avg)[0]);
            }
        }
        return final;
    }
}
