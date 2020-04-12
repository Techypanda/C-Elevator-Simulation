/*
    ----- NOTICE -----
    SUBMITTED FOR UCP 2019 ASSIGNMENT SEMESTER 2
    AUTHOR: JONATHAN WRIGHT
    DATE: 8/10/2019
*/
#include "list.h"
#include <stdlib.h>
#include <stdio.h>
/*
    Purpose: This file is about manipulating linked lists, it was originally written for prac 7, it is a
    doubly linked double ended generic linked list with a size.
    Author: Jonathan Wright
    Date: 8/10/2019
*/

/*
    Purpose: To create and export a empty linked list.
    Date: 8/10/2019
    Imports: None
    Exports: list (Empty)
*/
linkedList* createLinkedList()
{
    linkedList* list;
    list = malloc(sizeof(linkedList));
    list->size = 0;
    list->head = NULL;
    list->tail = NULL;
    return list;
}
/*
    Purpose: To insert generic data at the start of the linked list.
    Date: 8/10/2019
    Imports: list and data
    Exports: none
*/
void insertStart(linkedList* list, void* inData)
{
    listNode* node;
    node = (listNode*)malloc(sizeof(listNode));
    node->data = inData;
    list->size += 1;
    if (list->head == NULL)
    { /* NO HEAD */
        list->head = node;
        list->tail = node;
        node->next = NULL;
        node->prev = NULL;
    }
    else
    { /* HEAD */
        list->head->prev = node;
        node->next = list->head;
        node->prev = NULL;
        list->head = node;
    }
}
/*
    Purpose: Remove the data at head of linked list and return it.
    Date: 8/10/2019
    Imports: list
    Exports: data (removed from start)
*/
void* removeStart(linkedList* list)
{
    listNode* removed = NULL;
    void* outData = NULL;
    if (list->head != NULL)
    {
        removed = list->head;
        list->head = list->head->next;
        list->head->prev = NULL;
        list->size -= 1;
        outData = removed->data;
        free(removed);
    }
    else
    {

    }
    return outData; /* NOT FREED */
}
/*
    Purpose: Insert data at tail of linked list.
    Date: 8/10/2019
    Imports: list, data
    Exports: None
*/
void insertLast(linkedList* list, void* inData)
{
    listNode* node;
    node = (listNode*)malloc(sizeof(listNode));
    node->data = inData;
    list->size += 1;
    node->next = NULL;
    if (list->tail == NULL)
    { /* NO TAIL */
        list->head = node;
        list->tail = node;
        node->next = NULL;
        node->prev = NULL;
    }
    else
    { /* TAIL */
        list->tail->next = node;
        node->prev = list->tail;
        list->tail = node;
        node->next = NULL;
    }
}
/*
    Purpose: Remove data at tail of linked list.
    Date: 8/10/2019
    Imports: list
    Exports: data (removed data)
*/
void* removeLast(linkedList* list)
{
    void* outData = NULL;
    listNode* removed = NULL;
    if (list->tail == NULL)
    {
        /* NO TAIL */
    }
    else
    {
        removed = list->tail;
        if (list->tail->prev == NULL)
        {
            list->head = NULL;
            list->tail = NULL;
            list->size -= 1;
        }
        else
        {
            list->tail = list->tail->prev;
            list->tail->next = NULL;
            list->size -= 1;
        }
        outData = removed->data;
        free(removed);
    }
    return outData;
}
/*
    Purpose: To print contents of linked list.
    Date: 8/10/2019
    Imports: list
    Exports: None
*/
void printLinkedList(linkedList* list)
{
    listNode* curr;
    curr = list->head;
    while (curr != NULL)
    {
        printf("%s,",(char*)curr->data);
        curr = curr->next;
    }
    printf("\n");
}
/*
    Purpose: To free linked list.
    Date: 8/10/2019
    Imports: list
    Exports: None
*/
void freeLinkedList(linkedList* list, void (*freeMethod)(void* data))
{
    listNode* curr; listNode* prev;
    curr = list->head;
    while (curr != NULL)
    {
        prev = curr;
        curr = curr->next;
        (*freeMethod)(prev->data /* Return A Game */);
        free(prev);
    }
    free(list);
    list = NULL;
}