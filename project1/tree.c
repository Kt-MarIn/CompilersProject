#include "tree.h"

Node* New_Node(char* name, char* value, Node_TYPE type){
    Node* node=(Node*)malloc(sizeof(Node));

    node->sub_num=0;
    node->sib_num=1;
    node->child=NULL;
    node->sibling=NULL;

    node->name=name;
    node->type=type;
    if(type==INT_TYPE){
        node->int_value=atoi(value);
    }
    else if(type==FLOAT_TYPE){
        node->float_value=atof(value);
    }
    else if(type==LINE){
        node->linenum=atoi(value);
    }
    else if(type==CHAR_TYPE){
        char* char_value = (char*)malloc(strlen(value) + 1); 
        strcpy(char_value, value); 
        node->char_value = char_value; 
    }
    else{
        char* str_value = (char*)malloc(strlen(value) + 1); 
        strcpy(str_value, value); 
        node->str_value = str_value; 
    }
    return node;
}

Node* New_Parent_Node(char* name, int value, Node* child){
    Node* node=(Node*)malloc(sizeof(Node));
    node->sib_num = 1;
    node->sub_num = 0;
    node->child=child;
    if(node->child!=NULL){
        node->sub_num = child->sib_num;
    }
    node->sibling=NULL;

    node->name=name;
    node->type=LINE;
    node->linenum=value;

    return node;
}

void Connect_Nodes(int num,...){
    va_list valist;
    va_start(valist,num);
    Node* node=va_arg(valist, Node*);
    node->sib_num=num;
    for(int i=1;i<num;++i)
    {
        Node* next=va_arg(valist, Node*);
        node->sibling=next;
        node=next;
        node->sib_num=num;
    }
    node->sibling=NULL;
    va_end(valist);
}

void Node_print(FILE *file_pointer, Node* head_node, int space){
    if(head_node->type!=NOTHING){
        for(int i=0;i<space;i++){
            fprintf(file_pointer, "  ");
        }
    }
    switch (head_node->type)
    {
    case LINE:
        fprintf(file_pointer, "%s (%d)\n", head_node->name,head_node->linenum);
        break;
    case NAME:
        fprintf(file_pointer, "%s\n", head_node->name);
        break;
    case STRING:
        fprintf(file_pointer, "%s: %s\n", head_node->name, head_node->str_value);
        break;
    case CHAR_TYPE:
        fprintf(file_pointer, "%s: %s\n", head_node->name, head_node->char_value);
        break;
    case INT_TYPE:
        fprintf(file_pointer, "%s: %d\n", head_node->name, head_node->int_value);
        break;
    case FLOAT_TYPE:
        fprintf(file_pointer, "%s: %f\n", head_node->name, head_node->float_value);
        break;
    default:
        break;
    }
    Node* node=head_node->child;
    while(node!=NULL){
        Node_print(file_pointer, node, space+1);
        node=node->sibling;
    }
}

void freeAST(Node* head)
{
    Node* node=head->child;
    while (node!=NULL)
    {
        freeAST(node);
        node=node->sibling;
    }
    free(head);
}

void Type_B_Error(char* msg){
    fprintf(out, "%s\n", msg);
}