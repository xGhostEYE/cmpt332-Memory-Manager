/* CMPT 332
* Assignment 2
* Group 38
* Tanner Laroque, YJE740, 11122821
* Melvin Lugo, 11279686, mel196
*/

#include <list.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

LIST* internalListsPtrs;
NODE* nodes;
HashTable *translatorMap;

NODE *freeNodePtr;
LIST *freeListPtr;

uint64_t intialized = 0;
uint64_t numLists = MIN_LISTS;
uint64_t numNodes = MIN_NODES;

uint64_t emptyLists;
uint64_t emptyNodes;


int ReleaseNode(){

    NODE *temp;
    uint64_t i;
    for(i = numNodes / 2; i < numNodes; i++){
        if(nodes[i].item != NULL){
            while(freeNodePtr > &nodes[numNodes / 2]){
                freeNodePtr = freeNodePtr->next; 
            }
            temp = freeNodePtr;
            freeNodePtr = freeNodePtr->next;

            temp->item = nodes[i].item;
            temp->next = nodes[i].next;
            temp->prev = nodes[i].prev;

            nodes[i].item = NULL;
            nodes[i].next = NULL;
            nodes[i].prev = NULL;

            nodes[i].prev->next = temp;
            nodes[i].next->prev = temp;

        }
    }
    nodes = (NODE *)realloc(nodes, sizeof(NODE) * numNodes / 2);
    
    return 0;
}


int ReleaseList(LIST *list){

    uint64_t hashNum = (uint64_t)list % numLists;
    uint8_t r = 0;
    HashTable *temp;

    if(list == NULL){
        printf("Error in ReleaseList, invalid list\n");
        return -1;
    }

    while(translatorMap[hashNum].list != list){
        hashNum++;
        if(hashNum >= numLists){
            hashNum = 0;
            r++;
        } else if(r == 2){
            printf("Error in ReleaseList, invalid list\n");
            return -1;
        }
    }

    translatorMap[hashNum].list = NULL;
    translatorMap[hashNum].allocationNumber = 0;

    if(numLists > MIN_LISTS && emptyLists > numLists / 2){
        temp = (HashTable *)calloc(numLists / 2, sizeof(HashTable));

        internalListsPtrs = (LIST *)realloc(internalListsPtrs, 
                            sizeof(LIST) * numLists / 2);

        {
            uint64_t i;
            uint64_t hashNum;
        
            for(i = 0; i < numLists; i++){

                    hashNum = (uint64_t)translatorMap[i].allocationNumber
                                % numLists/2;

                    if(temp[hashNum].allocationNumber == 0){
                        memmove(&temp[hashNum], &translatorMap[i], 
                                sizeof(HashTable));

                    } else {

                        uint64_t j = hashNum + 1;
                        while(temp[j].allocationNumber != 0){
                            j++;
                            if(j >= numLists){
                                j = 0;
                            }
                            
                            if(j == hashNum){
                                printf("Error in List Release, \
                                        invalid list\n");
                                return -1;
                            }
                        }
                        memmove(&temp[j], &translatorMap[i], 
                                sizeof(HashTable));
                    }
            }
        }

        free(translatorMap);

        translatorMap = temp;
        numLists /= 2;
        emptyLists -= numLists / 2;
    }

    return 0;
}

/* extra function to allocate nodes from pool */
NODE * CreateNode(){

    NODE *newNode;

    if(freeNodePtr == NULL){
        
        numNodes *= 2;
        nodes = (NODE *)realloc(nodes, sizeof(NODE) * numNodes);
        
        emptyNodes += numNodes / 2;
        
        {uint64_t i;
        for(i = numNodes / 2; i < numNodes - 1; i++){
            nodes[i].next = &nodes[i+1];
            nodes[i].prev = NULL;
        }
        }
        freeNodePtr = &nodes[numNodes / 2];
    }

    emptyNodes--;

    newNode = freeNodePtr;
    freeNodePtr = freeNodePtr->next;

    newNode->next = NULL;

    return newNode;
}


/* Allocate an new empty list form the list pool
 * Manage the doubling of the list pool
 */
LIST *ListCreate(){
    
    LIST *newList;
    uint64_t hashNum;

    if(!intialized){

        internalListsPtrs = (LIST *)malloc(sizeof(LIST) * MIN_LISTS);
        nodes = (NODE *)malloc(sizeof(NODE) * MIN_NODES);

        translatorMap = (HashTable *)calloc(MIN_LISTS, sizeof(HashTable));

        emptyLists = MIN_LISTS;
        emptyNodes = MIN_NODES;

        {int i;
        for(i = 0; i < MIN_LISTS - 1; i++){
            internalListsPtrs[i].head = NULL;
            internalListsPtrs[i].tail = NULL;
            internalListsPtrs[i].curr = NULL;
            internalListsPtrs[i].size = 0;
            internalListsPtrs[i].next = &internalListsPtrs[i+1];
        }
        }
        freeListPtr = &internalListsPtrs[0];

        {int i;
        for(i = 0; i < MIN_NODES - 1; i++){
            nodes[i].next = &nodes[i+1];
            nodes[i].prev = NULL;
        }
        }
        freeNodePtr = &nodes[0];

        intialized = 1;
    }


    if(freeListPtr == NULL){
        numLists *= 2;

        internalListsPtrs = (LIST *)realloc(internalListsPtrs, 
                            sizeof(LIST) * numLists);

        {
            uint64_t i;
            uint64_t hashNum;
            HashTable *temp = (HashTable *)calloc(numLists, 
                                sizeof(HashTable));
        
            for(i = 0; i < numLists/2; i++){

                    hashNum = (uint64_t)translatorMap[i].allocationNumber
                                % numLists;

                    if(temp[hashNum].allocationNumber == 0){
                        memmove(&temp[hashNum], &translatorMap[i], 
                                sizeof(HashTable));

                    } else {

                        uint64_t j = hashNum + 1;

                        while(temp[j].allocationNumber != 0){
                            j++;
                            if(j >= numLists){
                                j = 0;
                            }
                            
                            if(j == hashNum){
                                printf("Error1 in List Create, \
                                        invalid list\n");
                                return NULL;
                            }
                        }
                        memmove(&temp[j], &translatorMap[i], 
                                sizeof(HashTable));
                    }
            }

            free(translatorMap);
            translatorMap = temp;
        }

        emptyLists += numLists / 2;

        {uint64_t i;
        for(i = numLists / 2; i < numLists-1; i++){
            internalListsPtrs[i].head = NULL;
            internalListsPtrs[i].tail = NULL;
            internalListsPtrs[i].curr = NULL;
            internalListsPtrs[i].size = 0;
            internalListsPtrs[i].next = &internalListsPtrs[i+1];
        }
        }
        internalListsPtrs[numLists - 1].next = NULL;
        freeListPtr = &internalListsPtrs[numLists / 2];
    }

    emptyLists--;

    newList = freeListPtr;
    freeListPtr = freeListPtr->next;

    hashNum = (uint64_t)newList % numLists;

    if(translatorMap[hashNum].allocationNumber == 0){
        translatorMap[hashNum].list = newList;
        translatorMap[hashNum].allocationNumber = newList;

    } else {

        uint64_t i = hashNum + 1;
        while(translatorMap[i].allocationNumber != 0){
            i++;
            if(i >= numLists){
                i = 0;
            }
            if(i == hashNum){
                {
                    uint64_t j;
                    for(j = 0; j < numLists; j++){
                        printf(" #%lu: %lu\n", j, 
                                (uint64_t)translatorMap[j].allocationNumber);
                    }
                }
                printf("Error2 in List Create, invalid list\n");
                return NULL;
            }
        }
        translatorMap[i].list = newList;
        translatorMap[i].allocationNumber = newList;
    }

    return newList;
}

/* Search a list 
 * Returns the first item in the list that matches the comparator function
 */
void *ListSearch(LIST* list, int (*comparator)(void*, void*), 
    void* comparisonArg){

    if (list == NULL){
        printf("Error in ListSearch, invalid list\n");
        return NULL;
    }
    if (comparator == NULL){
        printf("Error in ListSearch, invalid comparator\n");
        return NULL;
    }
    if (comparisonArg == NULL){
        printf("Error in ListSearch, invalid comparisonArg\n");
        return NULL;
    }

    while(list->curr != NULL){
        if(comparator(list->curr->item, comparisonArg)){
            return list->curr->item;
        }
        list->curr = list->curr->next;
    }

    return NULL;
}
