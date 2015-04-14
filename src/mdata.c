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
        printf("\n");
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
        printf("\n");
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
        //tcbData->prev = ptAux;

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
    //if (tcbData != NULL)
        *tcbData = tcbQueue;

    printf("\ntid da thread running dentro da func dequeue: %i\n", (*tcbData)->tid);
   //update queue
    tcbQueue = tcbQueue->next;

    //if queue is now empty (was of only one element before) there is no need to update prev
    if(tcbQueue == NULL)
        return NULL;

    //removes reference to previous, as it no longer exists.
    tcbQueue->prev = NULL;


    //return pointer to queue
    return tcbQueue;
}
