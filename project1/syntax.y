%{
    #include "tree.h"
    #include "lex.yy.c"
    Node* root_Node;
    void yyerror(const char *s);
%}
%locations
%union{
    Node* Token_value;
}
%nonassoc LOWER_ERROR

%type <Token_value> Program ExtDefList ExtDef ExtDecList Specifier
%type <Token_value> StructSpecifier VarDec FunDec VarList ParamDec
%type <Token_value> CompSt StmtList Stmt DefList Def DecList Dec Args Exp

%token <Token_value> SEMI COMMA
%token <Token_value> LC RC
%token <Token_value> TYPE STRUCT
%token <Token_value> IF WHILE RETURN FOR
%token <Token_value> INT
%token <Token_value> FLOAT
%token <Token_value> CHAR
%token <Token_value> ID

%right <Token_value> ASSIGN
%left <Token_value> OR
%left <Token_value> AND
%left <Token_value> LT LE GT GE NE EQ
%nonassoc LOWER_MINUS
%left <Token_value> PLUS MINUS
%left <Token_value> MUL DIV
%right <Token_value> NOT
%left <Token_value> LP RP LB RB DOT

%nonassoc <Token_value> ILLEGAL_TOKEN
%nonassoc <Token_value> ELSE
%%

/* high-level definition */
Program: ExtDefList { $$=New_Parent_Node("Program", @$.first_line, $1); root_Node=$$; }
    ;

ExtDefList: {$$=New_Parent_Node("ExtDefList", @$.first_line, NULL); $$->type=NOTHING;}
    | ExtDef ExtDefList { Connect_Nodes(2, $1, $2); $$=New_Parent_Node("ExtDefList", @$.first_line, $1); }
    ;

ExtDef: Specifier ExtDecList SEMI { Connect_Nodes(3, $1, $2, $3); $$=New_Parent_Node("ExtDef", @$.first_line, $1); }
    | Specifier SEMI { Connect_Nodes(2, $1, $2); $$=New_Parent_Node("ExtDef", @$.first_line, $1); }
    | Specifier FunDec CompSt { Connect_Nodes(3, $1, $2, $3); $$=New_Parent_Node("ExtDef", @$.first_line, $1); }
    | Specifier ExtDecList error {Type_B_Error("Missing semicolon \';\'");}
    | Specifier error {Type_B_Error("Missing semicolon \';\'");}
    ;

ExtDecList: VarDec { $$=New_Parent_Node("ExtDecList", @$.first_line, $1); }
    | VarDec COMMA ExtDecList { Connect_Nodes(3, $1, $2, $3); $$=New_Parent_Node("ExtDecList", @$.first_line, $1); }
    | VarDec ExtDecList error {Type_B_Error("Missing COMMA \',\'");}
    ;

/* specifier */
Specifier: TYPE { $$=New_Parent_Node("Specifier", @$.first_line, $1); }
    | StructSpecifier { $$=New_Parent_Node("Specifier", @$.first_line, $1); }
    ;

StructSpecifier: STRUCT ID LC DefList RC { Connect_Nodes(5, $1, $2, $3, $4, $5); $$=New_Parent_Node("StructSpecifier", @$.first_line, $1); }
    | STRUCT ID { Connect_Nodes(2, $1, $2); $$=New_Parent_Node("StructSpecifier", @$.first_line, $1); }
    //| STRUCT ID LC DefList error {Type_B_Error("Missing RC \'}\'");}
    ;

/* declarator */
VarDec: ID { $$=New_Parent_Node("VarDec", @$.first_line, $1); }
    | VarDec LB INT RB { Connect_Nodes(4, $1, $2, $3, $4); $$=New_Parent_Node("VarDec", @$.first_line, $1); }
    | VarDec LB INT error %prec LOWER_ERROR {Type_B_Error("Missing RB \']\'");}
    ;

FunDec: ID LP VarList RP { Connect_Nodes(4, $1, $2, $3, $4); $$=New_Parent_Node("FunDec", @$.first_line, $1); }
    | ID LP RP { Connect_Nodes(3, $1, $2, $3); $$=New_Parent_Node("FunDec", @$.first_line, $1); }
    | ID LP VarList error { Type_B_Error("Missing RP \')\'"); }
    | ID LP error { Type_B_Error("Missing RP \')\'"); }
    ;

VarList: ParamDec COMMA VarList { Connect_Nodes(3, $1, $2, $3); $$=New_Parent_Node("VarList", @$.first_line, $1); }
    | ParamDec VarList error {Type_B_Error("Missing COMMA \',\'");}
    | ParamDec { $$=New_Parent_Node("VarList", @$.first_line, $1); }
    ;

ParamDec: Specifier VarDec { Connect_Nodes(2, $1, $2); $$=New_Parent_Node("ParamDec", @$.first_line, $1); }
    ;

/* statement */
CompSt: LC DefList StmtList RC { Connect_Nodes(4, $1, $2, $3, $4); $$=New_Parent_Node("CompSt", @$.first_line, $1); }
    ;

StmtList: {$$=New_Parent_Node("StmtList", @$.first_line, NULL); $$->type=NOTHING;}
    | Stmt StmtList { Connect_Nodes(2, $1, $2); $$=New_Parent_Node("StmtList", @$.first_line, $1); }
    ;

Stmt: Exp SEMI { Connect_Nodes(2, $1, $2); $$=New_Parent_Node("Stmt", @$.first_line, $1); }
    | CompSt { $$=New_Parent_Node("Stmt", @$.first_line, $1); }
    | RETURN Exp SEMI { Connect_Nodes(3, $1, $2, $3); $$=New_Parent_Node("Stmt", @$.first_line, $1); }
    | IF LP Exp RP Stmt { Connect_Nodes(5, $1, $2, $3, $4, $5); $$=New_Parent_Node("Stmt", @$.first_line, $1); }
    | IF LP Exp RP Stmt ELSE Stmt { Connect_Nodes(7, $1, $2, $3, $4, $5, $6, $7); $$=New_Parent_Node("Stmt", @$.first_line, $1); }
    | WHILE LP Exp RP Stmt { Connect_Nodes(5, $1, $2, $3, $4, $5); $$=New_Parent_Node("Stmt", @$.first_line, $1); }
    | WHILE LP Exp error Stmt { Type_B_Error("Missing RP \')\'"); }
    | Exp error {Type_B_Error("Missing semicolon \';\'");}
    | RETURN Exp error {Type_B_Error("Missing semicolon \';\'");}
    | IF LP Exp error Stmt  { Type_B_Error("Missing RP \')\'"); }
    | IF error Exp RP Stmt { Type_B_Error("Missing LP \'(\'"); }
    | FOR LP Exp SEMI Exp SEMI Exp RP Stmt { Connect_Nodes(9, $1, $2, $3, $4, $5, $6, $7, $8, $9); $$=New_Parent_Node("Stmt", @$.first_line, $1); }
    ;

/* local definition */
DefList: {$$=New_Parent_Node("DefList", @$.first_line, NULL); $$->type=NOTHING;}
    | Def DefList { Connect_Nodes(2, $1, $2); $$=New_Parent_Node("DefList", @$.first_line, $1); }
    ;

Def: Specifier DecList SEMI { Connect_Nodes(3, $1, $2, $3); $$=New_Parent_Node("Def", @$.first_line, $1); }
    | Specifier DecList error {Type_B_Error("Missing semicolon \';\'");}
    | error DecList SEMI {Type_B_Error("Missing Specifier");}
    ;

DecList: Dec { $$=New_Parent_Node("DecList", @$.first_line, $1); }
    | Dec COMMA DecList { Connect_Nodes(3, $1, $2, $3); $$=New_Parent_Node("DecList", @$.first_line, $1); }
    | Dec DecList error { Type_B_Error("Missing COMMA \',\'"); }
    ;

Dec: VarDec { $$=New_Parent_Node("Dec", @$.first_line, $1); }
    | VarDec ASSIGN Exp { Connect_Nodes(3, $1, $2, $3); $$=New_Parent_Node("Dec", @$.first_line, $1); }
    ;

Args: Exp COMMA Args { Connect_Nodes(3, $1, $2, $3); $$=New_Parent_Node("Args", @$.first_line, $1); }
    | Exp { $$=New_Parent_Node("Args", @$.first_line, $1); }
    ;

/* Expression */
Exp: Exp ASSIGN Exp { Connect_Nodes(3, $1, $2, $3); $$=New_Parent_Node("Exp", @$.first_line, $1); }
    | Exp AND Exp { Connect_Nodes(3, $1, $2, $3); $$=New_Parent_Node("Exp", @$.first_line, $1); }
    | Exp OR Exp { Connect_Nodes(3, $1, $2, $3); $$=New_Parent_Node("Exp", @$.first_line, $1); }
    | Exp LT Exp { Connect_Nodes(3, $1, $2, $3); $$=New_Parent_Node("Exp", @$.first_line, $1); }
    | Exp LE Exp { Connect_Nodes(3, $1, $2, $3); $$=New_Parent_Node("Exp", @$.first_line, $1); }
    | Exp GT Exp { Connect_Nodes(3, $1, $2, $3); $$=New_Parent_Node("Exp", @$.first_line, $1); }
    | Exp GE Exp { Connect_Nodes(3, $1, $2, $3); $$=New_Parent_Node("Exp", @$.first_line, $1); }
    | Exp NE Exp { Connect_Nodes(3, $1, $2, $3); $$=New_Parent_Node("Exp", @$.first_line, $1); }
    | Exp EQ Exp { Connect_Nodes(3, $1, $2, $3); $$=New_Parent_Node("Exp", @$.first_line, $1); }
    | Exp PLUS Exp { Connect_Nodes(3, $1, $2, $3); $$=New_Parent_Node("Exp", @$.first_line, $1); }
    | Exp MINUS Exp { Connect_Nodes(3, $1, $2, $3); $$=New_Parent_Node("Exp", @$.first_line, $1); }
    | Exp MUL Exp { Connect_Nodes(3, $1, $2, $3); $$=New_Parent_Node("Exp", @$.first_line, $1); }
    | Exp DIV Exp { Connect_Nodes(3, $1, $2, $3); $$=New_Parent_Node("Exp", @$.first_line, $1); }
    | LP Exp RP { Connect_Nodes(3, $1, $2, $3); $$=New_Parent_Node("Exp", @$.first_line, $1); }
    | LP Exp error { Type_B_Error("Missing RP \')\'"); }
    | MINUS Exp %prec LOWER_MINUS { Connect_Nodes(2, $1, $2); $$=New_Parent_Node("Exp", @$.first_line, $1); }
    | NOT Exp { Connect_Nodes(2, $1, $2); $$=New_Parent_Node("Exp", @$.first_line, $1); }
    | ID LP Args RP { Connect_Nodes(4, $1, $2, $3, $4); $$=New_Parent_Node("Exp", @$.first_line, $1); }
    | ID LP Args error { Type_B_Error("Missing RP \')\'");} 
    | ID LP RP { Connect_Nodes(3, $1, $2, $3); $$=New_Parent_Node("Exp", @$.first_line, $1); }
    | ID LP error { Type_B_Error("Missing RP \')\'");} 
    | Exp LB Exp RB { Connect_Nodes(4, $1, $2, $3, $4); $$=New_Parent_Node("Exp", @$.first_line, $1); }
    | Exp LB Exp error { Type_B_Error("Missing RB \']\'"); }
    | Exp DOT ID { Connect_Nodes(3, $1, $2, $3); $$=New_Parent_Node("Exp", @$.first_line, $1); }
    | ID { $$=New_Parent_Node("Exp", @$.first_line, $1); }
    | INT { $$=New_Parent_Node("Exp", @$.first_line, $1); }
    | FLOAT { $$=New_Parent_Node("Exp", @$.first_line, $1); }
    | CHAR { $$=New_Parent_Node("Exp", @$.first_line, $1); }
    | Exp ILLEGAL_TOKEN Exp {}
    | ILLEGAL_TOKEN {}
    ;

%%

void yyerror(const char *s){
    isError=1;
    fprintf(out,"Error type B at Line %d: ",yylloc.first_line-1);
}

int main(int argc, char **argv) {
    if (argc < 2) {
        fprintf(stderr, "no input path\n");
        return 1;
    }
    else if (argc > 2) {
        fprintf(stderr, "too much input path\n");
        return 1;
    }
    else{
        in = fopen(argv[1], "r");
        if (!in) {
            perror(argv[1]);
            return 1;
        }

        char name[20];
        strcpy(name, argv[1]);
        char *p = strstr(name, ".bpl");
        if (!p) {
            fprintf(stderr, "Input file should have the '.bpl' extension.\n");
            fclose(in);
            return 1;
        }
        strcpy(p, ".out");

        out = fopen(name, "w");
        yyrestart(in);
        yyparse();

        if(isError==0){
            Node_print(out,root_Node,0);
        }
        freeAST(root_Node);
        root_Node = NULL;

        fclose(out);
        fclose(in);
    }
    return 0;
}