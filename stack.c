#include <stdio.h>
#include <stdlib.h> /*for dynamic allocation*/
#include "stack.h"

void stackInit(stackT *stackP, int maxSize)
{
	stackElementT *newContents;
	newContents = (stackElementT *)malloc(sizeof(stackElementT) * maxSize);
	
	if(newContents == NULL){
		fprintf(stderr, "Insufficient memory to initialize stack.\n");
		exit(EXIT_FAILURE); /*Exit returning error code*/
	}
	
	stackP->contents = newContents;
	stackP->maxSize = maxSize;
	stackP->top = -1; /*Empty stack*/
}

void stackDestroy(stackT *stackP)
{
	/*clear array*/
	free(stackP->contents);
	
	stackP->contents = NULL;
	stackP->maxSize = 0;
	stackP->top = -1;
}

void stackPush(stackT *stackP, stackElementT element)
{
	if(stackIsFull(stackP)){
		fprintf(stderr, "Can't push onto stack, stack is full.");
		exit(EXIT_FAILURE);
	}
	stackP->top++;
	stackP->contents[stackP->top].rows = element.rows;
	stackP->contents[stackP->top].cols = element.cols;
	stackP->contents[stackP->top].t = element.t;
}

stackElementT stackPop(stackT *stackP)
{
	if(stackIsEmpty(stackP)){
		fprintf(stderr, "Can't pop stack, stack is empty.");
		exit(EXIT_FAILURE);
	}
	
	stackElementT r;
	r = new_matrix(stackP->contents[stackP->top].rows, stackP->contents[stackP->top].cols);
	r.t = stackP->contents[stackP->top].t;
	stackP->top--;
	
	return r;
}

int stackIsEmpty(stackT *stackP)
{
	return stackP->top < 0;
}

int stackIsFull(stackT *stackP)
{
	return stackP->top >= stackP->maxSize - 1;
}