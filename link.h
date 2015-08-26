#ifndef _LINK_H
#define _LINK_H

#include "objects.h"

typedef struct {
	int id;
   struct Object *o;
   Item * next;
}Item;

void listInit(Item *head);
void listAdd(Item *head);
void listRemove(Item *head);

#endif