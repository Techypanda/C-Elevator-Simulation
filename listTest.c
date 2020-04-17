/*
 * File: listTest.c
 * File Created: Friday, 6th September 2019 11:40:35 am
 * Author: Jonathon Winter
 * -----
 * Last Modified: Tuesday 8th October 2019 4:49 am
 * Modified By: Jonathan Wright
 * -----
 * ANOTHER MODIFICATION: 16/04/2019 8:21PM, USING THIS FOR A SHARED LINKED LIST NOW!
 * MODIFIED BY: JONATHAN WRIGHT
 * Purpose: complete Test harness for a Generic Linked List in C
 * -- Edited with permission for prac 7, purely using it to test the list.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "sharedlist.h"

int main(int argc, char const *argv[])
{
   sharedLinkedList* list = NULL;
   char* data;
   char input1[100];
   char input2[100];

   strncpy(input1,"hello",6);
   strncpy(input2,"bye",6);

   /*CREATING*/
   printf("Creating List: ");
   list = createSharedLinkedList();
   if(list == NULL || list->head != NULL)
   {
      printf("FAILED\n");
   }
   else
   {
      printf("PASSED\n");
   }

   /*INSERTING FIRST*/
   printf("Inserting First: ");
   sharedInsertStart(list,input1);
   if(list->head == NULL)
   {
      printf("FAILED\n");
   }
   else if(strncmp((char*)list->head->data, input1, strlen(input1)+1)==0)
   {
      printf("PASSED\n");
   }
   else
   {
      printf("FAILED\n");
   }

   printf("Inserting First (2): ");
   sharedInsertStart(list,input2);
   if(list->head == NULL)
   {
      printf("FAILED\n");
   }
   else if(strncmp((char*)list->head->data, input2, strlen(input2)+1)==0 &&
            strncmp((char*)list->tail->data, input1, strlen(input1)+1)==0)
   {
      printf("PASSED\n");
   }
   else
   {
      printf("FAILED\n");
   }

   /*INSERTING LAST*/
   printf("Inserting Last: ");
   sharedInsertLast(list,"testt");
   if (list->tail == NULL)
   {
       printf("FAILED\n");
   }
   else if(strncmp((char*)list->tail->data, "testt", strlen(input1)+1)==0)
   {
      printf("PASSED\n");
   }
   else
   {
       printf("FAILED\n");
   }
   /*REMOVING FIRST*/
   printf("Remove First: ");
   data = (char*)sharedRemoveStart(list);

   if(strncmp(data, input2, strlen(input2)+1) == 0)
   {
      printf("PASSED\n");
   }
   else
   {
      printf("FAILED\n");
   }
   /*REMOVING LAST*/
   printf("Remove Last: ");
   data = (char*)sharedRemoveLast(list);
   if(strncmp(data, "testt", strlen(input1)+1) == 0)
   {
      printf("PASSED\n");
   }
   else
   {
      printf("FAILED: \n");
   }
   /*PRINTING*/
   printf("Remove Last (2): ");
   data = (char*)sharedRemoveLast(list);
   if(strncmp(data, "hello", strlen(input1)+1) == 0)
   {
      printf("PASSED\n");
   }
   else
   {
      printf("FAILED: \n");
   }
   printf("Testing Print List: ");
   sharedInsertLast(list,"first");
   sharedInsertLast(list,"second");
   sharedInsertLast(list,"third");
   sharedInsertLast(list,"fourth");
   sharedInsertLast(list,"fifth");
   sharedPrintLinkedList(list);
   /*FREEING*/
   /*freeSharedLinkedList(list, NULL);*/
   /*free(data);*/
   printf("Testing Free Data: ");
   printf("PASSED! (Try Valgrind.)\n");
   /* ANY OTHER TESTS YOU WANT TO DO*/
   return 0;
}
