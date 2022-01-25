#include<iostream>

#include "parser.hpp"

extern int yylex_destroy();
using namespace std;
using namespace mipa;

int main(){

    std::cout << "▙▗▌   ▌      ▜▘▐   ▛▀▖▗       ▜  ▞▀▖   ▐   ▐" << std::endl;
    std::cout << "▌▘▌▝▀▖▌▗▘▞▀▖ ▐ ▜▀  ▙▄▘▄ ▚▗▘▞▀▖▐  ▙▄▌▙▀▖▜▀  ▐" << std::endl;
    std::cout << "▌ ▌▞▀▌▛▚ ▛▀  ▐ ▐ ▖ ▌  ▐ ▗▚ ▛▀ ▐  ▌ ▌▌  ▐ ▖ ▐" << std::endl;
    std::cout << "▘ ▘▝▀▘▘ ▘▝▀▘ ▀▘ ▀  ▘  ▀▘▘ ▘▝▀▘ ▘ ▘ ▘▘   ▀  ▗" << std::endl;
    std::cout << "------------------------------------------" << std::endl;
    
    while(!mipa::ProgramState::finished){    
        try{
            prompt(true);
            yyparse();
        }catch(const std::runtime_error& err){
            std::cerr << "Error: " << err.what() << std::endl;
            yylex_destroy();
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

    return 0;
}