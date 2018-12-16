#ifndef _QUEUE_LIST_H_
#define _QUEUE_LIST_H_

#include <stdio.h>
//#include <fct_analyse_1.h>


typedef struct d_op* data_op;   // impossible d'inclure car inclusion croisé
typedef struct lex_t* LEXEM;

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

int lengh_of_list(LIST l);

LIST queue_to_list  ( QUEUE q );
void free_list_lex  ( LIST l );
void free_lex       ( LEXEM lex );
void free_list_dico ( LIST l );
void free_list_inst ( LIST l );
void free_list_data ( LIST l );
void free_data_op   ( data_op d );
void free_symb_table( LIST l );
void free_sort_symb_tab( LIST l );
void free_reloc_table  (LIST l);
void free_sym_char  (char** sym_char, int size );



#endif
