/* CMPT 332
* Assignment 2
* Group 38
* Tanner Laroque, YJE740, 11122821
* Melvin Lugo, 11279686, mel196
*/

#include <list.h>
#include <stdlib.h>
#include <stdint.h>

#include <stdio.h>

extern HashTable *translatorMap;

extern LIST * lists;
extern NODE * nodes;
extern uint64_t numLists;


/* Adds item to list after current item and makes it the current item */
int ListAdd(LIST* list, void* item){

    LIST *listPtr;
    
    NODE* newNode = CreateNode();
    newNode->item = item;

   LIST_PTR_TRANSLATOR()

    if(listPtr == NULL){
        return -1;
    }

    if(listPtr->curr != NULL){
        newNode->next = listPtr->curr->next;
        newNode->prev = listPtr->curr;

        if(listPtr->curr->next != NULL){
            listPtr->curr->next->prev = newNode;
        }

        listPtr->curr->next = newNode;
    }
    else{
        newNode->next = NULL;
        newNode->prev = NULL;
        listPtr->head = newNode;
        listPtr->tail = newNode;
    }

    if(listPtr->curr == listPtr->tail){
        listPtr->tail = newNode;
    }

    listPtr->curr = newNode;
    listPtr->size++;

    return 0;
}

/* Adds item before current item and makes it the current item */
int ListInsert(LIST* list, void* item){

    LIST *listPtr;

    NODE* newNode = CreateNode();
    newNode->item = item;

    LIST_PTR_TRANSLATOR()

    if(listPtr == NULL){
        return -1;
    }

    if(listPtr->curr != NULL){
        newNode->next = listPtr->curr;
        newNode->prev = listPtr->curr->prev;
        listPtr->curr->prev = newNode;
    }
    else{
        newNode->prev = listPtr->tail;
        newNode->next = NULL;
        listPtr->tail = newNode;
        listPtr->head = newNode;
    }

    if(listPtr->curr == listPtr->head){
        listPtr->head = newNode;
    }

    listPtr->curr = newNode;
    listPtr->size++;

    return 0;
}

/* Adds item to end of list and makes it the current item */
int ListAppend(LIST* list, void* item){

    LIST *listPtr;

    NODE* newNode = CreateNode();
    newNode->item = item;

    LIST_PTR_TRANSLATOR()

    if(listPtr == NULL){
        return -1;
    }

    if(listPtr->tail != NULL){
        newNode->prev = listPtr->tail;
        newNode->next = NULL;
        listPtr->tail->next = newNode;
    }
    else{
        newNode->next = NULL;
        newNode->prev = listPtr->head;
        listPtr->head = newNode;
    }

    listPtr->tail = newNode;
    listPtr->curr = newNode;
    listPtr->size++;

    return 0;
}

/* Adds item to front of list and makes it the current item */
int ListPrepend(LIST* list, void* item){

    LIST *listPtr;

    NODE* newNode = CreateNode();
    newNode->item = item;

    LIST_PTR_TRANSLATOR()

    if(listPtr == NULL){
        return -1;
    }

    if(listPtr->head != NULL){
        newNode->next = listPtr->head;
        newNode->prev = NULL;
        listPtr->head->prev = newNode;
    }
    else{
        newNode->next = listPtr->tail;
        newNode->prev = NULL;
        listPtr->tail = newNode;
    }

    listPtr->head = newNode;
    listPtr->curr = newNode;
    listPtr->size++;
    
    return 0;
}

