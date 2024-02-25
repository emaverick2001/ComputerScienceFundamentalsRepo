// Important: do not add any additional #include directives!
// The only C library functions that may be used are
//
// - fgetc
// - malloc
// - free

#include <ctype.h>
#include <stdlib.h>

#include "wcfuncs.h"

// Compute a hash code for the given NUL-terminated
// character string.
//
// The hash algorithm should be implemented as follows:
//
// uint32_t hash_code = 5381
// for each character c of w in order {
//   hash_code = hash_code * 33 + c
// }
//
// Note that the character values should be treated as
// being unsigned (in the range 0..255)
uint32_t wc_hash(const unsigned char *w) {

  uint32_t hash_code = 5381;
  int i = 0;
  while (w[i] != '\0') {
    hash_code = (hash_code * 33) + w[i];
    i++;
  }
  return hash_code;
}

// Compare two strings lexicographically. Return
//
// - a negative value if lhs string is less than rhs string
// - 0 if lhs string is identical to rhs string
// - a positive value if lhs string is greater than rhs string
//
// Lexicographical comparison is a generalization of alphabetical
// order, but using character codes. If one string is a prefix
// of the other, it is considered as "less than". E.g.,
// "hi" would compare as less than "high".
int wc_str_compare(const unsigned char *lhs, const unsigned char *rhs) {
  // use to_lower to convert strings characters to lowercase and then compare char by char
  unsigned char templ[MAX_WORDLEN + 1];
  unsigned char tempr[MAX_WORDLEN+1];
  wc_str_copy(templ,lhs);
  wc_str_copy(tempr,rhs);
  wc_tolower(templ);
  wc_tolower(tempr);
  int i = 0;
  while (templ[i] != '\0' && tempr[i] != '\0') {
    if (templ[i] < tempr[i]) {
      return -1;
    } else if (templ[i] > tempr[i]) {
      return 1;
    }
    i++;
  }
  if (templ[i] == '\0' && tempr[i] == '\0') {
    return 0;
  } else if (templ[i] == '\0') {
    return -1;
  }
  return 1;
}

// Copy NUL-terminated source string to the destination buffer.
void wc_str_copy(unsigned char *dest, const unsigned char *source) {
  // iterate through source and add each character to *dest
  int sourceLength = 0;

  while (source[sourceLength] != '\0') {
    sourceLength++;
  }

  for (int i = 0; i < sourceLength; i++) {
    dest[i] = source[i];
  }
  dest[sourceLength] = '\0';
}

// Return 1 if the character code in c is a whitespace character,
// false otherwise.
//
// For the purposes of this function, a whitespace character is one of
//
//   ' '
//   '\t'
//   '\r'
//   '\n'
//   '\f'
//   '\v'
int wc_isspace(unsigned char c) {
  // ' ': space (32)
  // \t: Represents the tab character (ASCII 9).
  // \r: Represents the carriage return character (ASCII 13).
  // \n: Represents the newline character (ASCII 10).
  // \f: form feed (12)
  // \v: vert tab (11)

  switch (c) {
    case 10:
    case 9:
    case 13:
    case 32:
    case 12:
    case 11:
      return 1;
    default:
      return 0;
  }
}

// Return 1 if the character code in c is an alphabetic character
// ('A' through 'Z' or 'a' through 'z'), 0 otherwise.
int wc_isalpha(unsigned char c) {
  if ((c >= 65 && c <= 90) || (c >= 97 && c <= 122)) {
    return 1;
  }

  return 0;
}

// }

// Read the next word from given input stream, storing
// the word in the array pointed-to by w. (This array should be
// assumed to be MAX_WORDLEN+1 elements in size.) Return 1
// if a word is read successfully, 0 otherwise.
//
// For the purposes of this function, a word is a sequence of
// 1 or more non-whitespace characters.
//
// If a sequence of non-whitespace characters has more than
// MAX_WORDLEN characters, then only the first MAX_WORDLEN
// characters in the sequence should be stored in the array.
int wc_readnext(FILE *in, unsigned char *w) {

  int i = 0;
  int c = fgetc(in);
  while (c != EOF) {
    if (wc_isspace(c)) {
      if (i == 0) {
        c = fgetc(in);
        continue;
      } else {
        break;
      }
    } else {  
      w[i] = c;
      i++;
      if (i == MAX_WORDLEN) {
        break;
      }
    }
    c = fgetc(in);
  }
  w[i] = '\0';
  if (i == 0) {
    return 0;
  }
  return 1;
}

// Convert the NUL-terminated character string in the array
// pointed-to by w so that every letter is lower-case.
void wc_tolower(unsigned char *w) {
  int i = 0;
  while (w[i] != '\0') {
    if (w[i] >= 65 && w[i] <= 90) {
      w[i] += 32;
    }
    i++;
  }
}

// Remove any non-alphaabetic characters from the end of the
// NUL-terminated character string pointed-to by w.
void wc_trim_non_alpha(unsigned char *w) {
  int i = 0;
  while (w[i] != '\0') {
    i++;
  }
  i--;
  while (i >= 0 && wc_isalpha(w[i]) == 0) {

    w[i] = '\0';
  
    i--;
  }
}

// Search the specified linked list of WordEntry objects for a word object
// containing the specified string.
//
// If a matching object is found, set the int variable pointed-to by
// inserted to 0 and return a pointer to the matching object.
//
// If a matching object is not found, allocate a new WordEntry object,
// set its next pointer to point to what head points to (i.e., so the
// new object is at the head of the list), set the variable pointed-to
// by inserted to 1, and return a pointer to the new node. Note that
// the new node should have its count value set to 0. (It is the caller's
// job to update the count.)
struct WordEntry *wc_find_or_insert(struct WordEntry *head,
                                    const unsigned char *s, int *inserted) {
  struct WordEntry *curr;
  curr = head;

  // traverse through the linked list to look for the struct object with the
  // specified word

  while (curr != NULL) {
    if (wc_str_compare(s, curr->word) == 0) {
      *inserted = 0;
      return curr;
    }
    curr = curr->next;
  }

  // if word s is not found in the linked list, create & initialize new node and
  // return

  struct WordEntry *new_node =
      (struct WordEntry *)malloc(sizeof(struct WordEntry));
  wc_str_copy(new_node->word, s);
  new_node->count = 0;
  new_node->next = head;
  *inserted = 1;
  return new_node;
}

// Find or insert the WordEntry object for the given string (s), returning
// a pointer to it. The head of the linked list which contains (or should
// contain) the entry for s is the element of buckets whose index is the
// hash code of s mod num_buckets.
//
// Returns a pointer to the WordEntry object in the appropriate linked list
// which represents s.
struct WordEntry *wc_dict_find_or_insert(struct WordEntry *buckets[],
                                         unsigned num_buckets,
                                         const unsigned char *s) {

  // calculate element index
  int element_index = wc_hash(s) % num_buckets;

  int inserted = 0;
  struct WordEntry *head = buckets[element_index];
  struct WordEntry *result = wc_find_or_insert(head, s, &inserted);
  if (inserted == 1){
    buckets[element_index] = result;
  }
  return result;
}

// Free all of the nodes in given linked list of WordEntry objects.
void wc_free_chain(struct WordEntry *p) {
  while (p != NULL) {
    struct WordEntry *skip = p->next;
    free(p);
    p = skip;
  }
}