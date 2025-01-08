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

/* returns pointer to first item in list */
void *ListFirst(LIST* list){

    LIST *listPtr;

    LIST_PTR_TRANSLATOR()

    if(listPtr->head == NULL){
        return NULL;
    }

    listPtr->curr = listPtr->head;

    return listPtr->curr->item;
}

/* returns pointer to last item in list */
void *ListLast(LIST* list){

    LIST *listPtr;

    LIST_PTR_TRANSLATOR()

    if(listPtr == NULL || listPtr->tail == NULL){
        return NULL;
    }

    listPtr->curr = listPtr->tail;

    return listPtr->tail->item;
}

/* returns pointer to next item in list */
void *ListNext(LIST* list){

    LIST *listPtr;

    LIST_PTR_TRANSLATOR()

    if(listPtr->curr == NULL || listPtr->curr->next == NULL){
        return NULL;
    }

    listPtr->curr = listPtr->curr->next;

    return listPtr->curr->item;
}

/* returns number of items in the list */
int ListCount(LIST* list){

    LIST *listPtr;

    LIST_PTR_TRANSLATOR()

    if(listPtr == NULL){
        return -1;
    }

    return listPtr->size;
}

/* returns pointer to previous item in list */
void *ListPrev(LIST* list){

    LIST *listPtr;

    LIST_PTR_TRANSLATOR()

    if(listPtr->curr == NULL || listPtr->curr->prev == NULL){
        return NULL;
    }

    listPtr->curr = listPtr->curr->prev;

    return listPtr->curr->item;
}

/* returns pointer to current item in list */
void *ListCurr(LIST* list){

    LIST *listPtr;

    LIST_PTR_TRANSLATOR()

    if(listPtr->curr == NULL){
        return NULL;
    }

    return listPtr->curr->item;
}

