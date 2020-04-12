
#ifndef LIST
#define LIST
/* --- NOTICE ---
    ORIGINALLY SUBMITTED FOR UCP ASSIGNMENT 2019
    AUTHOR: JONATHAN WRIGHT
    DATE: 8/10/2019
*/
/*
    Purpose: Nodes for a linked list (Contains generic data, doubly linked (prev and next))
    Date: 8/10/2019 -- Taken From Prac 7 UCP Credit to Jonathan Wright
    Author: Jonathan Wright
*/
typedef struct listNode {
    void* data;
    struct listNode* next;
    struct listNode* prev;
} listNode;
/*
    Purpose: Linked List Structure, double ended (head and tail) and contains a size.
    Date: 8/10/2019 -- Taken from Prac 7 UCP Credit To Jonathan Wright
    Author: Jonathan Wright
*/
typedef struct linkedList {
    int size;
    listNode* head;
    listNode* tail;
} linkedList;
linkedList* createLinkedList();
void insertStart(linkedList* list, void* data);
void* removeStart(linkedList* list);
void insertLast(linkedList* list, void* data);
void* removeLast(linkedList* list);
void printLinkedList(linkedList* list);
void freeLinkedList(linkedList* list, void (*freeMethod)(void* data));
#endif