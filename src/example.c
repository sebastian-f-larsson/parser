#include <stdlib.h>
#include <stdio.h>
#include "parser.h"


int main(int argc, char **argv) {
  if(argc < 2) return 0;

  Parser *p = parser_create();
  int id;
  char *name;
  size_t quest_size;
  int *quests = NULL;
  char **str_array = NULL;
  size_t str_arr_size;

  parse(p, argv[1]);
  parser_get_int(p, "id", &id);
  parser_get_string(p, "name", &name);
  parser_get_int_array(p, "quest", &quests, &quest_size);
  parser_get_string_array(p, "str", &str_array, &str_arr_size);

  printf("npc id: %d\n", id);
  printf("npc name: %s\n", name);
  printf("quests %zu: ", quest_size);
  for(size_t i = 0; i < quest_size; i++) {
    printf(" %d ", quests[i]);
  }
  printf("\n");

  printf("str array %zu :", str_arr_size);
  for(size_t i = 0; i < str_arr_size; i++) {
    printf(" %s ", str_array[i]);
  }

  printf("\n");
  for(size_t i = 0; i < str_arr_size; i++) {
    free(str_array[i]);
  }
  free(str_array);
  free(quests);
  free(name);
  destroy_parser(&p);
  return 0; 
}
