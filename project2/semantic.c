#include "semantic.h"

Type* handle_VarDec(Node *node, Type *basic_type);
Type* handle_StructSpecifier(Node *node);
Type* handle_Specifier(Node *node);
Type_node* handle_ParamDec(Node *node);
Type_node* handle_VarList(Node *node);
void handle_FunDec(Node *node, Type *return_type, int defined);
void handle_ExtDef(Node *node);
void handle_Def(Node *node);
Type *get_exp_type(Node *node);
void handle_Stmt(Node *node, Type *correct_type);
void handle_CompSt(Node *node, Type *correct_type, int func_flag);
Node* Node_chlid(Node* node, int num);
int cur_depth = 0;
FILE *file = NULL;

Node* Node_chlid(Node* node, int num){
    Node* child = node->child;
    for(int i=0; i<num; i++){
        child=child->sibling;
    }
    return child;
}

unsigned int hash_func(char *name){
    unsigned int val = 0;
    for(; *name; ++name){
        val = (val << 2) + *name;
        unsigned int i = val & ~HASH_TABLE_SIZE;
        if(i)
            val = (val ^ (i >> 12)) & HASH_TABLE_SIZE;
    }
    //printf("%d\n", val);
    return val;
}

Var_hash_node* get_var_hash_node(char *key){
    int id = hash_func(key);
    Var_hash_node *tmp = var_hash_table[id];
    Var_hash_node *ans = NULL;
    while(tmp != NULL){
        if(strcmp(key, tmp->name) == 0){
            ans = tmp;
            return ans;
        }
        tmp = tmp->next;
    }
    return ans;
}

Func_hash_node* get_func_hash_node(char *key){
    int id = hash_func(key);
    Func_hash_node *tmp = func_hash_table[id];
    while(tmp != NULL){
        if(strcmp(key, tmp->name) == 0){
            return tmp;
        }
        else{
            tmp = tmp->next;
        }
    }
    return NULL;
}

void semantic_error(int error_type, int lineno, char *name){
    char msg[100] = "\0";
    if(error_type == 1)
        sprintf(msg, "undefined variable: %s", name);
    else if(error_type == 2)
        sprintf(msg, "undefined function: %s", name);
    else if(error_type == 3)
        sprintf(msg, "redefine variable: %s", name);
    else if(error_type == 4)
        sprintf(msg, "redefine function: %s", name);
    else if(error_type == 5)
        sprintf(msg, "unmatching type on both sides of assignment");
    else if(error_type == 6)
        sprintf(msg, "left side in assignment is rvalue");
    else if(error_type == 7)
        sprintf(msg, "binary operation on non-number variables");
    else if(error_type == 8)
        sprintf(msg, "incompatiable return type");
    else if(error_type ==9)
        sprintf(msg, "%s", name);
    else if(error_type == 10)
        sprintf(msg, "indexing on non-array variable");
    else if(error_type == 11)
        sprintf(msg, "invoking non-function variable: %s", name);
    else if(error_type == 12)
        sprintf(msg, "indexing by non-integer");
    else if(error_type == 13)
        sprintf(msg, "accessing with non-struct variable");
    else if(error_type == 14)
        sprintf(msg, "no such member: %s", name);
    else if(error_type == 15)
        sprintf(msg, "redefine struct: %s", name);
    fprintf(file, "Error type %d at Line %d: %s\n", error_type, lineno, msg);
}

void table_init(){
    for(int i = 0; i <= HASH_TABLE_SIZE; i++){
        var_hash_table[i] = NULL;
        func_hash_table[i] = NULL;
    }
}

static void node_type_check(Node *node, char *correct_name){
    if(node == NULL)
        printf("Error: %s NULL node\n", correct_name);
    node->visited = 1;
    if(strcmp(node->name, correct_name) != 0){
        printf("It is a '%s' Node, not a '%s' Node\n",node->name, correct_name);
    }
}

Type* my_int_type = NULL;;
Type* my_float_type = NULL;
Type* new_type(int basic){
    if(basic == BASIC_INT_TYPE){
        if(my_int_type == NULL){
            my_int_type = malloc(sizeof(Type));
            my_int_type->kind = BASIC;
            my_int_type->u.basic = BASIC_INT_TYPE;
        }
        return my_int_type;
    }
    else if(basic == BASIC_FLOAT_TYPE){
        if(my_float_type == NULL){
            my_float_type = malloc(sizeof(Type));
            my_float_type->kind = BASIC;
            my_float_type->u.basic = BASIC_FLOAT_TYPE;
        }
        return my_float_type;
    }
    printf("Unknown basic type\n");
    return NULL;
}

int type_equal(Type *a, Type *b){
    if(a == NULL || b == NULL){
        //error has been detected
        return 1;
    }
    if(a->kind != b->kind){
        return 0;
    }
    else{
        if(a->kind == BASIC){
            return a->u.basic == b->u.basic;
        }
        else if(a->kind == ARRAY){
            return type_equal(a->u.array.elem, b->u.array.elem);
        }
        else if(a->kind == STRUCTURE){
            FieldList *tmpa = a->u.structure;
            FieldList *tmpb = b->u.structure;
            while(tmpa != NULL && tmpb != NULL){
                if(type_equal(tmpa->type, tmpb->type)){
                    tmpa = tmpa->next;
                    tmpb = tmpb->next;
                }
                else return 0;
            }
            if(tmpa != NULL || tmpb != NULL)
                return 0;
            else return 1;
        }
    }
    return 0;
}


void insert_to_val_table(char *name, int line, Type *type){
    Var_hash_node *existed_node = get_var_hash_node(name);
    if(existed_node != NULL){
        if(type->kind == STRUCTURE){
            semantic_error(15, line, name);
            return;
        }
        else if(existed_node->type->kind == STRUCTURE){
            semantic_error(3, line, name);
        }
        else {
            //in the same scope
            if(existed_node->depth == cur_depth){
                semantic_error(3, line, name);
                return;
            }
        }
    }
    unsigned int i = hash_func(name);
    Var_hash_node *node = malloc(sizeof(Var_hash_node));
    node->name = malloc(strlen(name) + 1);
    strcpy(node->name, name);
    node->next = node->last = NULL;
    node->lineno = line;
    node->type = type;
    node->depth = cur_depth;
    node->op = NULL;
    if(var_hash_table[i] == NULL){
        var_hash_table[i] = node;
    }
    else{
        Var_hash_node *tmp = var_hash_table[i];
        while(tmp->next != NULL){
            tmp = tmp->next;
        }
        node->last = tmp;
        tmp->next = node;
    }
}

int paralist_equal(Type_node *para1, Type_node *para2){
    while(para1 != NULL && para2 != NULL){
        if(!type_equal(para1->type, para2->type))
            return 0;
        else{
            para1 = para1->next;
            para2 = para2->next;
        }
    }
    if(para1 != NULL || para2 != NULL)
        return 0;
    else return 1;
}


int func_equal(Func_hash_node *func1, Func_hash_node *func2){
    if(type_equal(func1->return_type, func2->return_type)){
        if(paralist_equal(func1->para_type_list, func2->para_type_list))
            return 1;
    }
    return 0;
}

void insert_to_func_table(char *name, int line, Type *return_type, Type_node* para_type_list, int defined){
    unsigned int i = hash_func(name);
    Func_hash_node *node = malloc(sizeof(Func_hash_node));
    node->name = malloc(strlen(name) + 1);
    strcpy(node->name, name);
    node->next = node->last = NULL;
    node->whether_dec = 0;
    node->whether_def = defined;
    node->lineno = line;
    node->return_type = return_type;
    node->para_type_list = para_type_list;
    if(func_hash_table[i] == NULL){
        func_hash_table[i] = node;
    }
    else{
        Func_hash_node *tmp = func_hash_table[i];
        while(tmp->next != NULL){
            tmp = tmp->next;
        }
        node->last = tmp;
        tmp->next = node;
    }
}

Node* get_id_node(Node *Vardec){
    node_type_check(Vardec, "VarDec");
    Node *tmp = Vardec;
    tmp = tmp->child;
    while(tmp->name != "ID"){
        tmp = tmp->child;
    }
    return tmp;
}

static Type* get_id_type(Node *Vardec, Type *basic_type){
    node_type_check(Vardec, "VarDec");
    Type *ans = basic_type;
    while(Vardec->sub_num == 4){
        Type *new_type = malloc(sizeof(Type));
        new_type->kind = ARRAY;
        new_type->u.array.size = Vardec->child->sibling->sibling->int_value;
        new_type->u.array.elem = ans;
        ans = new_type;
        Vardec = Vardec->child;
    }
    return ans;
}

Type* get_field_type(FieldList *field, char *id){
    FieldList *tmp = field;
    while(tmp != NULL){
        if(strcmp(tmp->name, id) == 0){
            return tmp->type;
        }
        tmp = tmp->next;
    }
    return NULL;
}

int equal_args_type(Type_node *para_list, Node *args){
    node_type_check(args, "Args");
    while(args != NULL && para_list != NULL){
        if(!type_equal(para_list->type, get_exp_type(args->child))){
            return 0;
        }
        else{
            para_list = para_list->next;
            if(args->sub_num==3){
                args = args->child->sibling->sibling;
            }
            else{
                args=args->sibling;
            }
        }
    }
    if(args != NULL || para_list != NULL){
        return 0;
    }
    else return 1;
}

Type* handle_Func_exp(Node *node){
    node_type_check(node, "Exp");
    node_type_check(node->child->sibling, "LP");
    char *name = node->child->str_value;
    Func_hash_node *func = get_func_hash_node(name);
    if(func == NULL){
        if(get_var_hash_node(name) != NULL)
            semantic_error(11, node->linenum, name);
        else semantic_error(2, node->linenum, name);
        return NULL;
    }
    else{
        if(node->sub_num == 3){
            if(func->para_type_list != NULL){
                semantic_error(9, node->linenum, name);
                return NULL;
            }
            else return func->return_type;
        }
        else if(node->sub_num == 4){
            //ID LP Args RP
            if(!equal_args_type(func->para_type_list, node->child->sibling->sibling)){
                int expect = 0;
                struct _Type_node *b = func->para_type_list;
                while(b!=NULL){
                    b=b->next;
                    expect++;
                }
                int got = 1;
                Node* temp = node->child->sibling->sibling;
                while(temp->child->sibling!=NULL){
                    got++;
                    temp=temp->child->sibling->sibling;
                }
                char error_msg[100]="\0";
                if(expect!=got){
                    sprintf(error_msg, "invalid argument number for %s, expect %d, got %d", name,expect,got);
                }else{
                    sprintf(error_msg, "invalid argument type for %s", name);
                }
                semantic_error(9, node->linenum, error_msg);
                return NULL;
            }
            else return func->return_type;
        }
    }
}


Type* get_exp_type(Node* node){
    node_type_check(node, "Exp");
    if(node->sub_num == 1){
        if(strcmp(node->child->name , "ID")==0){
            Var_hash_node *tmp = get_var_hash_node(node->child->str_value);
            if(tmp == NULL){
                semantic_error(1, node->linenum, node->child->str_value);
                return NULL;
            }
            return tmp->type;
        }
        else if(strcmp(node->child->name , "FLOAT")==0){
            return new_type(BASIC_FLOAT_TYPE);
        }
        else if(strcmp(node->child->name , "INT")==0){
            return new_type(BASIC_INT_TYPE);
        }
        else printf("Unknown type\n");
    }
    else if(node->sub_num == 3){
        if(strcmp(node->child->name, node->child->sibling->sibling->name) == 0){
            //Exp xx Exp
            char *oper = node->child->sibling->name;
            Type *type1 = get_exp_type(node->child);
            Type *type2 = get_exp_type(node->child->sibling->sibling);
            if(strcmp(oper , "PLUS") == 0 
            || strcmp(oper , "MINUS" ) == 0
            || strcmp(oper , "MUL" ) == 0
            || strcmp(oper , "DIV") == 0){
                if(type_equal(type1, type2)){
                    //Exp1.type == Exp2.type
                    if(type1 == NULL || type2 == NULL)
                        return NULL;
                    else return type1;
                }
                else{
                    semantic_error(7, node->linenum, NULL);
                    return NULL;
                }
            }
            else if(strcmp(oper , "AND") == 0 || strcmp(oper , "OR") == 0){
                if(type1 == NULL || type2 == NULL)//error has occurred before
                    return NULL;
                if(type1->u.basic != BASIC_INT_TYPE || type2->u.basic != BASIC_INT_TYPE){
                    semantic_error(7, node->linenum, NULL);
                    return NULL;
                }
                else{
                    return new_type(BASIC_INT_TYPE);
                }
            }
            else if(strcmp(oper , "LT") == 0 
            || strcmp(oper ,"LE") == 0
            || strcmp(oper ,"GT") == 0
            || strcmp(oper ,"GE") == 0
            || strcmp(oper ,"NE") == 0
            || strcmp(oper ,"EQ") == 0){
                if(type_equal(type1, type2)){
                    return new_type(BASIC_INT_TYPE);
                }
                else{
                    semantic_error(7, node->linenum, NULL);
                    return NULL;
                }
            }
            else if(strcmp(oper , "ASSIGN") == 0){
                Node *exp = node->child;
                if(exp->sub_num == 1 && strcmp( exp->child->name , "ID") == 0
                   || exp->sub_num == 3 && strcmp( exp->child->sibling->name , "DOT") == 0
                   || exp->sub_num == 4 && strcmp( exp->child->sibling->name , "LB") == 0){
                    if(type_equal(type1, type2)){
                        //Exp1.type == Exp2.type
                        return type1;
                    }
                    else{
                        semantic_error(5, node->linenum, NULL);
                        return NULL;
                    }
                }
                else{
                    semantic_error(6, node->linenum, NULL);
                    return NULL;
                }
            }
        }
        else if(strcmp(node->child->name , "LP") == 0){
            // LP Exp RP
            return get_exp_type(node->child->sibling);
        }
        else if(strcmp(node->child->sibling->name , "DOT") == 0){
            //Exp DOT ID TODO
            Type *type = get_exp_type(node->child);
            if(type == NULL || type->kind != STRUCTURE){
                semantic_error(13, node->linenum, NULL);
            }
            else{
                char *id = node->child->sibling->sibling->str_value;
                Type *field_type = get_field_type(type->u.structure, id);
                if(field_type == NULL){
                    semantic_error(14, node->linenum, id);
                }
                return field_type;
            }
        }
        else if(strcmp(node->child->name , "ID") == 0){
            //Func()
            return handle_Func_exp(node);
        }
    }
    else if(node->sub_num == 2){
        if(strcmp(node->child->name , "MINUS") == 0){
            return get_exp_type(node->child->sibling);
        }
        else if(strcmp(node->child->name , "NOT") == 0){
            Type *type = get_exp_type(node->child->sibling);
            if(type == NULL)//error has occurred before
                return NULL;
            if(type->u.basic != BASIC_INT_TYPE){
                semantic_error(7, node->linenum, NULL);
                return NULL;
            }
            else{
                return new_type(BASIC_INT_TYPE);
            }
        }
    }
    else if(node->sub_num == 4){
        if(strcmp(node->child->name , "ID") == 0){
            //Func(args)
            return handle_Func_exp(node);
        }
        else if(strcmp(node->child->sibling->name , "LB") == 0){
            Type *type = get_exp_type(node->child);
            if(type == NULL || type->kind != ARRAY){
                semantic_error(10, node->linenum, "Exp");
                return NULL;
            }
            else if(!type_equal(get_exp_type(node->child->sibling->sibling), my_int_type)){
                semantic_error(12, node->linenum, "Exp");
                return NULL;
            }
            return type->u.array.elem;
        }
    }
}

Type* handle_VarDec(Node *node, Type *basic_type){
    node_type_check(node, "VarDec");
    Node *ID_node = get_id_node(node);
    Type *ID_type = get_id_type(node, basic_type);
    insert_to_val_table(ID_node->str_value, ID_node->linenum, ID_type);
    return ID_type;
}

FieldList* handle_struct_VarDec(Node *node, Type *basic_type){
    node_type_check(node, "VarDec");
    Node *ID_node = get_id_node(node);
    Type *ID_type = get_id_type(node, basic_type);
    FieldList *field = malloc(sizeof(FieldList));
    field->name = ID_node->str_value;
    field->type = ID_type;
    field->next = NULL;
    return field;
}

Type* handle_StructSpecifier(Node *node){
    node_type_check(node, "StructSpecifier");
    Type *type = malloc(sizeof(Type));
    type->kind = STRUCTURE;
    type->u.structure = NULL;
    FieldList *last_node = type->u.structure;
    if(node->sub_num == 5){
        Node *DefList = node->child->sibling->sibling->sibling;
        while(DefList != NULL && DefList->sub_num == 2){
            DefList->visited = 1;
            Node *Def = DefList->child;
            //handle type
            Type *basic_type = handle_Specifier(Def->child);
            //handle variable
            Node *Declist_node = Def->child->sibling;
            while(1){
                Node *Dec_node = Declist_node->child;
                Node *Vardec_node = Dec_node->child;
                if(Dec_node->sub_num == 1){
                    FieldList* field = handle_struct_VarDec(Vardec_node, basic_type);
                        if(last_node == NULL){
                            type->u.structure = last_node = field;
                        }
                        else{
                            last_node->next = field;
                            last_node = field;
                        }

                }
                if(Declist_node->sub_num > 1){
                    Declist_node = Declist_node->child->sibling->sibling;
                }
                else break;
            }
            DefList = DefList->child->sibling;
        }
        if(node->child->sibling != NULL){
            Node *id_node = node->child->sibling;
            insert_to_val_table(id_node->str_value, id_node->linenum, type);
        }
    }
    else if(node->sub_num == 2){
        char *name = node->child->sibling->str_value;
        Var_hash_node *hash_node = get_var_hash_node(name);
        type = hash_node->type;
    }
    return type;
}

Type* handle_Specifier(Node *node){
    node_type_check(node, "Specifier");
    Type *type;
    Node *type_node = node->child;
    if(strcmp( type_node->name , "TYPE") == 0){
        if(strcmp(type_node->str_value, "int") == 0){
            type = new_type(BASIC_INT_TYPE);
        }
        else if(strcmp(type_node->str_value, "float") == 0){
            type = new_type(BASIC_FLOAT_TYPE);
        }
    }
    else if(strcmp(type_node->name, "StructSpecifier") == 0){
        type = handle_StructSpecifier(type_node);

    }
    return type;
}

Type_node* handle_ParamDec(Node *node){
    node_type_check(node, "ParamDec");
    Type *type = handle_Specifier(node->child);
    type = handle_VarDec(node->child->sibling, type);
    Type_node* type_node = malloc(sizeof(Type_node));
    Node *ID = get_id_node(node->child->sibling);
    type_node->type = type;
    type_node->next = NULL;
    type_node->name = ID->str_value;
    return type_node;
}

Type_node* handle_VarList(Node *node){
    node_type_check(node, "VarList");
    Node *cur_node = node;
    Type_node *type_node = handle_ParamDec(node->child);
    if(node->sub_num > 1){
        type_node->next = handle_VarList(node->child->sibling->sibling);
    }
    return type_node;
}

void handle_FunDec(Node *node, Type *return_type, int defined){
    node_type_check(node, "FunDec");
    char *name = node->child->str_value;
    int lineno = node->child->linenum;
    Type_node *para_list = NULL;
    if(node->sub_num == 4){
        para_list = handle_VarList(node->child->sibling->sibling);
    }
    Func_hash_node *old_func = get_func_hash_node(name);
    if(old_func == NULL){
        insert_to_func_table(name, lineno, return_type, para_list, defined);
    }
    else{
        if(type_equal(return_type, old_func->return_type) && paralist_equal(para_list, old_func->para_type_list)){
            if(old_func->whether_def == 0){
                old_func->whether_def = defined;
            }
            else{
                if(defined == 1){//redefine function
                    semantic_error(4, node->linenum, name);
                }
            }
        }
        else{
            if(defined == 1 && old_func->whether_def == 1)
                semantic_error(4, node->linenum, name);
        }
    }
}

void handle_ExtDef(Node *node){//node shoule be a ExtDef
    node_type_check(node, "ExtDef");
    //handle type
    Type *basic_type = handle_Specifier(node->child);
    //handle variable
    if(strcmp(node->child->sibling->name, "ExtDecList") == 0){
        Node *ExtDecList_node = node->child->sibling;
        while(1){
            Node *Vardec_node = ExtDecList_node->child;
            handle_VarDec(Vardec_node, basic_type);
            if(ExtDecList_node->sub_num > 1){
                ExtDecList_node = ExtDecList_node->child->sibling->sibling;
            }
            else break;
        }
    }
    else if(strcmp(node->child->sibling->name, "FunDec") == 0){
        Node *FunDec_node = node->child->sibling;
        if(strcmp(node->child->sibling->sibling->name , "SEMI") ==0){
            handle_FunDec(FunDec_node, basic_type, 0);
        }
        else if(strcmp(node->child->sibling->sibling->name, "CompSt") == 0){
            cur_depth++;
            handle_FunDec(FunDec_node, basic_type, 1);
            handle_CompSt(node->child->sibling->sibling, basic_type, 1);
        }
        else
            printf("Uknown FunDec\n");

    }
}

void handle_Def(Node *node){
    node_type_check(node, "Def");
    //handle type
    Type *basic_type = handle_Specifier(node->child);
    //handle variable
    Node *Declist_node = node->child->sibling;
    while(1){
        Node *Dec_node = Declist_node->child;
        Node *Vardec_node = Dec_node->child;
        if(Dec_node->sub_num == 1){
            handle_VarDec(Vardec_node, basic_type);
        }
            //Dec -> VarDec ASSIGNOP Exp
        else if(Dec_node->sub_num == 3){
            Type *type = handle_VarDec(Vardec_node, basic_type);
            if(!type_equal(type, get_exp_type(Dec_node->child->sibling->sibling))){
                semantic_error(5, Dec_node->linenum, NULL);
            }

        }
        if(Declist_node->sub_num > 1){
            Declist_node = Declist_node->child->sibling->sibling;
        }
        else break;
    }
}

void handle_DefList(Node *node){
    if(node == NULL)
        return;
    node_type_check(node, "DefList");
    Node *DefList = node;
    while(DefList != NULL && DefList->sub_num == 2){
        DefList->visited = 1;
        handle_Def(DefList->child);
        DefList = DefList->child->sibling;
    }
}

void handle_CompSt(Node *node, Type *correct_type, int func_flag){
    node_type_check(node, "CompSt");
    if(!func_flag){//there is not a funcDec before it
        cur_depth++;
    }
    handle_DefList(node->child->sibling);
    Node *StmtList = node->child->sibling->sibling;
    while(StmtList != NULL && StmtList->sub_num > 1){
        handle_Stmt(StmtList->child, correct_type);
        StmtList = StmtList->child->sibling;
    }
}

void handle_Stmt(Node *node, Type *correct_type){
    node_type_check(node, "Stmt");
    if(node->child->sibling != NULL && strcmp(node->child->sibling->name , "LP") == 0){
        //IF/WHILE LP EXP RP
        if(!type_equal(my_int_type, get_exp_type(node->child->sibling->sibling)))
            semantic_error(7, node->linenum, NULL);
        handle_Stmt(node->child->sibling->sibling->sibling->sibling, correct_type);
        if(node->sub_num == 7){
            handle_Stmt(node->child->sibling->sibling->sibling->sibling->sibling->sibling, correct_type);
        }
    }
    else if(strcmp(node->child->name , "RETURN") == 0){
        if(!type_equal(correct_type, get_exp_type(node->child->sibling)))
            semantic_error(8, node->linenum, NULL);
    }
    else if(strcmp(node->child->name, "CompSt") == 0){
        handle_CompSt(node->child, correct_type, 0);
    }
    else if(strcmp(node->child->sibling->name , "SEMI") == 0){//Exp SEMI
        get_exp_type(node->child);
    }
    else{
        printf("Error, unknown Stmt");
    }
}

void add_var(Node *node){
    if(strcmp(node->name, "ExtDef") == 0){
        handle_ExtDef(node);
    }
    else if(strcmp(node->name, "DefList") == 0 && node->visited == 0){
        handle_DefList(node);
    }
    else if(strcmp(node->name, "Exp") == 0){
        get_exp_type(node);
    }
}

static void tree_search(Node *node){
    if(node == NULL || node->visited == 1) return;
    add_var(node);
    for(int i = 0; i < node->sub_num; i++){
        Node* tmp=Node_chlid(node, i);
        if( tmp!= NULL){
            tree_search(tmp);
        }
    }
}

void semantic_func(FILE *file_pointer, Node* root){
    file=file_pointer;
    table_init();
    tree_search(root);
}