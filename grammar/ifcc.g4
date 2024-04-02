grammar ifcc;

axiom : prog EOF ;

prog : function* ;

statement : declaration ';' | expression ';' | return_stmt | scope | if_stmt | loop | break_stmt | continue_stmt | switch_stmt ;

function : type VARIABLE '(' (type VARIABLE)? ( ',' type VARIABLE)* ')' '{' statement* '}' ;

scope : '{' statement* '}' ;

if_stmt : IF '(' expression ')' then_stmt = statement (ELSE else_stmt = statement)? ;

loop : do_while_stmt | while_stmt ;

do_while_stmt : DO then_stmt = statement WHILE '(' expression ')' ';' ;

while_stmt : WHILE '(' expression ')' then_stmt = statement ;

switch_stmt : SWITCH '(' expression ')' '{' case_stmt* default_case_stmt? '}' ;

case_stmt : CASE test_expr = (INT_CONST | CHAR_CONST) ':' statement* ;

default_case_stmt : DEFAULT ':' statement* ;

break_stmt : BREAK ';' ;

continue_stmt : CONTINUE ';' ;

declaration: type declaAffect (',' declaAffect)*;

declaAffect: VARIABLE '=' expression | 
             VARIABLE;

return_stmt: RETURN expression? ';' ;

expression: INT_CONST # intConst | 
            CHAR_CONST #charConst | 
            VARIABLE # variable |
            '(' expression ')' #paren |
            '++' VARIABLE #preInc |
            '--' VARIABLE #preDec |
            op = ('-' | '+' | '!') expression # unaryAdd |
            expression op = ('*' | '/' | '%') expression # mult |
            expression op = ('+' | '-') expression # add |
            expression op = ('<<' | '>>') expression #shift |
            expression op = ('<' | '>' | '<=' | '>=') expression #compare |
            expression op = ('==' | '!=') expression #compareEq |
            expression '&' expression # andBin |
            expression '^' expression # xorBin |
            expression '|' expression # orBin |
            expression '&&' expression #and |
            expression '||' expression #or |
            VARIABLE op = ('=' | '+=' | '-=' | '*=' | '/=' | '&=' | '^=' | '|=') expression  # affect |
            VARIABLE '++' #postInc |
            VARIABLE '--' #postDec |
            VARIABLE '(' expression? (',' expression)* ')' # func_call ;

type : 'int' | 'void' | 'char' ;

IF : 'if' ;
ELSE : 'else' ;
DO : 'do' ;
WHILE : 'while' ;
SWITCH : 'switch' ;
CASE : 'case' ;
DEFAULT : 'default' ;
RETURN : 'return' ;
BREAK : 'break' ;
CONTINUE : 'continue' ;
INT_CONST : [0-9]+ ;
CHAR_CONST : '\'' . '\'';
VARIABLE : [a-zA-Z_][a-zA-Z0-9_]* ;
COMMENT : '/*' .*? '*/' -> skip ;
DIRECTIVE : '#' .*? '\n' -> skip ;
WS    : [ \t\r\n] -> channel(HIDDEN);
