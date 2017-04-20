#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <error.h>
#include <ctype.h>

#include "redblack.h" // this includes linkedlist.h too

  // TODO figure out why the tree doesn't print right for
  // the dictionary with a lot of words


Tree *initTree(Tree * tree) {
  tree->root = NULL;
  return tree;
}

// To be run when a new word is found AT ALL
// Not every time a word is found in a new file
Node *initNode(Node *node, enum color col, char *word) {
  node->col = col;
  //node->word = word; // word might not point to malloced memory
  // word also might be temporary (as in a variable in a recursive function)
  int wordlen = strlen(word);
  node->word = malloc(wordlen+1);
  memcpy(node->word, word, wordlen);
  node->word[wordlen] = '\0';
  //printf("RB Node initialized with word %s\n", node->word);

  node->left = NULL;
  node->right = NULL;
  node->parent = NULL;
  //node->count = 1;
  //node->filelist = filelist; // filelist might not point to malloced memory
  node->filelist = malloc(sizeof(linkedlist));
  initLinkedList(node->filelist); // sets head to NULL

  // insert the filename where this word was found into the new LL


  return node;
}


void print(Node *node, char *prefix, int isTail) {
  //char *tmp = malloc(strlen(prefix) + strlen(node->word) + 1);
  //memcpy(tmp, prefix, strlen(prefix));
  //memcpy(tmp + strlen(prefix), "bla bla", strlen("bla bla"));
  // add null byte 
  if (node == NULL) return;
  
  // allocate space of length strlen(prefix) + 4 (see ternary)
  // + 1 for null byte

  char tmp[strlen(prefix) + 4 + 1]; // TODO do this with malloc
//  char *tmp = malloc(strlen(prefix) + 4 + 1);
  sprintf(tmp, "%s%s", prefix, (isTail ? "    " : "│   "));
//  printf("tmp: %s\n", tmp);
  printf("%s%s%s (%s)\n", prefix , (isTail ? "└── " : "├── ") , 
    node->word, (node->col == RED ? "red": "black"));
 
  print(node->left, tmp, 0);

  print(node->right, tmp, 1);

//  free(tmp);
}


void BSTinorder(Node *node) {
  if (node == NULL) return;
 
  // keep going left
  BSTinorder(node->left);  

  // print node
  printf("%s\n", node->word);

  // traverse the node's linked list
  traverse(node->filelist);

  // go right
  BSTinorder(node->right);

}

// searches the tree for the passed-in word
Node *searchTree(Tree *tree, char *word) {
  Node *current = tree->root;
  if (current == NULL) {
    return NULL;
  }
  int diff = compare(current->word, word); // if 1st < 2nd, will return negative
  while(current != NULL) {
    if(diff < 0) { 		// current string less than new string
      current = current->right;
    }
    else if(diff > 0) { 	// current string less than or equal to new string
      current = current->left;
    }
    else {
      //if diff == 0, word exists already
      // current->count++;
      return current;
    }
    if(current != NULL){
      diff = compare(current->word, word);
    }
  } // end while
  return NULL; // couldn't find it

}

void printTree(Tree *t) {
    print(t->root, "", 1);
}


void freeNodes(Node *node) {
  if (node == NULL) return;
  freeNodes(node->left);
  
  freeNodes(node->right);

  // free the node and the string:
  free(node->word); // TODO this assumes word was malloced
  free(node);
}


// frees the memory allocated for the nodes and the
// words associated with them
void freeNodesInTree(Tree *t) {
  freeNodes(t->root);
}


int BSTinsert(Tree *tree, Node *node) {
  // NOTE: Assumes a non-empty tree

  Node *current = tree->root;
  Node *above = NULL;
  char *str = node->word;
  int diff = compare(current->word, str); // if 1st < 2nd, will return negative
  while(current != NULL) {
    if(diff < 0) { 		// current string less than new string
      above = current;
      current = current->right;
    }
    else if(diff > 0) { 	// current string less than or equal to new string
      above = current;
      current = current->left;
    }
    else {
      //if diff == 0, word exists already
      // current->count++;
      return 0;
    }
    if(current != NULL){
      diff = compare(current->word, str);
    }
  }
  diff = compare(above->word, str);
  if(diff < 0) {
    // above->word < new string
    above->right = node;
    node->parent = above;
  } 
  else if(diff >= 0){
    // above->word >= new string
    above->left = node;
    node->parent = above;
  }
  return -1;
}

void rotateLeft(Tree *tree, Node *node) {
  // node is the parent in this case
  // giving node its child's left subtree
  Node *originalChild = node->right;
  node->right = originalChild->left;
  originalChild->left = node;
  
  //switching parents
  if(node->parent == NULL) { // if node is the root
    tree->root = originalChild;
  }
  else { // node is not the root (i.e. has a parent)
    // setting node's original parent's child to originalChild
    if(node->parent->left == node) { // i.e. node was left child of its parent
      node->parent->left = originalChild;
    }
    else { // node was right child of its parent
      node->parent->right = originalChild;
    }
  }
  Node *temp = node->parent;
  node->parent = originalChild;
  originalChild->parent = temp;
  if(node->right != NULL) {
    node->right->parent = node;
  }
}

void rotateRight(Tree* tree, Node *node) { // TODO must be same type
  // node is the parent in this case     // as in .h file
  // giving node its child's right subtree
  Node *originalChild = node->left;
  node->left = originalChild->right;
  originalChild->right = node;
  
  // switching parents
  if(node->parent == NULL){ // if node is the root
    tree->root = originalChild;
  }
  else { // node is not the root (i.e. has a parent)
    // setting node's original parent's child to originalChild
    if(node->parent->left == node) { // i.e. node was left child of its parent
      node->parent->left = originalChild;
    }
    else { // node was right child of its parent
      node->parent->right = originalChild;
    }
  }
  Node *temp = node->parent;
  node->parent = originalChild;
  originalChild->parent = temp;
  if(node->left != NULL) {
    node->left->parent = node;
  }
}

void insert(Tree *tree, Node *node){
  if(tree->root == NULL){ 	// Root is NULL
    tree->root = node;
    tree->root->col = BLACK;
  }
  else {		  	// Root is not NULL
    BSTinsert(tree, node);    // Do normal BST insert
    node->col = RED;		// Set color to RED (in case it wasn't
				// initialzed as such)
   
    // Color changes keep occurring as long as node and its parent
    // are both red (rotations can only happen 1 to 2 times at most)
    while((node != tree->root) && (node->parent->col == RED)) {

      // There shall always exist a grandfather at this point because there can
      // never be a red root and its children can never be red at the same time
      if(node->parent == node->parent->parent->left) {	// parent is left child

        Node *uncle = node->parent->parent->right;
        // case where uncle is black/NULL
        if(uncle == NULL || uncle->col == BLACK) {

          // check if grandfather and node are zigzag	//	G       /                  G      /
          if(node == node->parent->right) {		//     / \      /                 / \     /
            node = node->parent;			//    P  (U)    /  ---->>>>      N  (U)   /
            rotateLeft(tree, node);			//   / \   \    /               /         / 
          						//  A   N       /              P <-- node points here now
							//     / 	/	      / \         /
         						//    X	 	/	     A   X        /
          }						
          node->parent->col = BLACK;
          node->parent->parent->col = RED;
          rotateRight(tree, node->parent->parent);
        } // end if for black/NULL uncle
        
        // case where uncle is red
        else {
          // make uncle and parent black and grandparent red
          uncle->col = BLACK;
          node->parent->parent->col = RED;
          node->parent->col = BLACK;
          node = node->parent->parent;
        } // end else (for red uncle)
      } // end outer if (for when parent is left child)

      // node's parent is node's grandfather's right child
      else {
        Node *uncle = node->parent->parent->left;
        // case where uncle is black/null
        if(uncle == NULL || uncle->col == BLACK) {
          // check if grandfather and node are zigzag
          if(node == node->parent->left){
            node = node->parent;
            rotateRight(tree, node);
          }
          node->parent->col = BLACK;
          node->parent->parent->col = RED;
          rotateLeft(tree, node->parent->parent);
        } // end if
        // case where uncle is red
        else { // TODO factor out?
          uncle->col = BLACK;
          node->parent->parent->col = RED;
          node->parent->col = BLACK;
          node = node->parent->parent;
        } // end else
      } // end else
    } // end while
  } // end else
  tree->root->col = BLACK;
}

