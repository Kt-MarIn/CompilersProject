%{
    #include"lex.yy.c"
    void yyerror(const char*);
%}

%right LC RC LB RB COLON COMMA
%token STRING NUMBER
%token TRUE FALSE VNULL
%%

Json:
      Value
    ;
Value:
      Object
    | Array
    | STRING
    | NUMBER
    | TRUE
    | FALSE
    | VNULL
    | NUMBER NUMBER error { puts("Numbers cannot have leading zeros, recovered"); }
    ;

Object:
      LC RC
    | LC Members RC
    | LC Members RC Value error { puts("extra value after close, recovered"); }
    ;
Members:
      Member
    | Member COMMA Members
    | Member COMMA RC error { puts("extra comma, recovered"); }
    | Member COMMA error { puts("comma instead if closing brace, recovered"); }
    ;
Member:
      STRING COLON Value
    | STRING Value error { puts("missing colon, recovered"); }
    | STRING COLON COLON Value error { puts("double colon, recovered"); }
    | STRING COMMA Value error { puts("comma instead of colon, recovered"); }
    ;
Array:
      LB RB
    | LB Values RB
    | LB Values RC error { puts("mismatch, recovered"); }
    | LB Values error { puts("Unclosed array, recovered"); }
    | LB Values COLON error { puts("colon instead of comma, recovered"); }
    | LB Values RB COMMA error { puts("Comma after the close, recovered"); }
    | LB Values RB RB error { puts("Extra close, recovered"); }
    ;
    
Values:
      Value
    | Value COMMA Values
    | Value COMMA error { puts("extra comma, recovered"); }
    | Value COMMA COMMA error { puts("double extra comma, recovered"); }
    | COMMA Values error { puts("<--missing value, recovered"); }
    ;
%%

void yyerror(const char *s){
    printf("syntax error: ");
}

int main(int argc, char **argv){
    if(argc != 2) {
        fprintf(stderr, "Usage: %s <file_path>\n", argv[0]);
        exit(-1);
    }
    else if(!(yyin = fopen(argv[1], "r"))) {
        perror(argv[1]);
        exit(-1);
    }
    yyparse();
    return 0;
}
