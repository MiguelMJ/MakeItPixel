```

PROGRAM :- STATEMENT | STATEMENT ';' PROGRAM ;

STATEMENT :- ASSIGNATION
           | display VALUE{palette} in filename 
           | save VALUE{image|palette} in filename
           | push VARIABLE{image}
           | pull VARIABLE{image}
           | pixeled VARIABLE{image} NUM PIXELSELECTOR QUANTIZER
           ;

VALUE :- VARIABLE | COLOR | PALETTE | IMAGE | QUANTIZER | COLORER | PIXELSELECTOR;

PIXELSELECTOR :- median | average | minimum | maximum ;

QUANTIZER :- direct
           | ordered(MATRIX, NUM) // sparsity
           | ordered(MATRIX, NUM, NUM) // sparsity and threshold
           | error_propagation()
           | error_propagation(NUM) // threshold
           ;

MATRIX :- bayes2
        | bayes4
        | bayes8
        | horizontal
        | vertical
        ;

ASSIGNATION :- VARIABLE = VALUE ;
 

COLOR :- rgb(NUM, NUM, NUM) 
       | rgba(NUM, NUM, NUM, NUM)
       | hsv(NUM, NUM, NUM) 
       | hsva(NUM, NUM, NUM, NUM)
       | #HEX
       | COLOR >> NUM // shift hue left
       | COLOR << NUM // shift hue right
       | COLOR <> COLOR NUM % // lerped
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