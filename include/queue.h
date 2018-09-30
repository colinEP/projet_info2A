#ifndef _QUEUE_H_
#define _QUEUE_H_

#include <stdio.h>

typedef struct queue
{
  void * element;
  struct queue* next;

} *QUEUE;


QUEUE add_to_queue(char* token, int type, int numline);


QUEUE new_queue();


QUEUE ajouter_fin(QUEUE l, LEXEM lex);

#endif
