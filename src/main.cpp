#include <cmath>
#include <fstream>
#include <functional>
#include <iostream>
#include <map>
#include <regex>
#include <string>
#include <sstream>
#include <vector>

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

sf::Image pixelize(const sf::Image& image, uint max_width, uint max_height, const std::string& selector="avg"){
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

void palette_to_file(const Palette& palette, const std::string& path, int rows=1){
    sf::Image image;
    image.create(50*palette.size()/rows, 150*rows);
    for(int r = 0; r < image.getSize().y; r++){
        for(int c = 0; c < image.getSize().x; c++){
            image.setPixel(c,r,palette[(r/150)*(palette.size()/rows)+c/50]);
        }
    }
    image.saveToFile(path);
}

void print_help(){
    std::cout << "Usage: makeitpixel [OPTIONS] FILES..." << std::endl;
    std::cout << "" << std::endl;
    std::cout << "Program to make images look like pixel art." << std::endl;
    std::cout << "" << std::endl;
    std::cout << "OPTIONS" << std::endl;
    std::cout << "  -c, --config-file PATH  Set the configuration file." << std::endl;
    std::cout << "  -h, --help              Print this help message and exit." << std::endl;
    std::cout << "  -o, --output-dir DIR    Set the output directory for the generated images." << std::endl;
    std::cout << "  -p, --palette PATH      Create an image to display the palette." << std::endl;
    std::cout << "  -x, --config CONFIG     Set the CLI configuration as a JSON formatted string." << std::endl;
    exit(0);
}

/*
 * MAIN
 */

int main(int argc, char** argv){
    if(argc == 1){
        print_help();
    }
    // PARSE ARGUMENTS INTO PARAMETERS
    const std::map<std::string, std::string> args_shorts = {
        {"-c", "--config-file"},
        {"-x", "--config"},
        {"-o", "--output-dir"},
        {"-h", "--help"},
        {"-p", "--palette"},
    };
    std::map<std::string, bool> flags = {
    };
    std::map<std::string, std::string> opts = {
        {"--config", "{}"},
        {"--config-file", ""},
        {"--output-dir", "."},
        {"--palette", ""},
    };
    std::vector<std::string> positional;
    std::string last_opt;
    std::string last_real_opt;
    bool expect_positional = true;
    for(int i=1; i < argc; i++){
        std::string arg(argv[i]);
        last_real_opt = arg;
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
        if(arg == "--help"){
            print_help();
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
    // SOME ERROR HANDLING 
    if(!expect_positional){
        log(ERROR, last_real_opt + " expected an option value");
        return -1;
    }
    if(positional.size() == 0 && opts["--palette"] == ""){
        log(ERROR, "No files provided");
        return -1;
    }
    // A BIT OF POSTPROCESSING THE ARGS
    opts["--output-dir"] += sep;

    // DEFAULT CONFIGURATION
    json config = {
        {"normalize", "no"}, // no, pre, post
        {"select_pixel", "avg"}, // avg, med, min, max
        {"width", 64}, // <number>
        {"height", 64}, // <number>
        {"quantization", "none"}, // none, bit<number>, closest_rgb, closest_gray
        {"dithering", 
            {
                {"method", "none"}, // none, floydsteinberg, ordered
                {"matrix", "Bayes4"}, // see Quantization.cpp::matrices
                {"threshold", 0}, // <number>
                {"sparsity", "auto"} // auto, <number>
            }
        },
        {"palette", 
            {
                {"main", "00ff00"}, // <color>
                {"scheme", "analogous"}, // mono, analogous, complementary, triadic, split_complementary, rectangle, square 
                {"spectre", "linear"}, // complete, linear 
                {"inter", 0}, // <number>
                {"disparity", 0.85} // <number>
            } // object or <color> array
        }
    };
    // FILE CONFIGURATION
    json file_config;
    if(opts["--config-file"].size() > 0){
        try{
            std::ifstream config_file(opts["--config-file"]);
            config_file >> file_config;
            config_file.close();
        }catch(const std::exception& ex){
            log(ERROR, ex.what());
            return -1;
        }
        config.merge_patch(file_config);
    }

    // CLI CONFIGURATION
    json cli_config = json::parse(opts["--config"]);
    if(cli_config.size() > 0){
        config.merge_patch(cli_config);
    }
    
    // log(IMPORTANT, "Configuration");
    // log(PLAIN, config.dump(2));

    // PALETTE BUILDING
    log(IMPORTANT, "Palette");
    Palette base_colors;
    Palette palette = {};
    auto str2rgb = [](std::string str)->RGB{
        if(str.length() > 0 && str[0] == '#'){
            str = str.substr(1);
        }
        return RGB((std::stoi(str, nullptr, 16) << 8) | 0xff);
    };
    int palette_rows = 1;
    if(config["palette"].is_array()){
        for(auto& col: config["palette"]){
            palette.push_back(str2rgb(col.get<std::string>()));
        }
    }else{
        RGB main = str2rgb(config["palette"]["main"].get<std::string>());
        float disparity = config["palette"]["disparity"].get<float>();
        int inter = config["palette"]["inter"].get<int>();
        auto make_spectre = [disparity, inter](Palette p) -> Palette {
            RGB darkest = lerp(p[0], RGB(0xff), disparity);
            RGB lightest = lerp(p[p.size()-1], RGB(0xffffffff), disparity);
            Palette half = gradient({darkest}, p, std::floor((float)inter/2));
            Palette whole = gradient(half, {lightest}, std::ceil((float)inter/2));
            return whole;
        };

        if(config["palette"]["scheme"] == "mono"){
            base_colors = {main};
            
        }else if(config["palette"]["scheme"] == "analogous"){
            base_colors = {
                shiftHue(main, 30), 
                main, 
                shiftHue(main, 330),
                shiftHue(main, -30)
            };

        }else if(config["palette"]["scheme"] == "complementary"){
            base_colors = {
                shiftHue(main, 180),
                shiftHue(main, -180),
                main
            };

        }else if(config["palette"]["scheme"] == "triadic"){
            base_colors = {
                shiftHue(main, 120), 
                main, 
                shiftHue(main, -120),
                shiftHue(main, 240)
            };
        
        }else if(config["palette"]["scheme"] == "split_complementary"){
            base_colors = {
                shiftHue(main, 150),
                main,
                shiftHue(main, 210),
                shiftHue(main, -150)
            };
        
        }else if(config["palette"]["scheme"] == "rectangle"){
            base_colors = {
                main, 
                shiftHue(main, 60), 
                shiftHue(main, 180), 
                shiftHue(main, -180), 
                shiftHue(main, -120), 
                shiftHue(main, 240)
            };
        
        }else if(config["palette"]["scheme"] == "square"){
            base_colors = {
                main, 
                shiftHue(main, 90), 
                shiftHue(main, 180), 
                shiftHue(main, -180), 
                shiftHue(main, -90), 
                shiftHue(main, 270)
            };

        }else{
            log(ERROR, "Bad palette.scheme option: " + config["palette"]["scheme"].dump());
            return -1;
        }
        log(INFO, "Scheme: " + config["palette"]["scheme"].get<std::string>());
        if(config["palette"]["spectre"] == "complete"){
            palette_rows = base_colors.size();
            for(auto col: base_colors){
                Palette spectre = make_spectre({col});
                palette = gradient(palette, spectre, 0);
            }
        }else if(config["palette"]["spectre"] == "linear"){
            palette = make_spectre(closestByBrightness(base_colors, RGB(0xff)));
        }else{
            log(ERROR, "Bad palette.spectre option: " + config["palette"]["spectre"].dump());
            return -1;
        }
    }
    

    Palette printable_palette = palette;
    //// https://stackoverflow.com/questions/16476099/remove-duplicate-entries-in-a-c-vector#16476268
    palette = closestByBrightness(palette, RGB(0xff));
    auto last = std::unique(palette.begin(), palette.end());
    palette.erase(last, palette.end());
    last = std::unique(printable_palette.begin(), printable_palette.end());
    printable_palette.erase(last, printable_palette.end());

    
    for(auto& c: palette){
        std::stringstream ss;
        ss << c;
        log(PLAIN, "#" + ss.str());
    }

    if(opts["--palette"] != ""){
        log(INFO, "Displaying palette");
        palette_to_file(printable_palette, opts["--palette"], palette_rows);
        log(SUCCESS, "Palette displayed in " + opts["--palette"]);
    }

    // BUILD COLOR SELECTION STRATEGY
    double sparsity = 0;
    std::function<RGB(const RGB&)> quantizer = [&](const RGB& rgb) -> RGB { return rgb; };
    if(config["quantization"].is_string() && config["quantization"].get<std::string>().substr(0, 3) == "bit"){
        int bit_num = std::stoi(config["quantization"].get<std::string>().substr(3));
        int values_per_channel = std::pow(2, bit_num);
        double factor = 255.0 / (values_per_channel-1);
        const auto qchannel = [factor](int x) -> int { 
            double r =  factor * std::round((double)x / factor); 
            return r;
        };
        quantizer = [qchannel](const RGB &rgb) -> RGB {
            return RGB(
                qchannel(rgb.r),
                qchannel(rgb.g),
                qchannel(rgb.b),
                rgb.a
            );
        };
        sparsity = factor;
    }else if(config["quantization"] == "closest_rgb"){
        quantizer = [palette](const RGB &rgb) -> RGB {
            return closestByColor(palette, rgb)[0];
        };
        sparsity = 255.0 / palette.size();
    }else if(config["quantization"] == "closest_gray"){
        quantizer = [palette](const RGB &rgb) -> RGB {
            return closestByBrightness(palette, rgb)[0];
        };
        sparsity = 255.0 / palette.size();
    }else if(config["quantization"] != "none"){
        log(ERROR, "Bad quantization option: " + config["quantization"].dump());
        return -1;
    }
    //// Parameters for ordered dithering
    auto matrix_it = matrices.find(config["dithering"]["matrix"]);
       
    if(matrix_it == matrices.end()){
        log(ERROR, "Bad matrix option: " + config["dithering"]["matrix"].dump());
        return -1;
    }
    
    if(config["dithering"]["sparsity"].is_number()){
        sparsity = config["dithering"]["sparsity"].get<float>();
    
    }else if(config["dithering"]["sparsity"] == "auto"){
        log(INFO, "Auto sparsity: " + std::to_string(sparsity));
    
    }else{
        log(ERROR, "Bad sparsity option: " + config["dithering"]["sparsity"].dump());
        return -1;
    }

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
        
        //// Normalization
        if(config["normalize"] == "pre"){
            normalize(img);
        }else if(config["normalize"] != "post" && config["normalize"] != "no"){
            log(ERROR, "Bad normalize option: " + config["normalize"].dump());
            return -1;
        }

        //// Scaling
        out = pixelize(
            img, 
            config["width"].get<uint>(), 
            config["height"].get<uint>(), 
            config["select_pixel"].get<std::string>()
        );
        

        //// Normalization
        if(config["normalize"] == "post"){
            normalize(out);
        }
        // Quantization and dithering
        if(config["dithering"]["method"] == "floydsteinberg"){

            ditherFloydSteinberg(out, quantizer);

        }else if(config["dithering"]["method"] == "ordered"){

            ditherOrdered(out, quantizer, matrix_it->second, sparsity);
            
        }else if(config["dithering"]["method"] == "none"){

            directQuantize(out, quantizer);
        
        }else{
        
            log(ERROR, "Bad dithering method option: " + config["dithering"]["method"].dump());
            return -1;
        
        }

        
        // SAVE IT
        log(INFO, "Saving...", "");
        out.saveToFile(opts["--output-dir"] + name);
        log(SUCCESS, "Done");
    }
    return 0;
}