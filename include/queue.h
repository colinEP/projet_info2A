#ifndef _QUEUE_H_
#define _QUEUE_H_

#include <stdio.h>
#include <lex.h>


typedef struct queue
{
  void * element;
  struct queue* next;

} *QUEUE;


typedef struct
{
    int nline;
    //int pos;
    int lex_type;
    //char* lex_type_print;
    char* value;
} *LEXEM;


QUEUE add_to_queue(char* token, int type, int numline, QUEUE Q);


QUEUE new_queue();


QUEUE ajouter_fin(QUEUE l, LEXEM lex);


#endif
