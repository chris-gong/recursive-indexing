#include <stdlib.h>
#include <stdio.h>
#include <error.h>
#include <string.h>

// #include "redblack.h" // Not necessary to include in .c
#include "linkedlist.h"


linkedlist *initLinkedList(linkedlist *ll) {
  ll->head = NULL;

  return ll;
}

// Returns the number of nodes in a list
int length(linkedlist *list) {
  int count = 0;
  llnode *current = list->head;
  
  while (current != NULL) {
    count++;
    current = current->next;
  }
  return count;
}

void traverse(linkedlist *list) {
  llnode *current = list->head;
  
  while (current != NULL) {
    //printf("%s (%d time(s))\n", current->filename, current->count);
    current = current->next;
  }
}

int length2(linkedlist *list) {
  int count;
  llnode *current;
  for (count = 0, current=list->head; current != NULL; count++, current=current->next) {}
  return count;
}

llnode *initllnode(llnode *node, char *filename) {
  
  //newNode->filename = filename; // A relic of the past...
  /*
  int wordlen = 0;
  node->filename = malloc(wordlen+1);
  if (filename != NULL) { //this is needed for merge because previously, it didn't handle filename being null, and can't take strlen of null
    wordlen = strlen(filename);
    memcpy(node->filename, filename, wordlen);
  }*/
  //char * temp=NULL;
  int wordlen = strlen(filename);
  node->filename = (char *)malloc(sizeof(char)*wordlen+1);
  memcpy(node->filename, filename, wordlen);
  node->filename[wordlen] = '\0';
  node->next = NULL;
  node->count = 1;

  return node;

}

void llinsert(linkedlist *list, char *filename) {
  //check if linked list is empty
  if (list->head == NULL) {
    llnode *newNode = malloc(sizeof(llnode));
    newNode = initllnode(newNode, filename);
  }
  llnode *curr = list->head;
  llnode *prev = NULL;
  while (curr != NULL) {
    if (compare(curr->filename, filename) < 0){
      //update pointers
      prev = curr;
      curr = curr->next;
    }
    else if (compare(curr->filename, filename) > 0) {
      //first time we encounter a llnode in the list with a filename greater than inputted filename
      break;
    }
    else {
      //in this case, we encounter a llnode with the same filename as the inputted filename, so current increment the llnode's count	  
      curr->count++;
      return;
    }
  }
  //in the case where you insert at the front of the list
  if (prev == NULL) {
    llnode *newNode = malloc(sizeof(llnode));
    newNode = initllnode(newNode, filename);
    newNode->next = curr;
    list->head = newNode;
  }
  else {
    //in the case where new node goes in the middle or end of the list
    llnode *newNode = malloc(sizeof(llnode));
    newNode = initllnode(newNode, filename);
    newNode->next = curr;
    prev->next = newNode;
  }
}
void push(linkedlist *list, char *filename) {
  llnode *newNode = malloc(sizeof(llnode));
  newNode->filename = filename;
  newNode->next = list->head;
  list->head = newNode;
  newNode->count = 1;
}

llnode *mergeSort(llnode *head) {
  if (head == NULL || head->next == NULL) {
    return head;
  }
  else {
    llnode *middle = getMiddleNode(head);
    llnode *secondHalf = middle->next;
    middle->next = NULL;
    llnode *test = merge(mergeSort(head), mergeSort(secondHalf));
    return test;
  }
  return head;
}

llnode *getMiddleNode(llnode *head) {
  if (head == NULL) {
    return head;
  }
  llnode *slow = head; //slow traverses through the list half the speed that fast traverses through the list
  llnode *fast = head; //fast traverses through the list twice the speed that slow traverses through the list
  //the point of the loop below is to move slow one node at a time and fast two nodes at a time with each iteration
  //so that fast will be either the last or second to last node in the list while slow is the middle node of the list
  while (fast->next != NULL && fast->next->next != NULL) {
    slow = slow->next;
    fast = fast->next->next;
  }
  return slow;
}

llnode *merge(llnode *a, llnode *b) {
  llnode *dummyHead = (llnode *)malloc(sizeof(llnode));
  dummyHead = initllnode(dummyHead, ""); //dummyhead is just a node variable that is needed for reference purposes (the node after dummyHead is the actual head dummy)
  llnode *curr = dummyHead; //curr is the pointer that moves through the new merged list being created, always pointing to the last node in the new list
  //traverse through a and b until reaching the end of one of the lists, comparing elements from each to decide the order in which to add elements to the new list
  
  while (a != NULL && b != NULL) {
    if (a->count > b->count) {
      curr->next = a;
      a = a->next;
    }
    else if (a->count < b->count) {
      curr->next = b;
      b = b->next;
    }
    else {
      //if the counts are equal, then let the file names decide the order
      if (compare(a->filename, b->filename) <= 0) {
        curr->next = a;
        a = a->next;
      }
      else {
        curr->next = b;
        b = b->next;
      }
    }
    curr = curr->next;
  }
  //append the rest of the remaining list, not the list that traversed all the way through
  if (a == NULL) {
    curr->next = b;
  }
  else {
    curr->next = a;
  }
  
  return dummyHead->next;
}








