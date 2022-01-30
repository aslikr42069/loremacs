#include <stdio.h>
#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include "piece_table.h"

int main(){
 char *test_origin = "eo wfd";
 char *test_buffer = "hllorl";
 file_table_t* table = initTable(test_origin);
 table = insertEntry(0, table, 0);
 free(table);
 return 0; 
}

