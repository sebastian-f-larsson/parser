#ifndef LIBPARSER_H
#define LIBPARSER_H

#include <stdbool.h>
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

typedef struct ParserArray ParserArray;
typedef struct Parser Parser;

Parser *parser_create(void);

int parse(Parser *p, const char *file_path);
int parser_get_string_array(const Parser *p, const char *field, char ***out,
                            size_t *size);
int parser_get_int_array(const Parser *p, const char *field, int **out,
                         size_t *size);
int parser_get_string(const Parser *p, const char *field, char **out);
int parser_get_int(const Parser *p, const char *field, int *out);
void destroy_parser(Parser **p);

ParserArray *parser_array_create(void);
int parse_array(ParserArray *p, const char *file_path);
int parser_array_find_item_index_int(const ParserArray *p, const char *field,
                                     const int value);
int parser_array_get_string(const ParserArray *p, const char *field,
                            const int index, char **out);
int parser_array_get_int(const ParserArray *p, const char *field,
                         const int index, int *out);
size_t parser_array_count(const ParserArray *p);
void destroy_parser_array(ParserArray **p);

#endif
