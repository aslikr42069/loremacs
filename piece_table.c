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

 uintmax_t i = 0;
 uintmax_t start;
 edit_t* current = table->head;
 while(i < location){
  i += current->length;
  if(current->tail == NULL || i >= location){
   break;
  }
  current = current->tail;
 }

 start = i - current->length;
 if(location == i && current->origin == 1 &&
    current->start + current->length == buffer_location){
  current->length += 1;
  return;
 }

 if(location == i){
   edit_t* insertion = malloc(sizeof(edit_t));
   insertion->start = buffer_location;
   insertion->length = 1;
   insertion->origin = 1;
   insertion->tail = current->tail;
   insertion->parent = current;
   if(current->tail != NULL){
     current->tail->parent = insertion;
   }
   current->tail = insertion;
   return;
 }

 if(location == start){
   edit_t* insertion = malloc(sizeof(edit_t));
   insertion->start = buffer_location;
   insertion->length = 1;
   insertion->origin = 1;
   insertion->tail = current;
   current->parent->tail = insertion;
   insertion->parent = current->parent;
   current->parent = insertion;
   return;
 }
 
 printf("Start: %lu\ni:%lu\nlocation:%lu\n", start, i, location);
 edit_t* insertion = malloc(sizeof(edit_t));
 edit_t* post_insertion = malloc(sizeof(edit_t));
 uintmax_t split_length_1 = location - start;
 uintmax_t split_length_2 = i - location;
 
 post_insertion->tail = current->tail;
 post_insertion->parent = insertion;
 post_insertion->length = split_length_2;
 post_insertion->start = current->start + split_length_1;
 post_insertion->origin = current->origin;

 insertion->start = buffer_location;
 insertion->length = 1;
 insertion->origin = 1;
 insertion->tail = post_insertion;
 insertion->parent = current;

 current->length = split_length_1;
 current->tail = insertion;

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
     putchar(fullText[i + j]);
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

void deleteEntry(uintmax_t location, piece_table_t* table){
 if(table->file_length == 0){
  puts("Length == 0");
  return; // Just to save ourselves from memory errors in the future
 }
 
 uintmax_t i = 0;
 edit_t* current = table->head;
 uintmax_t start, end;
 while(i < location){
  i += current->length;
  if(current->tail == NULL){
   break;
  }
  current = current->tail;
 }
 if(i == 0){
   i = current->length;
 }
 start = i - current->length;
 end = i - 1;

 if(current->length == 1){
  if(current->parent == NULL && current->tail == NULL){
   // buffer with only one char and 0 edits, can't
   // delete node or else no head exists.
   current->length = 0;
  } else {
   if(current->tail != NULL){
    current->tail->parent = current->parent;
   }
   if(current->parent != NULL){
    current->parent->tail = current->tail;
   }
   free(current); 
  }
  return;
 }

 if(location == start){
  current->start += 1;
  current->length -= 1;
  return;
 }
 
 if(location == end){
  current->length -= 1;
  return;
 }
 current->length = location - start;
 edit_t* insertion = malloc(sizeof(edit_t));
 insertion->start = current->start + current->length + 1;
 insertion->length = end - location;
 insertion->origin = current->origin;
 insertion->tail = current->tail;
 if(current->tail != NULL){
  current->tail->parent = insertion;
 }
 insertion->parent = current;
 current->tail = insertion; 
 return;
}

