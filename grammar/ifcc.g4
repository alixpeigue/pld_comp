grammar ifcc;

axiom : prog EOF ;

prog : 'int' 'main' '(' ')' '{' statement* '}' ;

statement : declaration ';' | expression ';' | return_stmt | scope | if_stmt ;

scope : '{' statement* '}' ;

if_stmt : IF '(' expression ')' then_stmt = statement (ELSE else_stmt = statement)? ;

declaration: 'int' declaAffect (',' declaAffect)*;

declaAffect: VARIABLE '=' expression | 
             VARIABLE;

return_stmt: RETURN expression ';' ;

expression: INT_CONST # intConst | 
            CHAR_CONST #charConst | 
            VARIABLE # variable |
            '(' expression ')' #paren |
            op = ('-' | '+' | '!') expression # unaryAdd |
            expression op = ('*' | '/' | '%') expression # mult |
            expression op = ('+' | '-') expression # add |
            VARIABLE '=' expression  # affect;

IF : 'if' ;
ELSE : 'else' ;
RETURN : 'return' ;
INT_CONST : [-]?[0-9]+ ;
CHAR_CONST : '\'' . '\'';
VARIABLE : [a-zA-Z_][a-zA-Z0-9_]* ;
COMMENT : '/*' .*? '*/' -> skip ;
DIRECTIVE : '#' .*? '\n' -> skip ;
WS    : [ \t\r\n] -> channel(HIDDEN);
