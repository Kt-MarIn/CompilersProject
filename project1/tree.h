#ifndef FUNC_H
#define FUNC_H
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>

typedef enum {
    LINE,
    NAME,
    STRING,
    CHAR_TYPE,
    INT_TYPE,
    FLOAT_TYPE,
    NOTHING,
} Node_TYPE;

typedef struct Node{
    char* name;
    char* str_value;
    Node_TYPE type;

    int sib_num;
    int sub_num;
    struct Node* child;
    struct Node* sibling;
    union{
        int linenum;
        char* char_value;
        int int_value;
        float float_value;
    };
}Node;

FILE *in;
FILE *out;

Node* New_Node(char* name, char* value, Node_TYPE type);
Node* New_Parent_Node(char* name, int value, Node* child);
void Connect_Nodes(int num, ...);
void Node_print(FILE *file_pointer, Node* head_node, int space);
void freeAST(Node* head);
void Type_B_Error(char* msg);

#endif
