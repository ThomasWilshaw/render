#ifndef _STACK_H
#define _STACK_H
#include "matrix.h"

/*change char to whatever wanted in stack*/
typedef Matrix stackElementT;

typedef struct {
	stackElementT *contents;
	int maxSize;
	int top;
} stackT;

/*Initialize stack
 *Starts empty 
 *Usage: stackInit(&stack, maxSize);
 */
void stackInit(stackT *stackP, int maxSize);

/*Frees the stack from memory
 *Usage stackDestroy(&stack);
 */
void stackDestroy(stackT *stackP);

/*Push and pop into stack
 *Usage stackPush(&stack, element); stackPop(&stack); 
 */
void stackPush(stackT *stackP, stackElementT element);
stackElementT stackPop(stackT *stackP);

/*Stack tests
 *Usage: if(stackIsEmpty(&stack))...
 */
 int stackIsEmpty(stackT *stackP);
 int stackIsFull(stackT *stackP);
#endif