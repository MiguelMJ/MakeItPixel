%code requires{
  #include <stack>
  #include <stdexcept>
  #include "Value.hpp"
  extern int yylex();
  void yyerror(const char* msg);
}
%{
%}
%union {
    char* string;
    mipa::Value *value;
    std::stack<mipa::Value*> *stack;
}
%token IN
%token <value> VALUE
%token <string> VARIABLE COMMAND

%type <stack> argstack
%type <value> statement function_call value

%%

program : statement {} 
        | statement sep program {} 
        ;

sep : ';' | '\n' ;

statement : VARIABLE '=' value {}
          | function_call {}
          ;

function_call : COMMAND '(' argstack ')' {} ;

argstack : value ',' argstack {$$ = $3; $3->push($1); }
        | value {$$ = new std::stack<mipa::Value*>; $$->push($1); }
        | { $$ = new std::stack<mipa::Value*>; }
        ;

value : VARIABLE { $$ = nullptr; }
      | VALUE { $$ = nullptr; }
      ;

%%
void yyerror(const char* msg){
    throw std::runtime_error("parser: "+std::string(msg));
}