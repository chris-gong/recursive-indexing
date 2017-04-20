#ifndef LINKEDLIST_H
#define LINKEDLIST_H

// #include "redblack.h"
// Not needed anymore, since now we have a red black tree of
// linked lists, and not vice versa
#include "comparestrings.h" // don't forget this

typedef struct _linkedlist linkedlist;
typedef struct _llnode llnode;

struct _linkedlist {
  llnode *head;
};


// the node definition
struct _llnode {
  llnode *next;
  char *filename;
  int count;
};

linkedlist *initLinkedList(linkedlist *);

// Returns the number of nodes in a list
int length(linkedlist *);

void traverse(linkedlist *);

void llinsert(linkedlist *, char *);

void push(linkedlist *, char *);

llnode *mergeSort(llnode *);

llnode *getMiddleNode(llnode *);

llnode *merge(llnode *, llnode *);
#endif
