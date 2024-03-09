grammar ifcc;

axiom : prog EOF ;

prog : 'int' 'main' '(' ')' '{' (expression ';' | declaration ';' )* return_stmt '}' ;

declaration: 'int' declaAffect (',' declaAffect)*;

declaAffect: VARIABLE '=' expression | 
             VARIABLE;

return_stmt: RETURN expression ';' ;

expression: INT_CONST # intConst | 
            CHAR_CONST #charConst | 
            VARIABLE # variable |
            '(' expression ')' #paren |
            expression op = ('*' | '/' | '%') expression # mult |
            expression op = ('+' | '-') expression # add |
            op = ('-' | '+') expression # unaryAdd |
            VARIABLE '=' expression  # affect;

RETURN : 'return' ;
INT_CONST : [-]?[0-9]+ ;
CHAR_CONST : '\'' . '\'';
VARIABLE : [a-zA-Z_][a-zA-Z0-9_]* ;
COMMENT : '/*' .*? '*/' -> skip ;
DIRECTIVE : '#' .*? '\n' -> skip ;
WS    : [ \t\r\n] -> channel(HIDDEN);