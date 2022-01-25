%{

    #include <cstdlib>
    #include <sstream>
    #include "ProgramState.hpp"
    #include "Value.hpp"
    #include "parser.hpp"
    std::stringstream token;
    using namespace mipa;

%}
%option always-interactive
%option noyywrap
%option nounput
%x STRINGSTATE
%%
<STRINGSTATE>{
    \\b     {token<<'\b';}
    \\f     {token<<'\f';}
    \\n     {token<<'\n';}
    \\r     {token<<'\r';}
    \\t     {token<<'\t';}
    \\\"    {token<<'\"';}
    \\\\    {token<<'\\';}
    \\\'    {token<<'\'';}
    \"      {
                yylval.innervalue = new StringValue(token.str());
                token.str("");
                BEGIN(INITIAL);
                return VALUE;
            }
    .       {token << yytext;}
}

\"  { BEGIN(STRINGSTATE); }

-?[0-9]+(\.[0-9]+)? { 
    yylval.innervalue = new NumberValue(std::atof(yytext));
    return VALUE;
    }

#[0-9a-zA-Z]+ { 
      std::stringstream ss(&yytext[1]);
      RGB rgb;
      ss >> rgb;
      yylval.innervalue = new ColorValue(rgb); 
      return VALUE; 
    }

[a-zA-Z][_a-zA-Z0-9]* {strcpy(yylval.string,yytext); return VARIABLE; }

[()\n;=,.]   { return yytext[0]; }

"//".* {}
"/*"((\n|[^*])*(\*[^/])?)*"*/" {}
[ \t]+ {}

.   { return YYUNDEF; }

%%

void set_input_string(const char* in) {
  yy_scan_string(in);
}

void end_lexical_scan(void) {
  yy_delete_buffer(YY_CURRENT_BUFFER);
}