#include <iostream>
#include <atomic>
#include <thread>
#include <functional> // std::ref

#include <SFML/Graphics.hpp>

#include "parser.hpp"
#include "readline/readline.h"
#include "readline/history.h"

extern int yylex_destroy();
using namespace std;
using namespace mipa;

struct window{
    private:
    sf::RenderWindow rwindow;
    public:
    std::atomic<sf::Texture*> texture;
    std::atomic<bool> close;
    window(){
        close.store(false);
        texture.store(new sf::Texture);
    }
    ~window(){
        delete texture.load();
        texture.store(nullptr);
        rwindow.close();
    }
    friend void manage_window(window&);
};
void manage_window(window& w){
    w.rwindow.create(sf::VideoMode(400,300), std::string("Make It Pixel"));
    w.rwindow.setFramerateLimit(30);
    while(!w.close.load() && w.rwindow.isOpen()){
        sf::Event event;
        while(w.rwindow.pollEvent(event)){
            switch(event.type){
                case sf::Event::Closed:
                    w.rwindow.close();
                    break;
                default:
                break;
            }
        }
        w.rwindow.clear();
        w.rwindow.display();
    };
}

int main(){

    std::cout << "▙▗▌   ▌      ▜▘▐   ▛▀▖▗       ▜  ▐" << std::endl;
    std::cout << "▌▘▌▝▀▖▌▗▘▞▀▖ ▐ ▜▀  ▙▄▘▄ ▚▗▘▞▀▖▐  ▐" << std::endl;
    std::cout << "▌ ▌▞▀▌▛▚ ▛▀  ▐ ▐ ▖ ▌  ▐ ▗▚ ▛▀ ▐  ▐" << std::endl;
    std::cout << "▘ ▘▝▀▘▘ ▘▝▀▘ ▀▘ ▀  ▘  ▀▘▘ ▘▝▀▘ ▘ ▗" << std::endl;
    std::cout << "------------------------------------------" << std::endl;

    window win;
    // https://stackoverflow.com/questions/64734787/c-error-in-creating-threads-static-assertion-failed-stdthread-arguments-mu
    std::thread window_thread(manage_window, std::ref(win));

    while(!mipa::ProgramState::finished){    
        try{
            // prompt(true);
            char* in;
            in = readline("> ");
            if(in == nullptr) break;
            if(*in) add_history(in);
            
            set_input_string(in);
            yyparse();
            end_lexical_scan();
            
            free(in);
        }catch(const std::runtime_error& err){
            std::cerr << "Error: " << err.what() << std::endl;
            // yylex_destroy();
        }catch(int signal){ // signals are not errors
            switch(signal){
                case 0:
                    // exit signal
                    break;
                default:
                    std::cerr << "Internal error: Unknown signal: " << signal << std::endl;
            }
        }
    }
    std::cout << "Bye" << std::endl;
    win.close.store(true);
    window_thread.join();
    return 0;
}