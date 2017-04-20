#ifndef REDBLACK_H
#define REDBLACK_H

#include "linkedlist.h"
#include "comparestrings.h"

enum color {RED, BLACK}; // RED == 0, BLACK == 1

typedef struct Node_ Node;
typedef struct Tree_ Tree;

struct Tree_{
  Node *root;
};

struct Node_{
  enum color col;
  char *word;
//  int count;
  linkedlist *filelist;
  Node *left, *right, *parent;
};

Tree *initTree(Tree *);

Node *initNode(Node *, enum color, char*);

void print(Node *, char *, int);

void BSTinorder(Node *);

Node *searchTree(Tree *, char *);

void printTree(Tree *);

void freeNodes(Node *);


// frees the memory allocated for the nodes and the
// words associated with them
void freeNodesInTree(Tree *);

int BSTinsert(Tree *, Node *);

void insert(Tree *, Node *);



#endif
