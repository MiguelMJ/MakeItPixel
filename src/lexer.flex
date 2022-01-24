%{

    #include <cstdlib>
    #include <sstream>
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
                yylval.value = new StringValue(token.str());
                token.str("");
                BEGIN(INITIAL);
                return VALUE;
            }
    .       {token << yytext;}
}

\"  { BEGIN(STRINGSTATE); }
[0-9]+(\.[0-9]+)? { yylval.value = new NumberValue(std::atof(yytext)); return VALUE; }
[()\n;=,]   { return yytext[0]; }

%%