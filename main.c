/**
 *
 * Port renew template engine to C, for use in C CGI scripts.
 *
 * Compiled with this:
      clang-3.5 main.c -Weverything -pedantic -I /usr/include/glib-2.0 `pkg-config --cflags glib-2.0` `pkg-config --libs glib-2.0` -Wno-documentation
 *
 * @since 2015-02-22
 * @author Olle Härstedt
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <glib.h>
#include <ctype.h>

#include <dmalloc.h>

static void read_keywords(void);
void renew_init(void);
void renew_teardown(void);
char* trimwhitespace(char * str);

/* Array of keywords */
static GHashTable* keywords;

// Note: This function returns a pointer to a substring of the original string.
// If the given string was allocated dynamically, the caller must not overwrite
// that pointer with the returned value, since the original pointer must be
// deallocated using the same allocator with which it was allocated.  The return
// value must NOT be deallocated using free() etc.
char *trimwhitespace(char *str)
{
  char *end;

  // Trim leading space
  while(isspace(*str)) str++;

  if(*str == 0)  // All spaces?
    return str;

  // Trim trailing space
  end = str + strlen(str) - 1;
  while(end > str && isspace(*end)) end--;

  // Write new null terminator
  *(end+1) = 0;

  return str;
}

/**
 * Read keywords from keyword file "keywords.txt"
 *
 * @since 2015-02-22
 */
void read_keywords(void) {
  FILE *file = fopen("keywords.txt", "r");
  if (file) {
    char* line = NULL;
    size_t len = 0;
    ssize_t read;

    while ((read = getline(&line, &len, file)) != -1) {
      //printf("Line = %s", line);

      char *tokens[10]; // Max ten tokens
      char* line_tmp = line;
      int i = 0;
      while ((tokens[i] = strsep(&line, "\t")) != NULL) {
        if (i == 0 && tokens[i][0] == '#') {
          // Skip comments ('#' is first on line)
        }
        else {
          // NOTE: Not a copy, must copy manually.
          //char* trimed_token = trimwhitespace(tokens[i]);
          //printf("token = |%s|\n", trimed_token);
        }

        if (tokens[i][0] == '\0') {
          fprintf(stderr, "read_keywords: No spaces allowed in keywords.txt, use tabs.\n");
          exit(EXIT_FAILURE);
        }

        i++;

        if (i > 9) {
          fprintf(stderr, "read_keywords: More then 9 tokens on one row in keywords.txt. Aborting.\n");
          exit(EXIT_FAILURE);
        }
      }

      switch (i) {
        case 2:
          // Copy tokens to void freeing them
          {
            /**
            unsigned int key_len = (unsigned) strlen(tokens[0]);
            unsigned int value_len = (unsigned) strlen(tokens[1]);
            char* key = malloc(key_len);
            char* value = malloc(strlen(tokens[1]));
            key = strncpy(key, tokens[0], key_len);
            value = strncpy(value, tokens[1], value_len);

            g_hash_table_insert(keywords, key, value);
            */
          }
          break;
        default:
          break;
      }

    }

    //unsigned int table_size = g_hash_table_size(keywords);
    //printf("table size = %d\n", table_size);

    free(line);
    fclose(file);
  }
  else {
    // Abort, no keywords found
    fprintf(stderr, "No keywords.txt file found, aborting.\n");
    exit(EXIT_FAILURE);
  }
}

/**
 * Init stuff
 */
void renew_init(void) {

  keywords = g_hash_table_new(g_str_hash, g_str_equal);

  read_keywords();
}

/**
 * Free all
 */
void renew_teardown(void) {
  g_hash_table_destroy(keywords);
}

int main(void) {
  renew_init();
  renew_teardown();
}
