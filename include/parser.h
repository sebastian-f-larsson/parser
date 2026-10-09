#ifndef LIBPARSER_H
#define LIBPARSER_H


#include <stddef.h>

typedef enum { VALUE_INT, VALUE_STRING, VALUE_ARRAY } ValueType;

struct value {
  ValueType type;
  char *field;

  union {
    int integer;
    char *string;
    
    struct {
      struct value *items;
      size_t count;
    } array;
  };
};

typedef struct Parser Parser;

Parser *parser_create(void);
int parse(Parser *p, const char *file_path);
int parser_get_string_array(const Parser *p, const char *field, char ***out, size_t *size);
int parser_get_int_array(const Parser *p, const char *field, int **out, size_t *size);
int parser_get_string(const Parser *p, const char *field, char **out);
int parser_get_int(const Parser *p, const char *field, int *out);
void destroy_parser(Parser **p);
#endif
