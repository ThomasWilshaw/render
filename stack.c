#include <stdio.h>
#include<stdlib.h> /*for dynamic allocation*/
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
	
	stackP->contents[++stackP->top] = element;
}

stackElementT stackPop(stackT *stackP)
{
	if(stackIsEmpty(stackP)){
		fprintf(stderr, "Can't pop stack, stack is empty.");
		exit(EXIT_FAILURE);
	}
	
	return stackP->contents[stackP->top--];
}

int stackIsEmpty(stackT *stackP)
{
	return stackP->top < 0;
}

int stackIsFull(stackT *stackP)
{
	return stackP->top >= stackP->maxSize - 1;
}