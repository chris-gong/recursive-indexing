#include "comparestrings.h"
#include <string.h>
#include <ctype.h>
#include <stdio.h>

int compare(char *a, char *b) {
  char c, d;

  if (a == NULL || b == NULL) {
    return -2; // ERROR
  }
  int minLen = 0;
  int lena = strlen(a); // NOTE could fail
  int lenb = strlen(b); // NOTE could fail

  if (lena < lenb) {
    minLen = lena;
  } else {
    minLen = lenb;
  }
  while ((c=*a) != '\0' && (d=*b) != '\0') {
    int cisalpha = isalpha(c);
    int disalpha = isalpha(d);
    if ((cisalpha == 0 && disalpha == 0) || (cisalpha > 0 &&
	disalpha > 0)) { // either both chars are letters, or both are digits
        if (cisalpha) { // both are letters
	  c = tolower(c); // TODO could just call this regardless, but then
	  d = tolower(d); // we could end up calling it on digits
	}
	if (c < d) {
          return -1;
	}
	else if (c > d) {
          return 1;
	}
	else {
          a++;
	  b++;
	}
    }
    else if (cisalpha) { // c is a letter, d is a digit
      //printf("c is a letter, d is a digit\n");
      return -1; // letters come before digits
    }
    else { // d is a letter, c is a digit
      //printf("d is a letter, c is a digit\n");
      return 1; // digits come after letters
    }
  } // end while

  //printf("strings are equal up to this point.\n");
  if (lena == lenb) {
    //printf("Exactly equal\n");
    return 0;
  }
  else if (minLen == lena) {
    //printf("First string shorter than second, so first < second\n");
    return -1;
  }
  else {
    //printf("Second string shorter than first, so second < first\n");
    return 1;
  }


}

/*
int main() {
  //printf("%d\n", isalpha('t'));
  
  char s1[50];
  char s2[50];

  printf("Enter string one:\n");
  fgets(s1, 50, stdin);

  printf("Enter string two:\n");
  fgets(s2, 50, stdin);


  int result = compare(s1, s2);
  printf("Result is %d\n", result);

  return 0;
}
*/

