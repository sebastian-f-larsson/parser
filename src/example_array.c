#include "parser.h"
#include <stdio.h>
#include <stdlib.h>

int main(void) {
  ParserArray *p = parser_array_create();
  int id;
  char *name;
  int *quests = NULL;
  int health;
  char **actions = NULL;
  size_t actions_size;

  parse_array(p, "file_array.dat");

  const int index = parser_array_find_item_index_int(p, "id", 2);
  if (index < 0) {
    fprintf(stderr, "index not found");
    destroy_parser_array(&p);
    return -1;
  }

  printf("index found: %d\n", index);

  parser_array_get_string(p, "name", index, &name);
  parser_array_get_int(p, "health", index, &health);
  parser_array_get_array_string(p, "str", index, &actions, &actions_size);
  printf("name: %s\n", name);
  printf("health: %d\n", health);
  printf("actions: \n");
  for(size_t i = 0; i < actions_size; i++) {
    printf("\t%s\n", actions[i]);
  }

  for(size_t i = 0; i < actions_size; i++) {
    free(actions[i]);
  }

  free(actions);
  /*  parser_get_int(p, "id", &id);
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
    free(name);*/
  destroy_parser_array(&p);
  return 0;
}
