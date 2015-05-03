
/*
 * mdata.h: arquivo de inclusão de uso apenas na geração da libmthread
 *
 * Esse arquivo pode ser modificado. ENTRETANTO, deve ser utilizada a TCB fornecida.
 *
 */

#ifndef __mdata__
#define __mdata__

#include <ucontext.h>

#define CREATION_STATE 0
#define READY_STATE 1
#define RUNNING_STATE 2
#define BLOCKED_STATE 3
#define END_STATE 4

#define HIGH_PRIORITY 0
#define MEDIUM_PRIORITY 1
#define LOW_PRIORITY 2

#define FREE_MUTEX 0
#define OCCUPIED_MUTEX 1


/* NÃO ALTERAR ESSA struct */
typedef struct TCB {
	int  tid;		// identificador da thread
	int  state;	// estado em que a thread se encontra
					// 0: Criação; 1: Apto; 2: Execução; 3: Bloqueado e 4: Término
	int  prio;		// prioridade da thread (0:alta; 1: média, 2:baixa)
	ucontext_t*   context;	// contexto de execução da thread (SP, PC, GPRs e recursos)
	struct TCB   *prev;		// ponteiro para o TCB anterior da lista
	struct TCB   *next;		// ponteiro para o próximo TCB da lista
} TCB_t;


// Struct responsable to implement the waiting threads
typedef struct waitingStruct {
    int waitedThreadTid;
    TCB_t *blockedThread;
    struct waitingStruct *next;
} waitingStruct_t;

#endif



//priority lists functions
void printQueue(TCB_t *tcbQueue);
void printQueueReverse(TCB_t *tcbQueue);
int isEmpty(TCB_t *tcbQueue);
TCB_t* enqueue(TCB_t *tcbQueue, TCB_t *tcbData);
TCB_t* dequeue(TCB_t *tcbQueue, TCB_t **tcbData);

//waiting Structure functions
void printWaitingList(waitingStruct_t *wqueue);
waitingStruct_t* pushThread(waitingStruct_t *wqueue, waitingStruct_t *wdata);
waitingStruct_t* removeThread(waitingStruct_t *wqueue, int freedTID, waitingStruct_t **wdata);



