#include <cstdlib> // getenv
#include <iostream>
#include <atomic>
#include <mutex>
#include <thread>
#include <functional> // std::ref

#include <SFML/Graphics.hpp>

#include "parser.hpp"
#include "readline/readline.h"
#include "readline/history.h"

extern int yylex_destroy();
using namespace std;
using namespace mipa;


// for readline
// https://eli.thegreenplace.net/2016/basics-of-using-the-readline-library/
std::vector<std::string> vocabulary;
void update_vocabulary(){
    vocabulary.clear();
    for(auto& pair: ProgramState::symbolTable){
        vocabulary.push_back(pair.first);
    }
    for(auto& pair: matrices){
        vocabulary.push_back('"'+pair.first+'"');
    }
    for(auto& pair: BuiltInFunctions){
        if(pair.first.size() < 8 || pair.first.substr(pair.first.size() - 8) != "Operator"){
            vocabulary.push_back("."+pair.first);
            vocabulary.push_back(pair.first+"(");
        }
    }
}
char* completion_generator(const char* text, int state) {
  static std::vector<std::string> matches;
  static size_t match_index = 0;

  if (state == 0) {
    matches.clear();
    match_index = 0;

    std::string textstr = std::string(text);
    for (auto word : vocabulary) {
      if (word.size() >= textstr.size() &&
          word.compare(0, textstr.size(), textstr) == 0) {
        matches.push_back(word);
      }
    }
  }

  if (match_index >= matches.size()) {
    return nullptr;
  } else {
    return strdup(matches[match_index++].c_str());
  }
}
char** completer(const char* text, int /* start */, int end) {
    if(end == 0){
        rl_attempted_completion_over = 1; // don't complete filenames in blank lines
        return nullptr;
    }
    return rl_completion_matches(text, completion_generator);
}

struct window{
    private:
    sf::RenderWindow rwindow;
    sf::Texture texture;
    std::mutex mutex;
    bool m_available=true;
    public:
    std::atomic<bool> should_close;
    window(){
        texture.create(100,100);
        should_close.store(false);
    }
    bool check_open(){
        mutex.lock();
        bool ret = rwindow.isOpen();
        mutex.unlock();
        return ret;
    }
    void close(){
        mutex.lock();
        if(rwindow.isOpen()) rwindow.close();
        mutex.unlock();
    }
    void adjustView(const sf::Vector2u& size){
        sf::View view({(float)size.x/2, (float)size.y/2}, {(float)size.x, (float)size.y});
        rwindow.setView(view);
    }
    void open(const sf::Image& img){
        mutex.lock();
        auto imgSize = img.getSize();
        texture.create(imgSize.x, imgSize.y);
        texture.update(img);
        rwindow.create(sf::VideoMode(imgSize.x,imgSize.y), std::string("Make It Pixel"), sf::Style::Close);
        adjustView(imgSize);
        rwindow.setFramerateLimit(30);
        mutex.unlock();
    }
    void refresh(const sf::Image& img){
        std::cout << "..." << std::endl;
        mutex.lock();
        std::cout << "refreshing" << std::endl;
        auto imgSize = img.getSize();
        if(imgSize.x > texture.getSize().x || imgSize.y > texture.getSize().y){
            texture.create(imgSize.x, imgSize.y);
        }
        texture.update(img);
        rwindow.setSize(imgSize);
        adjustView(imgSize);
        std::cout << "done" << std::endl;
        mutex.unlock();
    }
    void process(){
        mutex.lock();
        sf::Event event;
        while(rwindow.pollEvent(event)){
            switch(event.type){
                case sf::Event::Closed:
                    rwindow.close();
                    break;
                default:
                break;
            }
        }
        rwindow.clear();
        rwindow.draw(sf::Sprite(texture));
        rwindow.display();
        mutex.unlock();
    }
    ~window(){
        // if(rwindow.isOpen()) rwindow.close();
    }
    void available(bool flag){
        if (flag == m_available) return;
        if(flag){
            m_available = true;
            mutex.unlock();
        }else{
            mutex.lock();
            m_available = false;
        }
    }
    bool available() const{
        return m_available;
    }
};
void manage_window(window& w){
    while(!w.should_close.load()){
        while(w.check_open()){
            w.process();
            std::this_thread::yield();
        };
        std::this_thread::yield();
    }
}

int main(){
    std::string historyfilestr=getenv("HOME");
    historyfilestr.append("/.mip_history");
    const char* historyfile=historyfilestr.c_str();
    std::cout << historyfile << std::endl;
    using_history();
    rl_attempted_completion_function = completer;
    read_history(historyfile);

    std::cout << "▙▗▌   ▌      ▜▘▐   ▛▀▖▗       ▜  ▐" << std::endl;
    std::cout << "▌▘▌▝▀▖▌▗▘▞▀▖ ▐ ▜▀  ▙▄▘▄ ▚▗▘▞▀▖▐  ▐" << std::endl;
    std::cout << "▌ ▌▞▀▌▛▚ ▛▀  ▐ ▐ ▖ ▌  ▐ ▗▚ ▛▀ ▐  ▐" << std::endl;
    std::cout << "▘ ▘▝▀▘▘ ▘▝▀▘ ▀▘ ▀  ▘  ▀▘▘ ▘▝▀▘ ▘ ▗" << std::endl;
    std::cout << "------------------------------------------" << std::endl;

    window win;
    win.available(false);
    // https://stackoverflow.com/questions/64734787/c-error-in-creating-threads-static-assertion-failed-stdthread-arguments-mu
    std::thread window_thread(manage_window, std::ref(win));

    while(!mipa::ProgramState::finished){    
        try{
            // prompt(true);
            char* in;
            std::thread gb_thread(ProgramState::gb);
            update_vocabulary();
            in = readline(">> ");
            gb_thread.join();
            if(in == nullptr){ 
                std::cout << "Fin input" << std::endl;
                mipa::ProgramState::finished = true;
                free(in);
                continue;
            }
            if(*in){
                add_history(in);
                set_input_string(in);
                yyparse();
                end_lexical_scan();
            }
            
            free(in);

            if(mipa::ProgramState::should_refresh){
                win.available(true);
                if(!win.check_open()){
                    win.open(*mipa::ProgramState::for_display);
                }else{
                    win.refresh(*mipa::ProgramState::for_display);
                }
                mipa::ProgramState::should_refresh = false;
            }else if(win.available() && !win.check_open()){
                win.available(false);
            }
        }catch(const std::runtime_error& err){
            std::cerr << "Error: " << err.what() << std::endl;
            // yylex_destroy();
        }
    }
    win.available(true);
    win.should_close.store(true);
    win.close();
    write_history(historyfile);
    history_truncate_file(historyfile, 100);
    window_thread.join();
    return 0;
}