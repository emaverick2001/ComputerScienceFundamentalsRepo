#include <stdint.h>
#include <stdio.h>

#include "wcfuncs.h"

// Suggested number of buckets for the hash table
#define HASHTABLE_SIZE 13249

int main(int argc, char **argv) {
  // stats (to be printed at end)
  uint32_t total_words = 0;
  uint32_t unique_words = 0;
  const unsigned char *best_word = (const unsigned char *)"";
  uint32_t best_word_count = 0;

  const unsigned char *filename = (const unsigned char *)argv[1];
  FILE *in;
  if (argc == 2) {
    in = fopen((const char *)filename, "r");
    if (in == NULL) {
      fprintf(stderr, "Error: could not open file %s\n", filename);
      return 1;
    }
  } else if (argc == 1) {
    // No filename provided, read from user input
    in = stdin;
  } else {
    printf("Usage: %s [filename]\n", argv[0]);
    return 1;
  }

  struct WordEntry *hashtable[HASHTABLE_SIZE];

  unsigned char word[MAX_WORDLEN + 1];
  word[MAX_WORDLEN] = '\0';

  // initialize the hash table in a loop
  for (int i = 0; i < HASHTABLE_SIZE; i++) {
    hashtable[i] = NULL;
  }

  while (wc_readnext(in, word)) {
    // increase total word count by 1
    total_words++;
    // use wc_tolower to convert word to lower case
    wc_tolower(word);

    // use wc_trim_non_alpha to remove non-alphabetic characters at end of
    // word
    wc_trim_non_alpha(word);
    // use wc_find_or_insert to find or insert the word in the hash table
    struct WordEntry *wordObject =
        wc_dict_find_or_insert(hashtable, HASHTABLE_SIZE, word);
    // increment the WordEntry's count
    wordObject->count += 1;
  }

  for (int i = 0; i < HASHTABLE_SIZE; i++) {
    struct WordEntry *entry = hashtable[i];
    while (entry != NULL) {
      unique_words++;
      if (entry->count > best_word_count) {
        best_word = entry->word;
        best_word_count = entry->count;
      } else if (entry->count == best_word_count) {
        if (wc_str_compare(entry->word, best_word) < 0) {
          best_word_count = entry->count;
          best_word = entry->word;
        }
      }
      entry = entry->next;
    }
  }

  printf("Total words read: %u\n", (unsigned int)total_words);
  printf("Unique words read: %u\n", (unsigned int)unique_words);
  printf("Most frequent word: %s (%u)\n", (const char *)best_word,
         best_word_count);

  // make sure file is closed (if one was opened)
  fclose(in);
  // make sure memory is freed
  for (int i = 0; i < HASHTABLE_SIZE; i++) {
    wc_free_chain(hashtable[i]);
  }

  return 0;
}