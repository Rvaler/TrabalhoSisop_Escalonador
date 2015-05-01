#include "../include/mthread.h"
#include "../include/mdata.h"
#include <stdio.h>
#include <stdlib.h>

int isEmpty(TCB_t *tcbQueue){
     if (tcbQueue == NULL)
        return 1;
     else
        return 0;
}

void printQueue(TCB_t *tcbQueue){
    TCB_t* ptAux = tcbQueue;        //pointer to go trough queue

    if (isEmpty(ptAux)){
        printf("Empty\n");
    } else {
        for (; ptAux!=NULL; ptAux=ptAux->next)
            printf(" %d \n",ptAux->tid);
        //printf("\n");
    }

}

void printQueueReverse(TCB_t *tcbQueue){
    TCB_t* ptAux = tcbQueue;        //pointer to go trough queue

    if (isEmpty(ptAux)){
        printf("Empty\n");
    } else {
        //skip to end
        while(ptAux->next != NULL)
            ptAux = ptAux->next;

        for (; ptAux!=NULL; ptAux=ptAux->prev)
            printf(" %d \n",ptAux->tid);
        //printf("\n");
    }

}

TCB_t* enqueue(TCB_t *tcbQueue, TCB_t *tcbData){

    //TCB_t *ptAuxPrevious = NULL; //auxiliar pointer to precious
    TCB_t* ptAux = tcbQueue;        //pointer to go trough queue

    if (isEmpty(ptAux)){
        tcbQueue = tcbData; //set first position
        tcbQueue->next = NULL;
        return tcbQueue;
    } else {
        while(ptAux->next != NULL){ //cycle to the last element
            ptAux = ptAux->next;
        }

        //set the next of the last element (a new one)
        tcbData->next = NULL;
        ptAux->next = tcbData;
        //adjust previous of last element
        tcbData->prev = ptAux;

        return tcbQueue;

    }

}

//remove first element from queue and update queue, the dequeued element is returned in tcbData
TCB_t* dequeue(TCB_t *tcbQueue, TCB_t **tcbData){
    if (tcbQueue == NULL){
        return NULL;
    }
    //recovers the first element
    if (tcbQueue != NULL)
        *tcbData = tcbQueue;

    //update queue
    tcbQueue = tcbQueue->next;

    //only after updating the return data, the next and prev of that returning data can be NULLED
    (*tcbData)->prev = NULL;
    (*tcbData)->next = NULL;

    //if queue is now empty (was of only one element before) there is no need to update prev
    if(tcbQueue == NULL)
        return NULL;

    //removes reference to previous, as it no longer exists.
    tcbQueue->prev = NULL;

    //return pointer to queue
    return tcbQueue;
}

waitingStruct_t* removeThread(waitingStruct_t *wqueue, int freedTID, waitingStruct_t **wdata){
    if (wqueue == NULL){
        return NULL;
    }

    if (wqueue->waitedThreadTid == freedTID ){ //primeiro elemento é hit

        //recovers the first element
        *wdata = wqueue;

        //update queue
        wqueue = wqueue->next;

        //only after updating the return data, the next and prev of that returning data can be NULLED
        (*wdata)->next = NULL;


        return wqueue;
    }
    else {
        waitingStruct_t* ptAux = wqueue->next;
        waitingStruct_t* ptAuxPrev = wqueue;
        //loop do segundo em diante
        for (; ptAux!=NULL; ptAux=ptAux->next){
            //ptAuxPrev = ptAuxPrev->next;

            if (ptAux->waitedThreadTid == freedTID ){
                //recovers the first element
                *wdata = ptAux;

                //update queue
                ptAuxPrev->next = ptAux->next;

                //only after updating the return data, the next and prev of that returning data can be NULLED
                (*wdata)->next = NULL;


                return wqueue;
            }



            //update prev for next step
            ptAuxPrev = ptAuxPrev->next;
        }

        //nothing found
        return wqueue;
    }
}

waitingStruct_t* pushThread(waitingStruct_t *wqueue, waitingStruct_t *wdata){
    if(wqueue == NULL){
        wqueue = wdata;

        wqueue->next = NULL;
        return wqueue;
    }else{
        waitingStruct_t* ptAux = wqueue;

        while(ptAux->next != NULL){
            ptAux = ptAux->next;
        }

        wdata->next = NULL;
        ptAux->next = wdata;
        return wqueue;
    }
}

void printWaitingList(waitingStruct_t *wqueue){
    waitingStruct_t* ptAux = wqueue;        //pointer to go trough queue
    printf("\n-------waitingList---------\n");
    if (wqueue == NULL){
        printf("\nEmpty Waiting List\n");
    } else {
        for (; ptAux!=NULL; ptAux=ptAux->next){
            printf("\nwaitedThread: %d ",ptAux->waitedThreadTid);
            //TCB_t *teste = ptAux->blockedThread;
            printf("\nblockedThread: %d ",ptAux->blockedThread->tid);
        }
        //printf("\n");
    }

}


/*
waitingStruct_t* pushThread(waitingStruct_t *wqueue, waitingStruct_t *wdata){
    waitingStruct_t* ptAux = wqueue;

    if (wqueue == NULL){
        wqueue = wdata;
        wqueue->next = NULL;
        return wqueue;
    }else{
        while(ptAux->next != NULL){
            ptAux = ptAux->next;
        }

        wdata->next = NULL;

        ptAux->next = wdata;

        return wqueue;
    }
}

void printWaitingList(waitingStruct_t *wqueue){
    waitingStruct_t* ptAux = wqueue;        //pointer to go trough queue

    if (wqueue == NULL){
        printf("Empty Waiting List\n");
    } else {
        for (; ptAux!=NULL; ptAux=ptAux->next)
            printf("\nwaitedThread: %d \n",ptAux->waitedThreadTid);
            printf("\nblockedThread: %d \n",ptAux->blockedThreadTid);
        //printf("\n");
    }

}
*/



