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

<<<<<<< HEAD
QUEUE ajouter_fin(QUEUE l, LEXEM lex);
void read_queue_lex(QUEUE Q);
=======
QUEUE ajouter_fin(QUEUE l, void* structure);

>>>>>>> 53dbb7bc4bcdbdc18c8b9344c0563c764896a5e4

typedef QUEUE LIST;

LIST new_list();
LIST queue_to_list( QUEUE q );
void free_list_lex( QUEUE l );
void free_lex( void* lex );
void print_list_lex( LIST l );

#endif
