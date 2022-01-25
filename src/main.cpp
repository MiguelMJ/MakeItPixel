#include<iostream>

#include "parser.hpp"
#include "readline/readline.h"
#include "readline/history.h"

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
    return 0;
}