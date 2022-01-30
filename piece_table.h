#ifndef PIECE_TABLE_H
#define PIECE_TABLE_H

#include <stdint.h>
#include <stddef.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>


typedef struct edit_t {
 uintmax_t start;
 uintmax_t length;
 int origin; // 0 = original file, 1 = append buffer
 uintmax_t line_count;
 uintmax_t *lines;

 struct edit_t *tail;
 struct edit_t *parent;
} edit_t;

typedef struct file_table_t {
 uintmax_t file_length;
 uintmax_t edit_count;
 edit_t *head;
} file_table_t;

file_table_t* insertEntry(uintmax_t location, file_table_t* table,
                          uintmax_t buffer_location);

char* readTable(file_table_t* table, char* original, char* append);

file_table_t* initTable(char* original);

/*file_table_t* deleteEntry(uintmax_t location, file_table_t* table);

file_table_t* deleteBatch(uintmax_t location, file_table_t* table,
                          uintmax_t length);
*/
#endif
