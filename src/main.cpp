#include <map>
#include <cmath>
#include <regex>
#include <vector>
#include <string>
#include <iostream>
#include <fstream>

#include <SFML/Graphics.hpp>

#include "json.hpp"
#include "Color.hpp"
#include "Palette.hpp"
#include "Quantization.hpp"

#ifdef _WIN32
const std::string sep("\\");
#else
const std::string sep("/");
#endif

using namespace mipa;
using json = nlohmann::json;


/*
 * LOG FUNCTIONS
 */
typedef enum {PLAIN, INFO, WARNING, ERROR, IMPORTANT, SUCCESS}  LogType;
void log(LogType level, std::string msg_pre, std::string end="\n"){
    std::cerr << "\r\x1b[2K";
    switch(level){
        case LogType::INFO:
         std::cerr << "- ";
        break;
        case LogType::WARNING:
         std::cerr << "\x1b[35mW ";
        break;
        case LogType::ERROR:
         std::cerr << "\x1b[1;31m\u2716 ";
        break;
        case LogType::IMPORTANT:
         std::cerr << "\x1b[1m> ";
        break;
        case LogType::SUCCESS:
         std::cerr << "\x1b[32m\u2714 ";
        break;
    }
    std::cerr << msg_pre << "\x1b[0m" << end;
}

/*
 * IMAGE PROCESSING FUNCTIONS
 */

void pixelize(sf::Image& newimg, const sf::Image& image, uint max_width, uint max_height, const std::string& selector="avg"){
    log(INFO, "Pixelizing...", "");
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
}

void normalize(sf::Image& image){
    log(INFO, "Normalizing...", "");
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

/*
 * MAIN
 */

int main(int argc, char** argv){
    // PARSE ARGUMENTS INTO PARAMETERS
    const std::map<std::string, std::string> args_shorts = {
        {"-c", "--config-file"},
        {"-x", "--config"},
        {"-o", "--output-dir"}
    };
    std::map<std::string, bool> flags = {
    };
    std::map<std::string, std::string> opts = {
        {"--config", "{}"},
        {"--config-file", "config.json"},
        {"--output-dir", "."}
    };
    std::vector<std::string> positional;
    std::string last_opt;
    bool expect_positional = true;
    for(int i=1; i < argc; i++){
        std::string arg(argv[i]);
        auto it = args_shorts.find(arg);
        if(it != args_shorts.end()) arg = it->second;
        if(flags.find(arg) != flags.end()){
            flags[arg] = true;
            continue;
        }
        if(opts.find(arg) != opts.end()){
            last_opt = arg;
            expect_positional = false;
            continue;
        }
        if(arg[0] == '-'){
            log(ERROR, "Unknown option: "+arg);
            continue;
        }
        if(expect_positional){
            positional.push_back(arg);
        }else{
            opts[last_opt] = arg;
            expect_positional = true;
        }
    }
    // A BIT OF POSTPROCESSING THE ARGS
    opts["--output-dir"] += sep;

    // DEFAULT CONFIGURATION
    json config = {
        {"normalize", "no"},
        {"width", 64},
        {"height", 64},
        {"dithering", 
            {
                {"method", "none"},
                {"threshold", 0}
            }
        },
        {"quantizer", "direct"}
    };
    // FILE CONFIGURATION
    json file_config;
    try{
        std::ifstream config_file(opts["--config-file"]);
        config_file >> file_config;
        config_file.close();
    }catch(const std::exception& ex){
        log(ERROR, ex.what());
        return -1;
    }

    // CLI CONFIGURATION
    json cli_config = json::parse(opts["--config"]);
    
    // FINAL CONFIGURATION
    config.merge_patch(file_config);
    config.merge_patch(cli_config);
    
    log(IMPORTANT, "Configuration");
    log(PLAIN, config.dump(2));

    // START FILE PROCESSING
    std::regex parent_dir_re (".*/");
    for(const std::string& file: positional){
        // LOAD FILE
        std::string name = std::regex_replace(file, parent_dir_re, "");
        sf::Image img, out;
        log(IMPORTANT, name);
        log(INFO, "Loading image...", "");
        if(img.loadFromFile(file)){
            log(INFO, "Loaded", "");
        }else{
            log(ERROR, "Couldn't load "+file);
            continue;
        }
        // PROCESS IMAGE
        if(config["normalize"] == "pre"){
            normalize(img);
        }else if(config["normalize"] != "pos" && config["normalize"] != "no"){
            log(ERROR, "Bad normalize option: " + config["normalize"].get<std::string>());
            continue;
        }

        pixelize(
            out, 
            img, 
            config["width"].get<uint>(), 
            config["height"].get<uint>(), 
            config["select_pixel"].get<std::string>()
        );
        
        std::string dithering_method = config["dithering"]["method"].get<std::string>();
        

        if(config["normalize"] == "pos"){
            normalize(out);
        }
        
        // SAVE IT
        log(INFO, "Saving...", "");
        out.saveToFile(opts["--output-dir"] + name);
        log(SUCCESS, "Done");
    }
    return 0;
}