/* CMPT 332
* Assignment 2
* Group 38
* Tanner Laroque, YJE740, 11122821
* Melvin Lugo, 11279686, mel196
*/

#ifndef LIST_H_
#define LIST_H_

#include <stdint.h>

#define MIN_LISTS 5
#define MIN_NODES 20

#define LIST_PTR_TRANSLATOR()        {int r = 0; \
    uint64_t hashNum = (uint64_t)list % numLists;\
    while(translatorMap[hashNum].allocationNumber != list){\
        hashNum++;\
        if(hashNum >= numLists){\
            hashNum = 0;\
            r++;\
            }\
        if(r == 2){\
            printf("Unable to find list: %lu\n\n", (uint64_t) list);\
            break;\
        }  \
    }\
    if(r==2){\
        listPtr = NULL;\
    }\
    listPtr = translatorMap[hashNum].list;\
    }


typedef struct NODE{
    struct NODE* prev;
    struct NODE* next;
    void* item;
} NODE;

typedef struct LIST{
    NODE* head;
    NODE* tail;
    NODE* curr;
    uint8_t size;
    struct LIST* next;
} LIST;

typedef struct HashTable{
    LIST *list;
    LIST *allocationNumber;
} HashTable;



LIST * ListCreate();
NODE * CreateNode();

int ListCount(LIST* list);
void *ListSearch(LIST* list, int (*comparator)(void*, void*), 
    void* comparisonArg);

int ListAdd(LIST* list, void* item);
int ListInsert(LIST* list, void* item);
int ListAppend(LIST* list, void* item);
int ListPrepend(LIST* list, void* item);

void *ListFirst(LIST* list);
void *ListLast(LIST* list);
void *ListNext(LIST* list);
void *ListPrev(LIST* list);
void *ListCurr(LIST* list);

void *ListRemove(LIST* list);
void ListConcat(LIST* list1, LIST* list2);
void ListFree(LIST* list, void(*itemFree)());
void *ListTrim(LIST* list);

#endif
