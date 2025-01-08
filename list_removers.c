/* CMPT 332
* Assignment 2
* Group 38
* Tanner Laroque, YJE740, 11122821
* Melvin Lugo, 11279686, mel196
*/

#include <list.h>

#include <stdio.h>

extern HashTable *translatorMap;

extern NODE *freeNodePtr;
extern LIST *freeListPtr;

extern LIST * lists;
extern NODE * nodes;

extern uint64_t numLists;
extern uint64_t numNodes;

extern uint64_t emptyLists;
extern uint64_t emptyNodes;

int ReleaseList(LIST *list);
int ReleaseNode();

/* Removes current item from list and returns it */
void *ListRemove(LIST* list){

    LIST *listPtr;
    NODE *removeNodePtr;

    void *item;

    LIST_PTR_TRANSLATOR()

    if(listPtr->curr == NULL){
        return NULL;
    }

    item = listPtr->curr->item;

    if(listPtr->curr->prev != NULL){
        
        listPtr->curr->prev->next = listPtr->curr->next;

    } else if (listPtr->curr->next != NULL){

        listPtr->curr->next->prev = listPtr->curr->prev;

    } else if (listPtr->curr->prev == NULL ){

        listPtr->head = listPtr->curr->next;

    } else if (listPtr->curr->next == NULL){

        listPtr->tail = listPtr->curr->prev;

    }

    removeNodePtr = listPtr->curr;

    if(listPtr->curr->next == NULL && listPtr->curr->prev == NULL){
        listPtr->head = NULL;
        listPtr->tail = NULL;
        listPtr->curr = NULL;        
    } 
    
    /* add node to free node list */
    removeNodePtr->prev = NULL;
    removeNodePtr->item = NULL;
    removeNodePtr->next = freeNodePtr;
    freeNodePtr = removeNodePtr;

    listPtr->size--;
    emptyNodes++;

    if(emptyNodes / 2 > numNodes){
        ReleaseNode();
    }

    return item;
}

/* adds list2 to the end of list 1 */
void ListConcat(LIST *list1, LIST *list2){

    LIST *list = list1;
    LIST *listPtr;
    
    LIST *list1Ptr;
    LIST *list2Ptr;

    LIST_PTR_TRANSLATOR()

    if(listPtr == NULL || listPtr->head == NULL){
        return;
    }

    list1Ptr = listPtr;

    if(list1Ptr == NULL || list1Ptr->tail == NULL){
        return;
    }

    list = list2;
    LIST_PTR_TRANSLATOR()

    if(listPtr == NULL || listPtr->head == NULL){
        return;
    }

    list2Ptr = listPtr;

    if(list2Ptr == NULL || list2Ptr->head == NULL){
        return;
    }

    list1Ptr->tail->next = list2Ptr->head;
    list2Ptr->head->prev = list1Ptr->tail;
    list1Ptr->tail = list2Ptr->tail;
    list1Ptr->size += list2Ptr->size;

    /* add list 2 to head of free lists list */
    list2Ptr->next = freeListPtr;
    freeListPtr = list2Ptr;

    emptyLists++;

    if(ReleaseList(list2) == -1){
        printf("Error releasing list\n");
    }

}

/* deletes an entire list */
void ListFree(LIST *list, void(*itemFree)()){

   LIST *listPtr;

   LIST_PTR_TRANSLATOR()

   if(listPtr == NULL){
        printf("Error in ListFree, invalid list\n");
        return;
    }

    if(itemFree == NULL){
        printf("Error in ListFree, invalid itemFree\n");
        return;
    }

    listPtr->curr = listPtr->head;

    while(listPtr->curr != NULL){
        itemFree(listPtr->curr->item);

        listPtr->curr = listPtr->curr->next;

        /* add node to free node list */
        listPtr->curr->prev->next = freeNodePtr;
        freeNodePtr = listPtr->curr->prev;
    }


    /* add list to head of free lists list */
    listPtr->next = freeListPtr;
    freeListPtr = listPtr;

    emptyLists++;

    if(ReleaseList(list) == -1){
        printf("Error releasing list\n");
    }
}

/* Removes the last item from a list and returns it */
void *ListTrim(LIST *list){

    LIST *listPtr;
    void *item;

    LIST_PTR_TRANSLATOR()

    if(listPtr == NULL || listPtr->tail == NULL || listPtr->curr == NULL){
        return NULL;
    }

    listPtr->curr = listPtr->tail;

    item = ListRemove(list);

    if(numNodes / 2 > numLists){
        ReleaseNode();
    }

    return item;
}

