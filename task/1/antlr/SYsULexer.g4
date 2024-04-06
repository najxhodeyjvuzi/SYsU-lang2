lexer grammar SYsULexer;

Int : 'int';
Return : 'return';

Void : 'void';

If : 'if';
Else : 'else';

Exclaim : '!';
Ampamp : '&&';
Pipepipe : '||';

Less : '<';
Greater : '>';
Equalequal : '==';
Lessequal : '<=';
Greaterequal : '>=';
Exclaimequal : '!=';

While : 'while';
Break : 'break';
Continue : 'continue';

LeftParen : '(';
RightParen : ')';
LeftBracket : '[';
RightBracket : ']';
LeftBrace : '{';
RightBrace : '}';
Const : 'const';
Plus : '+';
Minus : '-';
Star : '*';
Slash : '/';
Percent : '%';

Semi : ';';
Comma : ',';

Equal : '=';

Identifier
    :   IdentifierNondigit
        (   IdentifierNondigit
        |   Digit
        )*
    ;

fragment
IdentifierNondigit
    :   Nondigit
    ;

fragment
Nondigit
    :   [a-zA-Z_]
    ;

fragment
Digit
    :   [0-9]
    ;

Constant
    :   IntegerConstant
    ;

fragment
IntegerConstant
    :   DecimalConstant
    |   OctalConstant
    |   Hexadecimal
    ;

fragment
DecimalConstant
    :   NonzeroDigit Digit*
    ;

fragment
OctalConstant
    :   '0' OctalDigit*
    ;

fragment
Hexadecimal
    :   HexaFlag HexaDigit*
    ;

fragment
NonzeroDigit
    :   [1-9]
    ;

fragment
OctalDigit
    :   [0-7]
    ;

fragment
HexaFlag
    :   '0x'
    ;

fragment
HexaDigit
    :   [0-9a-f]
    ;

// 预处理信息处理，可以从预处理信息中获得文件名以及行号
// 预处理信息前面的数组即行号
LineAfterPreprocessing
    :   '#' Whitespace* ~[\r\n]*
    ;

Whitespace
    :   [ \t]+
    ;

// 换行符号，可以利用这个信息来更新行号
Newline
    :   (   '\r' '\n'?
        |   '\n'
        )
    ;

