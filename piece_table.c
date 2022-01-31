#include "piece_table.h"


piece_table_t* initTable(char* original){
 uintmax_t len = (uintmax_t) strlen(original);
 piece_table_t* new_table = malloc(sizeof(piece_table_t));
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
 if(file_head->line_count > 1){
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
 }
 new_table->head = file_head;
 return new_table;
} 

void insertEntry(uintmax_t location, piece_table_t* table,
                 uintmax_t buffer_location){

  table->edit_count += 1;
  table->file_length += 1;
 
 if(location == 0){
  // Should only be called if inserting at the start of a file
  
  edit_t* insertion = malloc(sizeof(edit_t));
  insertion->start = buffer_location;
  insertion->length = 1;
  insertion->origin = 1;
  insertion->tail = table->head;
  insertion->parent = NULL;
  table->head->parent = insertion;
  table->head = insertion;
  return;
 }

 edit_t* current = table->head;
 uintmax_t i = 0;
 while(i < location){
   i += current->length;
   if(current->tail != NULL){
    current = current->tail;
   } else {
     break;
   }
 }

 if(location == table->file_length - 1){
   // Should only be reachable if the edit is at
   // the very end of the file

  if(current->parent != NULL && current->parent->origin == 1 &&
     current->parent->start + current->parent->length == buffer_location){
    // Check if its neighbour behind it is also next to it on the buffer
    // and if so, simply extend the length of the neighbour
    current->parent->length += 1;
    return;
  }
  edit_t* insertion = malloc(sizeof(edit_t));
  insertion->start = buffer_location;
  insertion->length = 1;
  insertion->origin = 1;
  insertion->tail = NULL;
  insertion->parent = current;
  current->tail = insertion;
  cleanTable(table);
  return;
 }
  
 uintmax_t split_remaining_length = i - location;
 i = i - current->length;
 uintmax_t split_start_length = location - i;

 if(split_remaining_length == 0){
   if(current->parent != NULL && current->parent->origin == 1 &&
    current->parent->start + current->parent->length == buffer_location){
   // Check if its neighbour behind it is also next to it on the buffer
   // and if so, simply extend the length of the neighbour
   current->parent->length += 1;
   cleanTable(table);
   return;
  }
 }

 current->length = split_start_length;
 edit_t* insertion = malloc(sizeof(edit_t));
 edit_t* post_insertion = malloc(sizeof(edit_t));
 insertion->start = buffer_location;
 insertion->length = 1;
 insertion->origin = 1;
 insertion->tail = post_insertion;
 insertion->parent = current;
 post_insertion->tail = current->tail;
 post_insertion->origin = 0;
 if(current->tail != NULL){
  current->tail->parent = post_insertion;
 }
 post_insertion->length = split_remaining_length;
 post_insertion->start = current->start + current->length;
 post_insertion->parent = insertion;
 current->tail = insertion;
 cleanTable(table);
 return;
}


char* readTable(piece_table_t* table, char* original, char* append){
 char *fullText = malloc(sizeof(char) * table->file_length);
 uintmax_t i = 0;
 uintmax_t iter = 0;
 edit_t* current = table->head;

 while(i < table->file_length && current != NULL){
  iter++;
  switch(current->origin){
   case 0:
    for(uintmax_t j = 0; j < current->length; j++){
     fullText[i + j] = original[current->start + j];
    }
    break;
   case 1:
    for(uintmax_t j = 0; j < current->length; j++){
     fullText[i + j] = append[current->start + j];
    }
    break;
   default:
    break;
  }
  i += current->length;
  current = current->tail;
 }

 return fullText;
}

void freeTable(piece_table_t* table){
 edit_t* freeMe = table->head;
 edit_t* myTail = freeMe->tail;
 while(myTail != NULL){
  if(freeMe->line_count > 1){
   free(freeMe->lines);
  }
  free(freeMe);
  freeMe = myTail;
  myTail = freeMe->tail;
 }
 free(freeMe);
 free(table);
}

void cleanTable(piece_table_t* table){
 edit_t* current;
 current = table->head;
 edit_t* myTail = current->tail;
 while(current != NULL){
  if(current->length == 0 && current->parent != NULL){
   // Checking to see if node is empty and not just
   // an empty file. If not just an empty file,
   // delete the node.
   current->parent->tail = current->tail;
   if(myTail != NULL){
    myTail->parent = current->parent;
   }
   free(current);
  }
  current = myTail;
  if(current != NULL){
  myTail = current->tail;
  }
 }

current = table->head;
while(current != NULL){
  if((current->tail != NULL) && current->origin == 1 &&
     current->tail->origin == 1){
   if(current->start + current->length == current->tail->start){
    current->length += current->tail->length;
    edit_t* tmp_tail = current->tail;
    current->tail = current->tail->tail;
    if(current->tail != NULL){
     current->tail->parent = current;
    }
    free(tmp_tail);
   }
  }
 current = current->tail;
 }
 
 
}

void printTable(piece_table_t* table){
 printf("Start:\tLength:\tOrigin:\n");
 edit_t* current = table->head;
 while(current != NULL){
  printf("%ld\t%ld\t", current->start, current->length);
  switch(current->origin){
   case 0:
    printf("Original\n");
    break;
   case 1:
    printf("Append\n");
    break;
   default:
    break;
  }
  current = current->tail;
 }
}

