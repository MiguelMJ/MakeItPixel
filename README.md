```

PROGRAM :- STATEMENT | STATEMENT ';' PROGRAM ;

STATEMENT :- ASSIGNATION
           | display VARIABLE{palette} in filename 
           | save VARIABLE{image|palette} in filename
           | push VARIABLE{image}
           | pull VARIABLE{image}
           | quant VARIABLE{image} VALUE{palette} in filename
           ;

ASSIGNATION :- VARIABLE = VALUE ;

VALUE :- VARIABLE | COLOR | PALETTE | IMAGE;

COLOR :- rgb(NUM, NUM, NUM) 
       | rgba(NUM, NUM, NUM, NUM)
       | hsv(NUM, NUM, NUM) 
       | hsva(NUM, NUM, NUM, NUM)
       | #HEX
       | ( COLOR, NUM )> // shift hue left
       | <( COLOR, NUM ) // shift hue right
       | COLOR <NUM>(COLOR) // lerped
       | COLOR ^^ NUM // lightened
       | COLOR .. NUM // darkened
       | COLOR ! // full saturation
       | COLOR ? // grayscale
       | PALETTE [ NUM ]
       ;

PALETTE :- [ PALETTE_CONTENTS ]
         | [[ filename ]]
         ;

PALETTE_CONTENTS :- PALETTE_ELEMENT, PALETTE_CONTENTS
                  | PALETTE_CONTENTS
                  ;

PALETTE_ELEMENT :- VALUE
                 | VALUE =<NUM>=>( PALETTE_ELEMENT ) // gradient of NUM+2 size
                 ;

IMAGE :- load(filename) ;
```