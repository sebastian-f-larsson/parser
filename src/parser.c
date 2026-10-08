#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <ctype.h>
#include "parser.h"

struct Parser {
  struct value *value;
  size_t value_size;
  size_t value_capacity;
};


Parser *parser_create(void) {
  Parser *p = calloc(1, sizeof(*p));

  if(p == NULL) {
    fprintf(stderr, "%s\n", strerror(errno));
    return NULL;
  }

  return p;
}

static int init_parser(Parser *p) {
  p->value_capacity = 10;
  p->value_size = 0;
  p->value = calloc(p->value_capacity, sizeof(struct value));

  if(p->value == NULL) {
    fprintf(stderr, "%s\n", strerror(errno));
    return -1;
  }

  return 0;
}

static int resize_parser(Parser *p) {

  p->value_capacity *= 2;

  struct value *tmp = realloc(p->value, p->value_capacity * sizeof(struct value));

  if(tmp == NULL) {
    fprintf(stderr, "%s\n", strerror(errno));
    return -1;
  }

  p->value = tmp;

  return 0;
}

void destroy_parser(Parser **p) {
  for(size_t i = 0; i < (*p)->value_size; i++) {
    switch((*p)->value[i].type) {
      case VALUE_INT:
        break;
      case VALUE_STRING:
        free((*p)->value[i].string);
        break;
      case VALUE_ARRAY: 
        for(size_t j = 0; j < (*p)->value[i].array.count; j++) {
          if((*p)->value[i].array.items[j].type == VALUE_STRING) {
            free((*p)->value[i].array.items[j].string);
          }
        }

        free((*p)->value[i].array.items);
    }

    free((*p)->value[i].field);
  }

  free((*p)->value);
  free((*p));
}

static char *ltrim_c(char *str) {
  size_t new_size = strlen(str) - 1;

  char *new_string = calloc(new_size + 1, sizeof(char));

  if(new_string == NULL) {
    fprintf(stderr, "%s\n", strerror(errno));
    return NULL;
  }

  strncpy(new_string, str + 1, new_size);

  return new_string;
}

static char *rtrim_c(char *str) {
  size_t new_size = strlen(str) - 1;

  char *new_string = calloc(new_size + 1, sizeof(char));

  if(new_string == NULL) {
    fprintf(stderr, "%s\n", strerror(errno));
    return NULL;
  }

  memcpy(new_string, str, new_size);

  return new_string;
}

char *trim_array(char *str) {
  char *ltrim_arr = ltrim_c(str);
  char *rtrim_arr = rtrim_c(ltrim_arr);
  free(ltrim_arr);
  return rtrim_arr;
}


static char *ltrim(char *str) {
  if(isspace(str[0]) == 0) return str;

  int index = 0;

  while(isspace(str[index]) != 0) {
    index++;
  }

  size_t new_size = strlen(str) - index + 1;

  char *new_string = calloc(new_size, sizeof(char));

  if(new_string == NULL) {
    fprintf(stderr, "%s\n", strerror(errno));
    return NULL;
  }

  strncpy(new_string, str + index, new_size);

  return new_string;
}

static char *rtrim(char *str) {
  size_t end = strlen(str);
  
  if(isspace(str[end-1]) == 0) {
    printf("no space");
    return str;
  }

  int new_size = end;

  for(size_t i = 0; i < end; i++) {
    if(isspace(str[i]) != 0) {
      new_size = i;
    }
  }

  char *new_string = calloc(new_size + 1, sizeof(char));

  if(new_string == NULL) {
    fprintf(stderr, "%s\n", strerror(errno));
    return NULL;
  }

  strncpy(new_string, str, new_size);

  return new_string;
}

static size_t length(const char *arr) {
  size_t arr_size = 1;
  size_t arr_len = strlen(arr);

  if(arr[0] == '\0') return 0;

  for(size_t i = 0; i < arr_len; i++) {
    if(arr[i] == ',') arr_size++;
  }

  return arr_size;
}

static char *trim(char *str) {
  char *lt = ltrim(str);
  char *rt = rtrim(lt);
  free(lt);
  return rt;
}

int parser_get_int(const Parser *p, const char *field, int *out) {
  for(size_t i = 0; i < p->value_size; i++) {
    if(strcmp(p->value[i].field, field) == 0 && p->value[i].type == VALUE_INT) {
      (*out) = p->value[i].integer;
      return 0; 
    }
  }

  return -1;
}

int parser_get_string(const Parser *p, const char *field, char **out) {
  for(size_t i = 0; i < p->value_size; i++) {
    if(strcmp(p->value[i].field, field) == 0 && p->value[i].type == VALUE_STRING) {
    (*out) = strdup(p->value[i].string);
      return 0;
    }
  }

  return -1;
}

int parser_get_int_array(const Parser *p, const char *field, int **out, size_t *size) {
  for(size_t i = 0; i < p->value_size; i++) {
    if(strcmp(p->value[i].field, field) == 0 && p->value[i].type == VALUE_ARRAY) {
      (*out)  = calloc(p->value[i].array.count, sizeof(int));

      if((*out) == NULL) {
        fprintf(stderr, "%s\n", strerror(errno));
        return -1;
      }

      (*size) = p->value[i].array.count;
      for(size_t j = 0; j < p->value[i].array.count; j++) {
        (*out)[j] = p->value[i].array.items[j].integer;
      }

      return 0;
    }
  }

  return -1;
}

int parser_get_string_array(const Parser *p, const char *field, char ***out, size_t *size) {
  for(size_t i = 0; i < p->value_size; i++) {
    if(strcmp(p->value[i].field, field) == 0 && p->value[i].type == VALUE_ARRAY) {
      (*out)  = calloc(p->value[i].array.count, sizeof(char *) * p->value[i].array.count);

      if((*out) == NULL) {
        fprintf(stderr, "%s\n", strerror(errno));
        return -1;
      }

      (*size) = p->value[i].array.count;
      for(size_t j = 0; j < p->value[i].array.count; j++) {
        (*out)[j] = strdup(p->value[i].array.items[j].string);
      }

      return 0;
    }
  }

  return -1;
}

int parse(Parser *p, const char *file_path) {
  FILE *fp = NULL;
  char *line = NULL;
  size_t len = 0;
  size_t parser_index = 0;
  ssize_t read;
  fp = fopen(file_path, "r");

  if(fp == NULL) {
    fprintf(stderr, "%s\n", strerror(errno));
    return errno;
  }

  if(init_parser(p) != 0) {
    fprintf(stderr, "unable to init parser\n");
    return -1;
  }

  while((read = getline(&line, &len, fp)) != -1) {
  
    if(line[0] == ';') {
      continue;
    }
    if(line[0] == '\n') {
      continue;
    }

    char *token = NULL;
    char *saveptr = NULL;
    int token_count = 0;
    token = strtok_r(line, ":", &saveptr);

    while(token != NULL) {
      if(token_count == 0) {
        p->value[parser_index].field = strdup(token);
      }
      else if(token_count == 1) {
        char *trimmed = trim(token);

        // string value 
        // TODO: to func
        if(trimmed[0] == '\"' && trimmed[strlen(trimmed) - 1] == '\"') {
          p->value[parser_index].type = VALUE_STRING;
          p->value[parser_index].string = strdup(trimmed);
        }
        
        char *end_str = NULL;
        int number_value = strtoll(trimmed, &end_str, 10);
        // int value
        // TODO: to func
        if(end_str != trimmed) {
          p->value[parser_index].type = VALUE_INT;
          p->value[parser_index].integer = number_value;
          end_str = NULL;
        }

        // if array
        if(trimmed[0] == '[' && trimmed[strlen(trimmed) - 1] == ']') {
          char *trimmed_array = trim_array(trimmed);
          char *internal_token = strtok(trimmed_array, ",");
          bool is_string_arr = false;
          size_t internal_array_index = 0;
          size_t arr_length = length(trimmed);

          p->value[parser_index].type = VALUE_ARRAY; 
          p->value[parser_index].array.count = arr_length;
          p->value[parser_index].array.items = calloc(arr_length, sizeof(struct value));

           if(internal_token != NULL && internal_token[0] == '\"' && internal_token[strlen(internal_token) - 1] == '\"') {
            is_string_arr = true;
          }

          while(internal_token != NULL) {
            if(is_string_arr == true)  {

              p->value[parser_index].array.items[internal_array_index].string = strdup(internal_token);
              p->value[parser_index].array.items[internal_array_index].type = VALUE_STRING;
            }
            else {
              p->value[parser_index].array.items[internal_array_index].integer = strtol(internal_token, &end_str, 10);
              p->value[parser_index].array.items[internal_array_index].type = VALUE_INT;
            }
            internal_array_index++;
            internal_token = strtok(NULL, ",");
          }

          free(trimmed_array);
        }

        free(trimmed);
      }
      else {
        fprintf(stderr, "token count greater than 1\n");
        return -1;
      }

      token = strtok_r(NULL, ":", &saveptr);

      token_count++;
    }
      p->value_size++;

      if(p->value_size >= p->value_capacity) {
        if(resize_parser(p) != 0) {
          return -1;
      }
    }
      parser_index++;
  }

  fclose(fp);
  if(line) free(line);
  return 0;
}


