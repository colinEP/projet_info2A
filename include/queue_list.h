#ifndef _QUEUE_LIST_H_
#define _QUEUE_LIST_H_

#include <stdio.h>
//#include <lex.h>


typedef struct queue
{
  void* element;
  struct queue* next;

} *QUEUE;


QUEUE add_to_queue(QUEUE Q, char* token, int type, int numline);

QUEUE new_queue();

QUEUE ajouter_fin(QUEUE l, void* structure);


typedef QUEUE LIST;

LIST new_list();
LIST queue_to_list( QUEUE q );
void free_list_lex( QUEUE l );
void free_lex( void* lex );
void print_list_lex( LIST l );

#endif
