#include <stdio.h>
#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include "piece_table.h"

int main(){
 char *test_origin = "eo wfd";
 char *test_buffer = "hllorl!";
 piece_table_t* table = initTable(test_origin);
 insertEntry(0, table, 0);
 insertEntry(2, table, 1);
 insertEntry(3, table, 2);
 insertEntry(9, table, 6);
 char *read_buffer = readTable(table, test_origin, test_buffer);
 printTable(table);
 printf("%s\n", read_buffer);
 freeTable(table);
 free(read_buffer);
 return 0; 
}

