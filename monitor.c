/* CMPT 332
* Assignment 4
* Group 38
* Tanner Laroque, YJE740, 11122821
* Melvin Lugo, 11279686, mel196
*/

#include <rtthreads.h>
#include <RttCommon.h>
#include <stdio.h>
#include <stdlib.h>
#include <list.h>

#define SUCCESS 1
#define FAILURE 0
/*Global data:
Lists for each CV and an enterq and an urgentq
Lists make it easy to debug what is going on and give you practice
with list operations.*/
typedef struct threadData{
    RttThreadId threadID;
    int message;
    int CV;
}threadData;

LIST **ListOfCV;
LIST *ListMonEnter;
LIST *ListUrgency;
RttThreadId serverID;
int MonitorOccupied;



RTTTHREAD RttMonEnter(){
    RttThreadId threadID;
    threadData threadData;
    unsigned int reply;
    int status;
    unsigned int replyLen;
    replyLen = sizeof(reply);
    
    
    threadID = RttMyThreadId();
    threadData.threadID = threadID;
    threadData.message = 1;

    status = RttSend(serverID, &threadData, 
        sizeof(threadData), &reply, &replyLen);
    
    if (status == RTTFAILED) {
        printf("Failed to send message MonEnter\n");
    } else if (status == RTTNOSUCHTHREAD) {
        printf("No such thread exists MonEnter\n");
    }
}

RTTTHREAD RttMonLeave(){
    RttThreadId threadID;
    threadData threadData;
    unsigned int reply;
    int status;
    unsigned int replyLen;
    replyLen = sizeof(reply);
    
    
    threadID = RttMyThreadId();
    threadData.threadID = threadID;
    threadData.message = 2;

    status = RttSend(serverID, &threadData, 
        sizeof(threadData), &reply, &replyLen);
    
    if (status == RTTFAILED) {
        printf("Failed to send message MonLeave \n");
    } else if (status == RTTNOSUCHTHREAD) {
        printf("No such thread exists MonLeave \n");
    }
    /*don't know what to do with reply yet*/
}

RTTTHREAD RttMonWait(int CV){
    RttThreadId threadID;
    threadData threadData;
    unsigned int reply;
    int status;
    unsigned int replyLen;
    replyLen = sizeof(reply);
    
    
    threadID = RttMyThreadId();
    threadData.threadID = threadID;
    threadData.message = 3;
    threadData.CV = CV;

    status = RttSend(serverID, &threadData, 
        sizeof(threadData), &reply, &replyLen);
    
    if (status == RTTFAILED) {
        printf("Failed to send message MonWait\n");
    } else if (status == RTTNOSUCHTHREAD) {
        printf("No such thread exists MonWait\n");
    }
    /*don't know what to do with reply yet*/
}

RTTTHREAD RttMonSignal(int CV){
    RttThreadId threadID;
    threadData threadData;
    unsigned int reply;
    int status;
    unsigned int replyLen;
    replyLen = sizeof(reply);
    
    
    threadID = RttMyThreadId();
    threadData.threadID = threadID;
    threadData.message = 4;
    threadData.CV = CV;

    status = RttSend(serverID, &threadData, 
        sizeof(threadData), &reply, &replyLen);
    
    if (status == RTTFAILED) {
        printf("Failed to send message MonSignal\n");
    } else if (status == RTTNOSUCHTHREAD) {
        printf("No such thread exists MonSignal\n");
    }
    /*don't know what to do with reply yet*/
}

RTTTHREAD RttMonServer(){
    /*serverID = RttMyThreadId();*/
    printf("server made\n");
    
    while (1){
        int reply;
        RttThreadId recievingThreadID;
        threadData recievingThreadData;
        u_int len = sizeof(recievingThreadData);
        reply = SUCCESS;
        RttReceive(&recievingThreadID, &recievingThreadData, &len);

        switch (recievingThreadData.message)
        {
        case 1:
            /*ENTER*/
            if (MonitorOccupied == 1){
                ListAdd(ListMonEnter, &recievingThreadData);
            }
            else{
                MonitorOccupied = 1;
                RttReply(recievingThreadData.threadID, &reply, sizeof(int));
            }
            break;

        case 2:
            /*LEAVE*/
            reply = SUCCESS;
            if (ListCount(ListUrgency) > 0){
                threadData *threadData;
                RttThreadId threadID;
                threadData = ListFirst(ListUrgency);
                threadID = threadData->threadID;
                ListRemove(ListUrgency);
                RttReply(threadID, &reply, sizeof(int));
            }
            else if (ListCount(ListMonEnter) > 0){
                threadData *threadData;
                RttThreadId threadID;
                threadData = ListFirst(ListMonEnter);
                threadID = threadData->threadID;
                ListRemove(ListMonEnter);
                RttReply(threadID, &reply, sizeof(int));       
            }
            else{
                MonitorOccupied = 0;
                RttReply(recievingThreadID, &reply, sizeof(int));
            }                
            break;

        case 3:
            /*WAIT*/
            reply = SUCCESS;
            ListAdd(ListOfCV[recievingThreadData.CV], &recievingThreadData);
            /*add to list*/
            if (ListCount(ListUrgency) > 0){
                threadData *threadData;
                RttThreadId threadID;
                threadData = ListFirst(ListUrgency);
                threadID = threadData->threadID;
                ListRemove(ListUrgency);
                RttReply(threadID, &reply, sizeof(int));
            }
            else if (ListCount(ListMonEnter) > 0){
                threadData *threadData;
                RttThreadId threadID;
                threadData = ListFirst(ListMonEnter);
                threadID = threadData->threadID;
                ListRemove(ListMonEnter);
                RttReply(threadID, &reply, sizeof(int));       
            }
            else{
                MonitorOccupied = 0;
                RttReply(recievingThreadID, &reply, sizeof(int));
            }        
            break;

        case 4:
            /*SIGNAL*/
            if (ListCount(ListOfCV[recievingThreadData.CV]) > 0){
                threadData *threadData;
                reply = SUCCESS;
                threadData = ListFirst(ListOfCV[recievingThreadData.CV]);
                ListRemove(ListOfCV[recievingThreadData.CV]);
                ListAdd(ListUrgency, &threadData);
                RttReply(recievingThreadID, &reply, sizeof(int));
            }
            else{
                reply = FAILURE;
                RttReply(recievingThreadID, &reply, sizeof(int));
            }
            
            break;

        default:
            /*well shit it doesn't work*/
            break;
        }
        
    }
}


void RttMonInit(int numConds){
    RttSchAttr 	attr;
    int temp;
    MonitorOccupied = 0;
    ListMonEnter = ListCreate();
    ListUrgency = ListCreate();
    ListOfCV = (LIST**)malloc(numConds*sizeof(LIST*));
    attr.startingtime = RTTZEROTIME;
    attr.priority = RTTNORM;
    attr.deadline = RTTNODEADLINE;    
    {int i;
    for (i = 0; i < numConds; i++){
        ListOfCV[i] = ListCreate();
    }
    }
    
   /*create processes for each function*/
    temp = RttCreate(&serverID, 
        (void(*)()) RttMonServer,65536,"1", (void *) 1000, 
                attr, RTTUSR );
    if (temp == RTTFAILED) perror("RttMonServer create\n");
}

