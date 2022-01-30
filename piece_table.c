#include "piece_table.h"


file_table_t* initTable(char* original){
 uintmax_t len = (uintmax_t) strlen(original);
 file_table_t* new_table = malloc(sizeof(file_table_t));
 new_table->file_length = len;
 new_table->edit_count = 0;
 
 edit_t* file_head = malloc(sizeof(edit_t));
 file_head->start = 0;
 file_head->length = len;
 file_head->origin = 0;
 file_head->tail = NULL;
 file_head->parent = NULL;

 for(uintmax_t i = 0; i < len; i++){
  if(original[i] == '\n'){
   file_head->line_count += 1;
  }
 }
 
 file_head->lines = malloc(sizeof(uintmax_t) * file_head->line_count);
 uintmax_t j = 0;
 uintmax_t i = 0; 
 while(i < file_head->line_count){
  if(original[j] == '\n'){
   file_head->lines[i] = j;
   i++;
  }
  j++;
 }

 new_table->head = file_head;
 return new_table;
} 

file_table_t* insertEntry(uintmax_t location, file_table_t* table,
                          uintmax_t buffer_location){
 
 file_table_t* new_table = malloc(sizeof(file_table_t));
 *new_table = *table;
 
 if(location == 0){
  // Should only be called if inserting at the start of a file
  
  edit_t* insertion = malloc(sizeof(edit_t));
  insertion->start = buffer_location;
  insertion->length = 1;
  insertion->origin = 1;
  insertion->tail = new_table->head;
  new_table->head->parent = insertion;

  new_table->file_length += 1;
  new_table->head = insertion;
  free(table);
  return new_table;
 }
 
 edit_t* current = new_table->head;
 uintmax_t i = 0;
 while(i <= location){
  i += current->length;
  if(current->tail == NULL){
   break;
  }
  current = current->tail;
 }

 if(location == new_table->file_length){
  // Is only called if insertion is at the end of the file
  if((current->origin == 1) &&
     ((current->start + current->length) == buffer_location)){
   // ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
   // The above code checks if the edit directly before this
   // insertion is next to this edit in the append buffer.
   // If it is, we simply add to the length of the current edit
   current->length += 1;
   free(table);
   new_table->file_length += 1;
   return new_table;
  }
  
  edit_t* insertion = malloc(sizeof(edit_t));
  insertion->start = buffer_location;
  insertion->length = 1;
  insertion->parent = current;
  insertion->origin = 1;
  insertion->tail = NULL;
  current->tail = insertion;
  
  new_table->file_length += 1;
  free(table);
  return new_table;
 }

 if(i == location){
  // Should only be called if an edit starts exactly the next
  // character after the end of one edit
  if((current->origin == 1) &&
     ((current->start + current->length) == buffer_location)){
   // ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
   // The above code checks if the edit directly before this
   // insertion is next to this edit in the append buffer.
   // If it is, we simply add to the length of the current edit
   current->length += 1;
   free(table);
   new_table->file_length += 1;
   return new_table;
  }
  
  edit_t* insertion = malloc(sizeof(edit_t));
  insertion->length = 1;
  insertion->start = buffer_location;
  insertion->origin = 1;
  insertion->tail = current->tail;
  insertion->parent = current;
  if(insertion->tail != NULL){
   insertion->tail->parent = insertion;
  }
  current->tail = insertion;
  
  free(table);
  new_table->file_length += 1;
  return new_table;
 }
 
 if((current->origin == 1) &&
    ((current->start + current->length) == buffer_location)){
  // ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
  // The above code checks if the edit directly before this
  // insertion is next to this edit in the append buffer.
  // If it is, we simply add to the length of the current edit
  current->length += 1;
  free(table);
  new_table->file_length += 1;
  return new_table;
 }
 
 uintmax_t split_distance = i - location;
 uintmax_t remaining_distance = current->length - split_distance;

 edit_t* insertion = malloc(sizeof(edit_t));
 edit_t* remainder = malloc(sizeof(edit_t));
 remainder->tail = current->tail;
 remainder->length = remaining_distance;
 remainder->origin = current->origin;
 remainder->start = current->start + split_distance;
 if(remainder->tail != NULL){
  remainder->tail->parent = remainder;
 }
 remainder->parent = insertion;
 
 insertion->start = buffer_location;
 insertion->length = 1;
 insertion->origin = 1;
 insertion->parent = current;
 insertion->tail = remainder;
 
 current->length = split_distance;
 
 free(table);
 new_table->file_length += 1;
 return new_table;
}

