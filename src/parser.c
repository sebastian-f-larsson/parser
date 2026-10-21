#include "parser.h"
#include <ctype.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct Block {
  struct value *values;
  size_t value_size;
  size_t value_capacity;
};

struct ParserArray {
  struct Block *blocks;
  size_t block_size;
  size_t block_capacity;
};

struct Parser {
  struct value *values;
  size_t value_size;
  size_t value_capacity;
};

Parser *parser_create(void) {
  Parser *p = calloc(1, sizeof(*p));

  if (p == NULL) {
    fprintf(stderr, "%s\n", strerror(errno));
    return NULL;
  }

  return p;
}

ParserArray *parser_array_create(void) {
  ParserArray *p = calloc(1, sizeof(*p));

  if (p == NULL) {
    fprintf(stderr, "%s\n", strerror(errno));
    return NULL;
  }

  return p;
}

static int init_parser(Parser *p) {
  p->value_capacity = 10;
  p->value_size = 0;
  p->values = calloc(p->value_capacity, sizeof(struct value));

  if (p->values == NULL) {
    fprintf(stderr, "%s\n", strerror(errno));
    return -1;
  }

  return 0;
}

static int resize_parser(Parser *p) {

  p->value_capacity *= 2;

  struct value *tmp =
      realloc(p->values, p->value_capacity * sizeof(struct value));

  if (tmp == NULL) {
    fprintf(stderr, "%s\n", strerror(errno));
    return -1;
  }

  p->values = tmp;

  return 0;
}

int parser_array_find_item_index_int(const ParserArray *p, const char *field,
                                     const int value) {
  for (size_t i = 0; i < p->block_size; i++) {
    printf("%zu\n", i);
    for (size_t j = 0; j < p->blocks[i].value_size; j++) {
      if (strcmp(field, p->blocks[i].values[j].field) == 0 &&
          value == p->blocks[i].values[j].integer &&
          p->blocks[i].values[j].type == VALUE_INT) {
        return i;
      }
    }
  }

  return -1;
}

size_t parser_array_count(const ParserArray *p) { return p->block_size; }

void destroy_parser(Parser **p) {
  for (size_t i = 0; i < (*p)->value_size; i++) {
    switch ((*p)->values[i].type) {
    case VALUE_INT:
      break;
    case VALUE_STRING:
      free((*p)->values[i].string);
      break;
    case VALUE_ARRAY:
      for (size_t j = 0; j < (*p)->values[i].array.count; j++) {
        if ((*p)->values[i].array.items[j].type == VALUE_STRING) {
          free((*p)->values[i].array.items[j].string);
        }
      }

      free((*p)->values[i].array.items);
    }

    free((*p)->values[i].field);
  }

  free((*p)->values);
  free((*p));
}

static int init_parser_array_item(struct Block *b) {
  b->value_capacity = 10;
  b->value_size = 0;
  b->values = calloc(b->value_capacity, sizeof(struct value));

  if (b->values == NULL) {
    fprintf(stderr, "%s\n", strerror(errno));
    return -1;
  }

  return 0;
}

static int init_parser_array_block(ParserArray *p) {
  p->block_capacity = 10;
  p->block_size = 0;
  p->blocks = calloc(p->block_capacity, sizeof(struct Block));

  if (p->blocks == NULL) {
    fprintf(stderr, "%s\n", strerror(errno));
    return -1;
  }

  return 0;
}

static int resize_blocks(ParserArray *p) {

  p->block_capacity *= 2;

  struct Block *tmp =
      realloc(p->blocks, p->block_capacity * sizeof(struct Block));

  if (tmp == NULL) {
    fprintf(stderr, "%s\n", strerror(errno));
    return -1;
  }

  p->blocks = tmp;

  return 0;
}

static int resize_items(struct Block *b) {

  b->value_capacity *= 2;

  struct value *tmp =
      realloc(b->values, b->value_capacity * sizeof(struct value));

  if (tmp == NULL) {
    fprintf(stderr, "%s\n", strerror(errno));
    return -1;
  }

  b->values = tmp;

  return 0;
}

void destroy_parser_array(ParserArray **p) {
  for (size_t b = 0; b < (*p)->block_size; b++) {

    for (size_t i = 0; i < (*p)->blocks[b].value_size; i++) {
      switch ((*p)->blocks[b].values[i].type) {
      case VALUE_INT:
        break;
      case VALUE_STRING:
        free((*p)->blocks[b].values[i].string);
        break;
      case VALUE_ARRAY:
        for (size_t j = 0; j < (*p)->blocks[b].values[i].array.count; j++) {
          if ((*p)->blocks[b].values[i].array.items[j].type == VALUE_STRING) {
            free((*p)->blocks[b].values[i].array.items[j].string);
          }
        }

        free((*p)->blocks[b].values[i].array.items);
      }

      free((*p)->blocks[b].values[i].field);
    }
    free((*p)->blocks[b].values);
  }
  free((*p)->blocks);
  free((*p));
}

static char *ltrim_c(const char *str) {
  size_t new_size = strlen(str) - 1;

  char *new_string = calloc(new_size + 1, sizeof(char));

  if (new_string == NULL) {
    fprintf(stderr, "%s\n", strerror(errno));
    return NULL;
  }

  strncpy(new_string, str + 1, new_size);

  return new_string;
}

static char *rtrim_c(const char *str) {
  size_t new_size = strlen(str) - 1;

  char *new_string = calloc(new_size + 1, sizeof(char));

  if (new_string == NULL) {
    fprintf(stderr, "%s\n", strerror(errno));
    return NULL;
  }

  memcpy(new_string, str, new_size);

  return new_string;
}

static char *trim_array(const char *str) {
  char *ltrim_arr = ltrim_c(str);
  char *rtrim_arr = rtrim_c(ltrim_arr);
  free(ltrim_arr);
  return rtrim_arr;
}

static char *ltrim(char *str) {
  if (isspace(str[0]) == 0)
    return str;

  int index = 0;

  while (isspace(str[index]) != 0) {
    index++;
  }

  size_t new_size = strlen(str) - index + 1;

  char *new_string = calloc(new_size, sizeof(char));

  if (new_string == NULL) {
    fprintf(stderr, "%s\n", strerror(errno));
    return NULL;
  }

  strncpy(new_string, str + index, new_size);

  return new_string;
}

static char *rtrim(char *str) {
  size_t end = strlen(str);

  if (isspace(str[end - 1]) == 0) {
    return str;
  }

  int new_size = end;

  for (size_t i = 0; i < end; i++) {
    if (isspace(str[i]) != 0) {
      new_size = i;
    }
  }

  char *new_string = calloc(new_size + 1, sizeof(char));

  if (new_string == NULL) {
    fprintf(stderr, "%s\n", strerror(errno));
    return NULL;
  }

  strncpy(new_string, str, new_size);

  return new_string;
}

static size_t length(const char *arr) {
  size_t arr_size = 1;
  size_t arr_len = strlen(arr);

  if (arr[0] == '\0')
    return 0;

  for (size_t i = 0; i < arr_len; i++) {
    if (arr[i] == ',')
      arr_size++;
  }

  return arr_size;
}

static char *trim(char *str) {
  char *lt = ltrim(str);
  char *rt = rtrim(lt);
  return rt;
}

int parser_get_int(const Parser *p, const char *field, int *out) {
  for (size_t i = 0; i < p->value_size; i++) {
    if (strcmp(p->values[i].field, field) == 0 &&
        p->values[i].type == VALUE_INT) {
      (*out) = p->values[i].integer;
      return 0;
    }
  }

  return -1;
}

int parser_get_string(const Parser *p, const char *field, char **out) {
  for (size_t i = 0; i < p->value_size; i++) {
    if (strcmp(p->values[i].field, field) == 0 &&
        p->values[i].type == VALUE_STRING) {
      (*out) = strdup(p->values[i].string);
      return 0;
    }
  }

  return -1;
}

int parser_array_get_int(const ParserArray *p, const char *field,
                         const int index, int *out) {
  for (size_t i = 0; i < p->blocks[index].value_size; i++) {
    if (strcmp(p->blocks[index].values[i].field, field) == 0 &&
        p->blocks[index].values[i].type == VALUE_INT) {
      (*out) = p->blocks[index].values[i].integer;
      return 0;
    }
  }

  return -1;
}

int parser_array_get_string(const ParserArray *p, const char *field,
                            const int index, char **out) {
  for (size_t i = 0; i < p->blocks[index].value_size; i++) {
    if (strcmp(p->blocks[index].values[i].field, field) == 0 &&
        p->blocks[index].values[i].type == VALUE_STRING) {
      (*out) = strdup(p->blocks[index].values[i].string);
      return 0;
    }
  }

  return -1;
}

int parser_array_get_array_string(const ParserArray *p, const char *field,
                            const int index, char ***out, size_t *size) {
  for (size_t i = 0; i < p->blocks[index].value_size; i++) {
    if (strcmp(p->blocks[index].values[i].field, field) == 0 &&
        p->blocks[index].values[i].type == VALUE_ARRAY) {


  (*out) = calloc(p->blocks[index].values[i].array.count,
                      sizeof(char *) * p->blocks[index].values[i].array.count);

      if ((*out) == NULL) {
        fprintf(stderr, "%s\n", strerror(errno));
        return -1;
      }

      (*size) = p->blocks[index].values[i].array.count;
      for (size_t j = 0; j < p->blocks[index].values[i].array.count; j++) {
        char *trimmed = trim(p->blocks[index].values[i].array.items[j].string);
        (*out)[j] = strdup(trimmed);
      }

      return 0;
    }
  }

  return -1;
}



int parser_get_int_array(const Parser *p, const char *field, int **out,
                         size_t *size) {
  for (size_t i = 0; i < p->value_size; i++) {
    if (strcmp(p->values[i].field, field) == 0 &&
        p->values[i].type == VALUE_ARRAY) {
      (*out) = calloc(p->values[i].array.count, sizeof(int));

      if ((*out) == NULL) {
        fprintf(stderr, "%s\n", strerror(errno));
        return -1;
      }

      (*size) = p->values[i].array.count;
      for (size_t j = 0; j < p->values[i].array.count; j++) {
        (*out)[j] = p->values[i].array.items[j].integer;
      }

      return 0;
    }
  }

  return -1;
}

int parser_get_string_array(const Parser *p, const char *field, char ***out,
                            size_t *size) {
  for (size_t i = 0; i < p->value_size; i++) {
    if (strcmp(p->values[i].field, field) == 0 &&
        p->values[i].type == VALUE_ARRAY) {
      (*out) = calloc(p->values[i].array.count,
                      sizeof(char *) * p->values[i].array.count);

      if ((*out) == NULL) {
        fprintf(stderr, "%s\n", strerror(errno));
        return -1;
      }

      (*size) = p->values[i].array.count;
      for (size_t j = 0; j < p->values[i].array.count; j++) {
        (*out)[j] = strdup(p->values[i].array.items[j].string);
      }

      return 0;
    }
  }

  return -1;
}

static char *unquote_string(const char *str) {
  char *l_unquoted = ltrim_c(str);
  char *r_unquoted = rtrim_c(l_unquoted);

  free(l_unquoted);
  return r_unquoted;
}

int parse_array(ParserArray *p, const char *file_path) {
  FILE *fp = NULL;
  char *line = NULL;
  size_t len = 0;
  size_t parser_index = 0; // TODO: rename item index
  size_t block_index = 0;
  ssize_t read;
  fp = fopen(file_path, "r");

  if (fp == NULL) {
    fprintf(stderr, "%s\n", strerror(errno));
    return errno;
  }

  if (init_parser_array_block(p) != 0) {
    fprintf(stderr, "unable to initialize parser block\n");
  }
  while ((read = getline(&line, &len, fp)) != -1) {

    if (line[0] == '{') {
      if (init_parser_array_item(&p->blocks[block_index]) != 0) {
        fprintf(stderr, "unable to initialize parser item\n");
        return -1;
      }
      parser_index = 0;
      continue;
    }
    if (line[0] == '}') {
      p->block_size++;

      if (p->block_size >= p->block_capacity) {
        if (resize_blocks(p) != 0) {
          return -1;
        }
      }

      block_index++;
      continue;
    }

    if (line[0] == ';') {
      continue;
    }
    if (line[0] == '\n') {
      continue;
    }

    char *token = NULL;
    char *saveptr = NULL;
    int token_count = 0;
    token = strtok_r(line, ":", &saveptr);

    while (token != NULL) {
      char *trimmed = trim(token);
      if (token_count == 0) {
        p->blocks[block_index].values[parser_index].field = strdup(trimmed);
      } else if (token_count == 1) {

        // string value
        // TODO: to func
        if (trimmed[0] == '\"' && trimmed[strlen(trimmed) - 1] == '\"') {
          p->blocks[block_index].values[parser_index].type = VALUE_STRING;
          char *unquoted = unquote_string(trimmed);
          p->blocks[block_index].values[parser_index].string = strdup(unquoted);
        }

        char *end_str = NULL;
        int number_value = strtoll(trimmed, &end_str, 10);
        // int value
        // TODO: to func
        if (end_str != trimmed) {
          p->blocks[block_index].values[parser_index].type = VALUE_INT;
          p->blocks[block_index].values[parser_index].integer = number_value;
          end_str = NULL;
        }

        // if array
        if (trimmed[0] == '[' && trimmed[strlen(trimmed) - 1] == ']') {
          char *trimmed_array = trim_array(trimmed);
          char *internal_token = strtok(trimmed_array, ",");
          bool is_string_arr = false;
          size_t internal_array_index = 0;
          size_t arr_length = length(trimmed);

          p->blocks[block_index].values[parser_index].type = VALUE_ARRAY;
          p->blocks[block_index].values[parser_index].array.count = arr_length;
          p->blocks[block_index].values[parser_index].array.items =
              calloc(arr_length, sizeof(struct value));

          if (internal_token != NULL && internal_token[0] == '\"' &&
              internal_token[strlen(internal_token) - 1] == '\"') {
            is_string_arr = true;
          }

          while (internal_token != NULL) {
            if (is_string_arr == true) {

              p->blocks[block_index]
                  .values[parser_index]
                  .array.items[internal_array_index]
                  .string = strdup(internal_token);
              p->blocks[block_index]
                  .values[parser_index]
                  .array.items[internal_array_index]
                  .type = VALUE_STRING;
            } else {
              p->blocks[block_index]
                  .values[parser_index]
                  .array.items[internal_array_index]
                  .integer = strtol(internal_token, &end_str, 10);
              p->blocks[block_index]
                  .values[parser_index]
                  .array.items[internal_array_index]
                  .type = VALUE_INT;
            }
            internal_array_index++;
            internal_token = strtok(NULL, ",");
          }

          free(trimmed_array);
        }

        free(trimmed);
      } else {
        fprintf(stderr, "token count greater than 1\n");
        return -1;
      }

      token = strtok_r(NULL, ":", &saveptr);

      token_count++;
    }
    p->blocks[block_index].value_size++;

    if (p->blocks[block_index].value_size >=
        p->blocks[block_index].value_capacity) {
      if (resize_items(&p->blocks[block_index]) != 0) {
        return -1;
      }
    }

    parser_index++;
  }

  fclose(fp);
  if (line)
    free(line);
  return 0;
}

int parse(Parser *p, const char *file_path) {
  FILE *fp = NULL;
  char *line = NULL;
  size_t len = 0;
  size_t parser_index = 0;
  ssize_t read;
  fp = fopen(file_path, "r");

  if (fp == NULL) {
    fprintf(stderr, "%s\n", strerror(errno));
    return errno;
  }

  if (init_parser(p) != 0) {
    fprintf(stderr, "unable to init parser\n");
    return -1;
  }

  while ((read = getline(&line, &len, fp)) != -1) {

    if (line[0] == ';') {
      continue;
    }
    if (line[0] == '\n') {
      continue;
    }

    char *token = NULL;
    char *saveptr = NULL;
    int token_count = 0;
    token = strtok_r(line, ":", &saveptr);

    while (token != NULL) {
      if (token_count == 0) {
        p->values[parser_index].field = strdup(token);
      } else if (token_count == 1) {
        char *trimmed = trim(token);

        // string value
        // TODO: to func
        if (trimmed[0] == '\"' && trimmed[strlen(trimmed) - 1] == '\"') {
          p->values[parser_index].type = VALUE_STRING;
          char *unquoted = unquote_string(trimmed);
          p->values[parser_index].string = strdup(unquoted);
          free(unquoted);
        }

        char *end_str = NULL;
        int number_value = strtoll(trimmed, &end_str, 10);
        // int value
        // TODO: to func
        if (end_str != trimmed) {
          p->values[parser_index].type = VALUE_INT;
          p->values[parser_index].integer = number_value;
          end_str = NULL;
        }

        // if array
        if (trimmed[0] == '[' && trimmed[strlen(trimmed) - 1] == ']') {
          char *trimmed_array = trim_array(trimmed);
          char *internal_token = strtok(trimmed_array, ",");
          bool is_string_arr = false;
          size_t internal_array_index = 0;
          size_t arr_length = length(trimmed);

          p->values[parser_index].type = VALUE_ARRAY;
          p->values[parser_index].array.count = arr_length;
          p->values[parser_index].array.items =
              calloc(arr_length, sizeof(struct value));

          if (internal_token != NULL && internal_token[0] == '\"' &&
              internal_token[strlen(internal_token) - 1] == '\"') {
            is_string_arr = true;
          }

          while (internal_token != NULL) {
            if (is_string_arr == true) {

              p->values[parser_index].array.items[internal_array_index].string =
                  strdup(internal_token);
              p->values[parser_index].array.items[internal_array_index].type =
                  VALUE_STRING;
            } else {
              p->values[parser_index]
                  .array.items[internal_array_index]
                  .integer = strtol(internal_token, &end_str, 10);
              p->values[parser_index].array.items[internal_array_index].type =
                  VALUE_INT;
            }
            internal_array_index++;
            internal_token = strtok(NULL, ",");
          }

          free(trimmed_array);
        }

        free(trimmed);
      } else {
        fprintf(stderr, "token count greater than 1\n");
        return -1;
      }

      token = strtok_r(NULL, ":", &saveptr);

      token_count++;
    }
    p->value_size++;

    if (p->value_size >= p->value_capacity) {
      if (resize_parser(p) != 0) {
        return -1;
      }
    }
    parser_index++;
  }

  fclose(fp);
  if (line)
    free(line);
  return 0;
}
