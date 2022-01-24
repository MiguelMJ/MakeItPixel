%code requires{
  #include <iostream>
  #include <stack>
  #include <stdexcept>
  #include <cstring>
  #include "Value.hpp"
  #include "ProgramState.hpp"
  extern int yylex();
  extern FILE* yyin;
  void yyerror(const char* msg);
  void prompt(bool force=false);
  #include "BuiltInFunctions.hpp"
}
%union {
    char string[32];
    mipa::Value *innervalue;
    std::stack<mipa::Value*> *stack;
}
%token IN
%token <innervalue> VALUE
%token <string> VARIABLE

%type <stack> argstack
%type <innervalue> statement function_call value


%%

program : statement { mipa::ProgramState::gb_constants(); } 
        | statement sep { mipa::ProgramState::gb_constants(); } program {}
        | YYEOF { mipa::ProgramState::finished = true; }
        ;

sep : ';' | '\n' {prompt();} ;

statement : VARIABLE '=' value {
                mipa::ProgramState::set($1, $3);
                $$ = nullptr;
              }
          | function_call {$$ = $1;}
          | {$$=nullptr;}
          ;

function_call : VARIABLE '(' argstack ')' {
        auto it = mipa::BuiltInFunctions.find($1);
        if(it == mipa::BuiltInFunctions.end()) throw std::runtime_error("Undefined function: "+std::string($1));
        it->second(*$3);
        delete $3;
      } 
              | '.' VARIABLE { 
                            auto it = mipa::BuiltInFunctions.find($2);
                            if(it == mipa::BuiltInFunctions.end()) throw std::runtime_error("Undefined function: "+std::string($2));
                            it->second({});
                          }
              ;


argstack : value ',' argstack {$$ = $3; $3->push($1); }
        | value {$$ = new std::stack<mipa::Value*>; $$->push($1); }
        | { $$ = new std::stack<mipa::Value*>; }
        ;

value : VARIABLE { 
          $$ = mipa::ProgramState::get($1);
        }
      | VALUE { 
            $$ = $1; 
            mipa::ProgramState::setConstant($1);
          }
      ;

%%
void yyerror(const char* msg){
    throw std::runtime_error("parser: "+std::string(msg));
}
void prompt(bool force){
  if (force || yyin == stdin){
    std::cout << "> ";
  }
}