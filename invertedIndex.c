#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <dirent.h>
#include <errno.h>

#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <libgen.h> // for basename

#include "linkedlist.h"
#include "redblack.h"
#include "comparestrings.h"
void traverseFile(char *filePath, char *fileName, Tree *t){
  // check if file is even openable
  int fd;
  fd = open(filePath, O_RDONLY);
  if(errno == ENAMETOOLONG) {
    printf("Something went wrong with open()! Pathname too long\n");
  }
  else if(errno == EACCES) {
    printf("Something went wrong with open()! Permission to open file denied\n");
  }
  else if(errno == EOVERFLOW) {
    printf("Something went wrong with open()! File to be opened is too big, overflow occurred\n");
  }
  if (fd == 0) {
    // bad stuff
    printf("what happened\n");
    return;
  }
  char *cwh = NULL; //current word holder
  char buffer[20];
  long long count = 0;
  char *curr = buffer;
  char letter;
  //long long begIndex = 0; //probs don't need this
  long long currBufferIndex = 0; //gets reset to zero after reading a buffer of size of 20
  long long currFileIndex = 0; //never gets reset, follows the file till the end
  long long nextSize = 20;
  long long fileLength = lseek(fd, 0, SEEK_END);
  long long endOfCurrentBufferIndex = 20; //fixed value, but depends on the situation, if file is 20 or less bytes to begin with, it's fileLength
                                    //if the fileLength is more than 20 bytes to begin with, it stays at 20 until there's less than 20 bytes left to read, then it's just the number of bytes left to read
  lseek(fd, 0, SEEK_SET);
  //long long bytesLeft = fileLength; //probs don't need this
  //read(fd, buffer, 1);
  //printf("variables %s\n", buffer);
  
  /*if (fileLength - currFileIndex <= 20 && fileLength - currFileIndex > 0) { //fileLength - currFileIndex is basically the number of bytes in the file left to read
    cwh = malloc(sizeof(char) * fileLength + 1);
    read(fd, buffer, fileLength);
    endOfCurrentBufferIndex = fileLength;
    //letter = *curr;
    while (currFileIndex < endOfCurrentBufferIndex) {
      int isValid;
      letter = *curr;
      if (count == 0 && isdigit(letter)) { //number at beginning of token
        isValid = 0;
      }
      else if(isalnum(letter)) {
        isValid = 1;
      }
      else { //junk char
        isValid = 0;
      }
      if (isValid) {
	cwh[count] = tolower(letter);
        count++;
      }
      else {
        if (count > 0) {
          cwh[count] = '\0';
          Node *searchResult = searchTree(t, cwh);
	  if (searchResult == NULL) {
            //first time a word is being added to the tree
	    Node *redBlackNode = malloc(sizeof(Node));
	    redBlackNode = initNode(redBlackNode, RED, cwh);
            insert(t, redBlackNode);
	    llinsert(redBlackNode->filelist, fileName);
	  }
	  else {
            //word already exists in the tree
	    llinsert(searchResult->filelist, fileName);
	  }
	  //reset current word holder
          free(cwh);
	  cwh = malloc(sizeof(char) * fileLength + 1);
	  count = 0;
	}
	//begIndex = currBuffIndex + 1; probs don't need this
      }
      curr++; //in larger documents, curr has to be reset to beginning of buffer after reading every 20 bytes, same goes to letter
      //currBufferIndex++;
      currFileIndex++; //currBuffIndex and currFileIndex are the same in the case where file is 20 or less bytes
      //letter = *curr;
    }
  }*/
  //else {
  cwh = malloc(sizeof(char) * nextSize + 1); //+1 for the null terminating character, but when doubling size of cwh, only double nextSize then add 1 again
  endOfCurrentBufferIndex = 20; //fixed value, only changes when reading the last less than 20 bytes
  //read until less than 20 bytes in the file left, then you can kinda repeat the code above for the last less than 20 bytes
  while (fileLength - currFileIndex >= 20) { //fileLength - currFileIndex is basically the number of bytes in the file left to read
    read(fd, buffer, 20);
    while (currBufferIndex < endOfCurrentBufferIndex) {
      letter = *curr;
      int isValid;
      if (count == 0 && isdigit(letter)) { //number at beginning of token
        isValid = 0;
      }
      else if(isalnum(letter)) {
        isValid = 1;
      }
      else { //junk char
        isValid = 0;
      }
      if (isValid) {
	cwh[count] = tolower(letter);
        count++;
        if (count == nextSize) {
          //if the current token reached one before the end of current word holder (one because last index is always reserved for '\0')
          //then reallocate double the current size of cwh for cwh
          char *tempcwh = malloc(sizeof(char) * nextSize + 1);
          memcpy(tempcwh, cwh, nextSize);
          free(cwh);
	  cwh = malloc(sizeof(char) * (nextSize * 2) + 1);
          memcpy(cwh, tempcwh, nextSize);
          nextSize *= 2; //updating nextSize here is the key
          free(tempcwh); //temp variable can be discarded
        }
    
      }
      //in this case, count has a chance of being greater than 20 because cwh only gets reset after the current word gets added to the tree
      else {
        if (count > 0) {
          cwh[count] = '\0';
          Node *searchResult = searchTree(t, cwh);
	  if (searchResult == NULL) {
            //first time a word is being added to the tree
	    Node *redBlackNode = malloc(sizeof(Node));
	    redBlackNode = initNode(redBlackNode, RED, cwh);
            insert(t, redBlackNode);
	    llinsert(redBlackNode->filelist, fileName);
            //printf("word added %s\n", cwh);
	  }
	  else {
            //word already exists in the tree
	    llinsert(searchResult->filelist, fileName);
	  }
	  //reset current word holder
          free(cwh);
	  count = 0;
          nextSize = 20; //when we add the current word to the tree, nextSize needs to be reset back to 20 for the next current word holder
          cwh = malloc(sizeof(char) * nextSize + 1);
	}
	//begIndex = currBuffIndex + 1; probs don't need this
      }
      curr++; //in larger documents, curr has to be reset to beginning of buffer after reading every 20 bytes, same goes to letter
      //currBufferIndex++;
      currFileIndex++;
      currBufferIndex++;
      //letter = *curr;
    }
    //at the end looping through the entire buffer, reset specific indices
    currBufferIndex = 0;
    curr = buffer;
  }
  //IMPORTANT NOTE, even in last less than 20 bytes, cwh could still hold something so keep that in mind
  if (fileLength - currFileIndex <= 20 && fileLength - currFileIndex > 0) {
    read(fd, buffer, fileLength - currFileIndex);
    endOfCurrentBufferIndex = fileLength - currFileIndex;
    while (currBufferIndex < endOfCurrentBufferIndex) {
      letter = *curr;
      int isValid;
      if (count == 0 && isdigit(letter)) { //number at beginning of token
        isValid = 0;
      }
      else if(isalnum(letter)) {
        isValid = 1;
      }
      else { //junk char
        isValid = 0;
      }
      if (isValid) {
	cwh[count] = tolower(letter);
        count++;
        if (count == nextSize) {
          //if the current token reached one before the end of current word holder (one because last index is always reserved for '\0')
          //then reallocate double the current size of cwh for cwh
          char *tempcwh = malloc(sizeof(char) * nextSize + 1);
          memcpy(tempcwh, cwh, nextSize);
          free(cwh);
	  cwh = malloc(sizeof(char) * (nextSize * 2) + 1);
          memcpy(cwh, tempcwh, nextSize);
          nextSize *= 2; //updating nextSize here is the key
          free(tempcwh); //temp variable can be discarded
        }
          
      }
      //in this case, count has a chance of being greater than 20 because cwh only gets reset after the current word gets added to the tree
      else {
        if (count > 0) {
          cwh[count] = '\0';
          Node *searchResult = searchTree(t, cwh);
	  if (searchResult == NULL) {
            //first time a word is being added to the tree
	    Node *redBlackNode = malloc(sizeof(Node));
	    redBlackNode = initNode(redBlackNode, RED, cwh);
            insert(t, redBlackNode);
	    llinsert(redBlackNode->filelist, fileName);
            //printf("word added %s\n", cwh);
	  }
	  else {
            //word already exists in the tree
	    llinsert(searchResult->filelist, fileName);
	  }
	  //reset current word holder
          free(cwh);
	  count = 0;
          nextSize = 20; //when we add the current word to the tree, nextSize needs to be reset back to 20 for the next current word holder
          cwh = malloc(sizeof(char) * nextSize + 1);
	}
	//begIndex = currBuffIndex + 1; probs don't need this
      }
      curr++; //in larger documents, curr has to be reset to beginning of buffer after reading every 20 bytes, same goes to letter
      //currBufferIndex++;
      currFileIndex++;
      currBufferIndex++;
      //letter = *curr;
    }
  }
  //}
  //if the file doesn't end with garbage value, ensures the last word gets inserted into our data structure, regardless if the file is small (less than or equal to 20 bytes or not)
  if (count > 0) {
    cwh[count] = '\0';
    Node *searchResult = searchTree(t, cwh);
    if (searchResult == NULL) {
      //first time a word is being added to the tree
      Node *redBlackNode = malloc(sizeof(Node));
      redBlackNode = initNode(redBlackNode, RED, cwh);
      insert(t, redBlackNode);
      llinsert(redBlackNode->filelist, fileName);
      //printf("word added %s\n", cwh);
    }
    else {
      //word already exists in the tree
      llinsert(searchResult->filelist, fileName);
    }
    //reset current word holder
    free(cwh);
    count = 0;
    nextSize = 20; //when we add the current word to the tree, nextSize needs to be reset back to 20 for the next current word holder
    //cwh = malloc(sizeof(char) * nextSize + 1);
  }
}
int traverseDir(char *dirname, Tree *t) {

  DIR *dir;             // directory struct
  struct dirent *de;    // directory entry
  int fd;		// file descriptor to represent each file in a directory
  //int file_length;  // length of current file
  //char file_path[10000];   // path to a file in this directory
  char *filePath;
  /*
   * NOTE: The above is not efficient, because in every recursive call
   * of this function, we create a new path string and add on to it.
   *
   */
  
  // .
  // ./test
  // ./test/test2 <- could be other stuff in here; can't modify this recursive call's path variable!!
  // ./test/test2/test3
  //
  // TODO stuff regarding above^, if you do stuff to deal above using malloc, remember to deal with the issue of freeing words in llnodes

  dir = opendir(dirname);
  
  if (dir != NULL) {
    de = readdir(dir);
    while (de != NULL) {
      //printf("%s\n", de->d_name);
      //snprintf(file_path, 10000, "%s/%s", dirname, de->d_name); 	// as we traverse the directory, filepath will update to contain the filename of current file
      filePath = malloc((strlen(dirname) + strlen(de->d_name) + 2) * sizeof(char)); // + 2 BECAUSE +1 FOR THE '/' CHARACTER AND ANOTHER +1 FOR NULL TERMINATING CHARACTER
      snprintf(filePath, strlen(dirname) + strlen(de->d_name) + 2, "%s/%s", dirname, de->d_name);
      //printf("dirname = %s\nread-in thing's name = %s\nfilePath = %s\n", dirname, de->d_name, filePath);
      // append the name of this file (whether it's a regular file
      // or a directory) to the current path name

      // TODO check for overflow!!

      //printf("Full path to this file: %s\n", file_path);
      if (de->d_type == DT_DIR && 
	  strcmp(de->d_name, "..") && strcmp(de->d_name, ".")) { // directory, should we use our compare method or does it not matter?
        //printf("This is a directory!\n");
	// TODO recursively call traverseDir
	traverseDir(filePath, t);
      }
      else if (de->d_type == DT_REG) {		// regular file
        //printf("This is a regular file.\n");
	fd = open(de->d_name, O_RDONLY);
        if(errno == ENAMETOOLONG) {
          printf("Something went wrong with open()! Pathname too long\n");
        }
        else if(errno == EACCES) {
          printf("Something went wrong with open()! Permission to open file denied\n");
        }
        else if(errno == EOVERFLOW) {
          printf("Something went wrong with open()! File to be opened is too big, overflow occurred\n");
        }
        //file_length = lseek(fd, 0, SEEK_END);
        lseek(fd, 0, SEEK_SET);
        //printf("Length is %d\n", file_length);
        char *fileName = basename(filePath);
        traverseFile(filePath,fileName, t); 
      }

      //printf("\n");

      free(filePath);
      de = readdir(dir); // prep for next iteration of loop
    }
    closedir(dir); // all entries read; close the directroy
    return 0;
  } 
  else {
    printf("Could not open directory!");
    if(errno == EBADF) {
      printf("Something went wrong with opendir()! Invalid file descriptor\n");
    }
    else if(errno == EACCES) {
      printf("Something went wrong with opendir()! Permission to open file denied\n");
    }
    // printf("Something went wrong with opendir()! %s\n", strerror(errno));
    // permission issue, not a directory issue, etc.
    // TODO check errno
    return -1;
  }


}
void fileLLTraversal(llnode *head, FILE *fp) {
  if (head == NULL) return;
  
  llnode *curr = head;
  while (curr != NULL) {
    fprintf(fp, "\t\t<file name=\"%s\">%i</file>\n", curr->filename, curr->count);
    curr = curr->next;
  }
}
void fileInorderTraversal(Node *root, FILE *fp) {
  if (root == NULL) return;
 
  // keep going left
  fileInorderTraversal(root->left, fp);  

  // write word text to file
  fprintf(fp, "\t<word text=\"%s\">\n", root->word);

  // perform a merge sort on each BST node's linked list before traversing through it
  llnode *head = mergeSort(root->filelist->head);

  // traverse the node's linked list
  fileLLTraversal(head, fp);

  fprintf(fp, "\t</word>\n");

  // go right
  fileInorderTraversal(root->right, fp);

}
//TODO FILEPATH SHOULD BE FILENAME INSTEAD, WHAT IF USER WANTED TO WRITE TO FILE IN A FOLDER INSIDE DIRECTORY?
void writeXML(char *filePath, Tree *t) {
  FILE *fp = fopen(filePath, "w"); 
  //TODO implement error check if file can't be opened or if it exist already, if it exists then need to prompt user
  fprintf(fp, "<?xml version=\"%i.%i\" encoding=\"UTF-%i\"?>\n<fileIndex>\n",1,0,8);
  //traverse through the tree
  //for each BST node, sort each linked list, then traverse through each linked list and write to the XML
  fileInorderTraversal(t->root, fp);
  fprintf(fp, "</fileIndex>\n");
}

int main(int argc, char *argv[]) {

  if (argc != 3) {
    printf("Incorrect number of arguments.\n"
           "Usage: ./indexer <inverted-index file name> <directory or file name>\n");
    return -1;
  }


  //llnode * head = NULL;
  Tree t;
  initTree(&t);
  //char *word;
  //Node *node;
  char *filePath = argv[2];
  char *XMLfile = argv[1];
  if(access(XMLfile, F_OK) != -1) {
    //if file exists, ask user for overwrite permission
    DIR *xml = opendir(XMLfile);
    if (xml != NULL) {
      printf("Inverted-index file name cannot be a directory, exiting program\n");
      return 0;
    }
    else {
      if(errno == EBADF) {
        printf("Something went wrong with opendir()! Invalid file descriptor\n");
      }
      else if(errno == EACCES) {
        printf("Something went wrong with opendir()! Permission to open file denied\n");
      }
      printf("Inverted-index file with that name already exists, would you like to overwrite it (y or n)?\n");
      char answer[5000];
      scanf("%s", answer);
      while (strcmp(answer, "y") && strcmp(answer, "n")) {
        printf("Invalid answer, enter y or n\n");
        scanf("%s", answer);
      }
      if (!strcmp(answer, "y")) {
        printf("User chose to overwrite existing file\n");
      }
      else {
        printf("User chose to not overwrite existing file, exiting program\n");
        return 0;
      }
    }
    closedir(xml);
  } else {
    //if file doesn't exist
    //create the file if needby (additional folders, etc.)
    //printf("File doesn't exist\n");
    //if the file path doesn't contain a slash then it's just a file so this should be ok
    if (strstr(XMLfile, "/") == NULL) {
      //do nothing because if a file doesn't exist then it's ok because it's going to be made later
      //printf("doesn't contain a slash\n");
    }
    else {
      //case example: folder/folder2/ exists and want to write to folder/folder2/newfile
      //another case: folder/folder2/folder3/ is bad
      //check if last character is '/', then we can assume inverted-index file path is a directory and we can exit
      if(XMLfile[strlen(XMLfile)-1] == '/') {
        printf("Inverted-index file name cannot be a directory, exiting program\n");
        return 0;
      }
      else {
        //in the case where the last part of the xml file path is a file that does not exist
        //but we need to make sure that the folders preceding the newly wanted file exist
        //case example: folder/folder2/file, does folder/folder2 exist? because we know file doesn't exist in folder/folder2
        //extracting the string preceding, but including, the last slash

        char *p = XMLfile + strlen(XMLfile) - 1; // pointer to last character
        // will always be at least one slash, so no need to keep a counter to prevent running off the front
        while (*p != '/') {
          p--;
        }
        // p points to the last slash
        // get everything from 0 to p, including the slash
        char extracted[p - XMLfile + 2];
        char requestedFileName[strlen(XMLfile) - (p - XMLfile + 1) + 1];
        memcpy(extracted, XMLfile, p - XMLfile + 1);
        strcpy(requestedFileName, p + 1);
        extracted[p - XMLfile + 1] = '\0';
        DIR *xml = opendir(extracted);
        if (xml == NULL) {
          if(errno == EBADF) {
            printf("Something went wrong with opendir()! Invalid file descriptor\n");
          }
          else if(errno == EACCES) {
            printf("Something went wrong with opendir()! Permission to open file denied\n");
          }
          printf("Requested directory %s for inverted-index file %s to be in does not exist, exiting program\n", extracted, requestedFileName);
          return 0;
        }
        closedir(xml);
      }
    }
  }
  //check if filepath is a file
  //if so then open the file and start reading it
  DIR *dir = opendir(filePath);
  if (dir == NULL) {
    //check errno here???
    if(errno == EBADF) {
      printf("Something went wrong with opendir()! Invalid file descriptor\n");
    }
    else if(errno == EACCES) {
      printf("Something went wrong with opendir()! Permission to open file denied\n");
    }
    // if opendir returns null, then the filePath leads to a file
    // FIRST check errno to make sure it's not a permission error
    // if necessary, extract filename from filepath
    char *fileName = basename(filePath);
    traverseFile(filePath,fileName, &t); // filePath is a file name <-not always
    //printf("finished traversing");
    writeXML(XMLfile, &t);
  }
  else {
    //if opendir returns anything other than NULL, then the filePath leads to a directory
    traverseDir(filePath, &t);
    writeXML(XMLfile, &t);
  }
  closedir(dir);
  //BSTinorder(t.root);
  //printf("hi");
  // else filePath is a directory and we should begin traversing!


  /*
  char *curr = argv[1]; //points to the current word being parsed
  long long count = 0; //keeps track of the current word's length
  char letter = *curr; //keeps track of the current letter being iterated through
  long long beginningIndex = 0; //keeps track of where the first char is in a contiguous sequence of alphabetic letters
  long long currentIndex = 0; //keeps track of the index of the current letter being iterated on
  
  while (letter != '\0') { //keep looping until the end of the input
    // printf("Letter: %c\n", letter);
    if (isalpha(letter)) {
      count++;
    }
    else {
      if (count != 0) { //signifies the end of a consecutive sequence of alphabetic characters
        // printf("End of word found\n");
        word = (char *) malloc(sizeof(char) * (count + 1)); //+1 for null terminating character
        if (word == NULL) {
          printf("FATAL ERROR. MEMORY COULD NOT BE ALLOCATED!\n");
          return -1;
        }
        memcpy(word, argv[1] + beginningIndex, count);
        word[count] = '\0';
        count = 0;
        node = (Node *) malloc(sizeof(Node));
        if (node == NULL) {
          printf("FATAL ERROR. MEMORY COULD NOT BE ALLOCATED!\n");
          return -1;
        }
        initNode(node, RED, word);
        insert(&t, node);
      }
      else { // If count is 0 then that means two consecutive
	     //non alphabetic characters have been found in a row
      }

      beginningIndex = currentIndex + 1;
    }
    //update variables for next iteration
    //printf("%c", letter);
    curr++;
    currentIndex++;
    letter = *curr;
  }
  // In the case that a contiguous sequence of alphabetic letters
  // doesn't end with a non alphabetic char (i.e. its end is
  // the end of the string), it has to be added this way
  if (count > 0) {
    word = (char *) malloc(sizeof(char) * (count + 1)); //+1 for null terminating character
    if (word == NULL) {
      printf("FATAL ERROR. MEMORY COULD NOT BE ALLOCATED!\n");
      return -1;
    }
    memcpy(word, argv[1] + beginningIndex, count);
    word[count] = '\0';
    count = 0;
    node = (Node *) malloc(sizeof(Node));
    if (node == NULL) {
      printf("FATAL ERROR. MEMORY COULD NOT BE ALLOCATED!\n");
      return -1;
    }
    initNode(node, RED, word);
    insert(&t, node);
  }
  
  BSTinorder(t.root);
  //printTree(&t);
  freeNodesInTree(&t);
  
  */

//traverseDir(".");


  return 0;
}
