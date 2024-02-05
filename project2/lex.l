%{
    #include "tree.h"
    #include "syntax.tab.h"
    int isError = 0;
    int yycolumn = 1;
    #define YY_USER_ACTION \
        yylloc.first_line = yylineno; \
        yylloc.last_line = yylineno; \
        yylloc.first_column = yycolumn; \
        yylloc.last_column = yycolumn + yyleng;\
        yycolumn += yyleng;
%}
%option noyywrap
%option yylineno

letters [A-Za-z]
letters_ [A-Za-z_]
digit [0-9]
digit_16 [0-9a-fA-F]
UINT_10 (0|([1-9]{digit}*))
UINT_16 0[xX](0|[1-9a-fA-F]{digit_16}*)
UFLOAT {UINT_10}\.{digit}+
CHAR '.'
CHAR_16 '\\x(0|[1-9a-fA-F]{digit_16})'
ID {letters_}({letters_}|{digit}){0,31}
FAKEID ([0-9]+{ID})
annotation ("//".*|"/*"[^*]*"*"+([^*/][^*]*"*"+)*"/")
ws [ \t\r\n]+

CHAR_error ('\\x0{2,}{digit_16}*'|'\\x([1-9a-fA-F]){digit_16}*[g-z]{digit_16}*')
CHAR_tolong '\\x{digit_16}{3,}'
UINT_16_error 0[xX]([0-9a-fA-F]*[g-zG-Z]+[0-9a-fA-F]*)+

%%

{ws} {}
"int" { yylval.Token_value = New_Node(yylineno, "TYPE", "int", STRING); return TYPE; }
"float" { yylval.Token_value = New_Node(yylineno, "TYPE", "float", STRING); return TYPE; }
"char" { yylval.Token_value = New_Node(yylineno, "TYPE", "char", STRING); return TYPE; }

"struct" { yylval.Token_value = New_Node(yylineno, "STRUCT", "STRUCT", NAME); return STRUCT; }
"if" { yylval.Token_value = New_Node(yylineno, "IF", "IF", NAME); return IF; }
"for" { yylval.Token_value = New_Node(yylineno, "FOR", "FOR", NAME); return FOR; }
"else" { yylval.Token_value = New_Node(yylineno, "ELSE", "ELSE", NAME); return ELSE; }
"while" { yylval.Token_value = New_Node(yylineno, "WHILE", "WHILE", NAME); return WHILE; }
"return" { yylval.Token_value = New_Node(yylineno, "RETURN", "RETURN", NAME); return RETURN; }
"." { yylval.Token_value = New_Node(yylineno, "DOT", "DOT", NAME); return DOT; }
";" { yylval.Token_value = New_Node(yylineno, "SEMI", "SEMI", NAME); return SEMI; }
"," { yylval.Token_value = New_Node(yylineno, "COMMA", "COMMA", NAME); return COMMA; }
"=" { yylval.Token_value = New_Node(yylineno, "ASSIGN", "ASSIGN", NAME); return ASSIGN; }
"<" { yylval.Token_value = New_Node(yylineno, "LT", "LT", NAME); return LT; }
"<=" { yylval.Token_value = New_Node(yylineno, "LE", "LE", NAME); return LE; }
">" { yylval.Token_value = New_Node(yylineno, "GT", "GT", NAME); return GT; }
">=" { yylval.Token_value = New_Node(yylineno, "GE", "GE", NAME); return GE; }
"!=" { yylval.Token_value = New_Node(yylineno, "NE", "NE", NAME); return NE; }
"==" { yylval.Token_value = New_Node(yylineno, "EQ", "EQ", NAME); return EQ; }
"+" { yylval.Token_value = New_Node(yylineno, "PLUS", "PLUS", NAME); return PLUS; }
"-" { yylval.Token_value = New_Node(yylineno, "MINUS", "MINUS", NAME); return MINUS; }
"*" { yylval.Token_value = New_Node(yylineno, "MUL", "MUL", NAME); return MUL; }
"/" { yylval.Token_value = New_Node(yylineno, "DIV", "DIV", NAME); return DIV; }
"&&" { yylval.Token_value = New_Node(yylineno, "AND", "AND", NAME); return AND; }
"||" { yylval.Token_value = New_Node(yylineno, "OR", "OR", NAME); return OR; }
"!" { yylval.Token_value = New_Node(yylineno, "NOT", "NOT", NAME); return NOT; }
"(" { yylval.Token_value = New_Node(yylineno, "LP", "LP", NAME); return LP; }
")" { yylval.Token_value = New_Node(yylineno, "RP", "RP", NAME); return RP; }
"[" { yylval.Token_value = New_Node(yylineno, "LB", "LB", NAME); return LB; }
"]" { yylval.Token_value = New_Node(yylineno, "RB", "RB", NAME); return RB; }
"{" { yylval.Token_value = New_Node(yylineno, "LC", "LC", NAME); return LC; }
"}" { yylval.Token_value = New_Node(yylineno, "RC", "RC", NAME); return RC; }

{ID} { yylval.Token_value = New_Node(yylineno, "ID", yytext, STRING); return ID; }
{CHAR} { yylval.Token_value = New_Node(yylineno, "CHAR", yytext, CHAR_TYPE); return CHAR; }
{CHAR_16} { yylval.Token_value = New_Node(yylineno, "CHAR", yytext, CHAR_TYPE); return CHAR; }
{UINT_10} { yylval.Token_value = New_Node(yylineno, "INT", yytext, INT_TYPE); return INT; }
{UINT_16} { yylval.Token_value = New_Node(yylineno, "INT", yytext, CHAR_TYPE); return INT; }
{UFLOAT} { yylval.Token_value = New_Node(yylineno, "FLOAT", yytext, FLOAT_TYPE); return FLOAT; }
{annotation} {}

{CHAR_error} {
    isError=1;
    fprintf(out,"Error type A at Line %d: Wrong hex format for char '%s'\n",yylineno, yytext);
    return ILLEGAL_TOKEN;
}
{CHAR_tolong} {
    isError=1;
    fprintf(out,"Error type A at Line %d: Wrong hex format for char '%s'\n",yylineno, yytext);
    return ILLEGAL_TOKEN;
}
{UINT_16_error} {
    isError=1;
    fprintf(out,"Error type A at Line %d: Wrong hex format for int '%s'\n",yylineno, yytext);
    return ILLEGAL_TOKEN;
}
{FAKEID} {
    isError=1;
    fprintf(out,"Error type A at Line %d: Wrong format for ID '%s'\n",yylineno, yytext);
    return ID;
}
. {
    isError=1;
    fprintf(out,"Error type A at Line %d: unknown lexeme '%s'\n",yylineno, yytext);
    return ILLEGAL_TOKEN;
}
%%