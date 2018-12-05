#ifndef _QUEUE_LIST_H_
#define _QUEUE_LIST_H_

#include <stdio.h>
//#include <lex.h>


typedef struct queue
{
  void* element;
  struct queue* next;

} *QUEUE;

typedef QUEUE LIST;


QUEUE add_to_queue_lex(QUEUE Q, char* token, int type, int numline);

QUEUE new_queue();

QUEUE ajouter_fin(QUEUE l, void* element);

LIST add_to_head_list(LIST L, void* element);
LIST add_to_end_list (LIST L, void* element);
LIST revers_list(LIST l);

LIST new_list();

LIST queue_to_list( QUEUE q );
void free_list_lex( LIST l );
void free_list_dico(LIST l);
//void free_list_lex( QUEUE l );

void free_lex( void* lex );

void free_list_lex( QUEUE l );

#endif
