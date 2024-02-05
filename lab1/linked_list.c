#include "linked_list.h"

node *linked_list_init(){
    node *head = (node*)malloc(sizeof(node));
    head->count = 0;
    head->next = NULL;
    return head;
}

void linked_list_free(node *head){
    node *cur = head;
    node *last;
    while(cur != NULL){
        last = cur;
        cur = cur->next;
        free(last);
    }
}

char linked_list_string[0x10000];

char *linked_list_tostring(node *head){
    node *cur = head->next;
    char *position;
    int length = 0;
    while(cur != NULL){
        position = linked_list_string + length;
        length += sprintf(position, "%d", cur->value);
        cur = cur->next;
        if(cur != NULL){
            position = linked_list_string + length;
            length += sprintf(position, "->");
        }
    }
    position = linked_list_string + length;
    length += sprintf(position, "%c", '\0');
    return linked_list_string;
}

int linked_list_size(node *head){
    return head->count;
}

void linked_list_append(node *head, int val){
    node *cur = head;
    node *new_node;
    while(cur->next != NULL){
        cur = cur->next;
    }
    new_node = (node*)malloc(sizeof(node));
    new_node->value = val;
    new_node->next = NULL;
    cur->next = new_node;
    head->count++;
}

/* your implementation goes here */

void linked_list_insert(node *head, int val, int index){
    if (head == NULL || index < 0 || index > head->count) {
        return;
    }
    node *cur = head;
    node *new_node;
    for(int i=0;i<index;i++){
        cur = cur->next;
    }
    new_node = (node*)malloc(sizeof(node));
    new_node->value = val;
    new_node->next = cur->next;
    cur->next = new_node;
    head->count++;
}

void linked_list_delete(node *head, int index){
     if (head == NULL || index < 0 || index > head->count-1) {
        return;
    }
    node *cur = head;
    node *deleteone;
    // 如果删除的是头节点
    if (index == 0) {
        deleteone = head->next;
        head->next = deleteone->next;
    } else {
        for (int i = 0; i < index; i++) {
            cur = cur->next;
        }
        deleteone = cur->next;
        cur->next = deleteone->next;
    }
    free(deleteone);
    head->count--;
}

void linked_list_remove(node *head, int val){
    if (head == NULL) {
        return;
    }

    node *cur = head;
    node *deleteone;
    int temp = 0;
    // 如果删除的是头节点
    if (head->next->value == val) {
        deleteone = head->next;
        head->next = deleteone->next;
        free(deleteone);
        head->count--;
        return;
    }
    while (cur->next != NULL && temp!=1) {
        if (cur->next->value == val) {
            temp = 1;
            deleteone = cur->next;
            cur->next = deleteone->next;
            free(deleteone);
            head->count--;
        } else {
            cur = cur->next;
        }
    }
}

void linked_list_remove_all(node *head, int val){
    if (head == NULL) {
        return;
    }
    node *cur = head;
    node *deleteone; 
    while (cur->next != NULL) {
        if (cur->next->value == val) {
            deleteone = cur->next;
            cur->next = deleteone->next;
            free(deleteone);
            head->count--;
        } else {
            cur = cur->next;
        }
    }
}

int linked_list_get(node *head, int index){
    if (head == NULL || index > head->count-1) {
        return -0x80000000;
    }
    node *cur = head->next;
    for (int i = 0; i < index; i++) {
        cur = cur->next;
    }
    return cur->value;
}

int linked_list_search(node *head, int val){
    if (head == NULL) {
        return -1;
    }
    node *cur = head->next;
    int index = 0;
    while (cur != NULL) {
        if (cur->value == val) {
            return index;
        }
        cur = cur->next;
        index++;
    }
    return -1;
}

node *linked_list_search_all(node *head, int val){
    if (head == NULL) {
        return 0;
    }
    node *result_head = linked_list_init(); 
    node *cur = head->next;
    node *result_cur = result_head;
    int index = 0;
    while (cur != NULL) {
        if (cur->value == val) {
            linked_list_append(result_head, index);
        }
        cur = cur->next;
        index++;
    }
    return result_head;
}
