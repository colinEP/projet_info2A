#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <strings.h>

#include <global.h>
#include <notify.h>
#include <lex.h>
#include <queue_list.h>
#include <dictionnaire.h>
#include <fct_analyse_1.h>
#include <etiq.h>





QUEUE new_queue()
{
  return NULL;
}


QUEUE ajouter_fin(QUEUE Q, void* element) //à vérfier
{
  if (Q == NULL)
    {
      Q = calloc (1, sizeof(*Q));
      Q->element = element;
      Q->next = Q;
      return Q;
    }
  QUEUE adresse_retour = Q->next;
  Q->next = calloc (1, sizeof(*Q));
  Q = Q->next;
  Q->element = element;
  Q->next = adresse_retour;
  return Q;
}



QUEUE add_to_queue_lex(QUEUE Q, char* token, int type, int numline)
{
    LEXEM lex = calloc (1, sizeof(*lex));
    lex->nline = numline;
    lex->lex_type = type;
    lex->value = strdup(token);
    Q = ajouter_fin(Q, lex);

    return Q;
}


LIST add_to_head_list(LIST L, void* element)
{
  if (L == NULL)
    {
      L = calloc (1, sizeof(*L));
      L->element = element;
      L->next = NULL;
      return L;
    }
  LIST new_list = calloc (1, sizeof(*new_list));
  new_list-> element = element;
  new_list-> next = L;
  return new_list;
}

LIST add_to_end_list(LIST l, void* element) {
    if (l==NULL) {
        LIST l = calloc(1, sizeof(*l) );
        l->element = element;
        l->next = NULL;
        return l;
    }
    LIST p;
    for (p=l ; p->next!=NULL ; p=p->next);
    LIST new = calloc(1, sizeof(*new) );
    p->next = new;
    new->element = element;
    new->next = NULL;
    return l;
}

LIST revers_list(LIST l) {

    if (l==NULL) return l;   // cas liste vide

    LIST l_next = l->next;
    l->next     = NULL;
    LIST l_prev = l;
    l           = l_next;

    for ( ; l!=NULL ; l=l_next ) {
        l_next  = l->next;
        l->next = l_prev;
        l_prev  = l;
    }
    return l_prev;
}

LIST new_list() {
  return NULL;
}

LIST queue_to_list( QUEUE q ) {
    LIST l=q->next;
    q->next=NULL;
    return l;
}

int lengh_of_list(LIST l) {

    if (l==NULL) return 0;   // cas liste vide
    int i =0;
    while( l!=NULL ) {
        i = i+1;
        l = l->next;
    }
    return i;
}



/** Cette fonction n'est pas générique pour le type abstrait list
 *  Elle libère seulement les listes de lexeme
 */
void free_list_lex(LIST l) {
    if (l==NULL) return;
	else {
		free_list_lex( l->next);
        free( ( (LEXEM) (l->element) ) ->value);    // conversion du type void* en type LEXEM
        //free_lex(l->element);      // en passant free_lex en paramètre, on pourra rendre la fonction générique
		free(l->element);
		free(l);
	}
}

void free_list_dico(LIST l) {
    if (l==NULL) return;
    else {
        free_list_dico( l->next );
        free( ( (DICO_LINE) (l->element) ) ->instruction);
        free( ( (DICO_LINE) (l->element) ) ->arg_type_1);
        free( ( (DICO_LINE) (l->element) ) ->arg_type_2);
        free( ( (DICO_LINE) (l->element) ) ->arg_type_3);
        free( ( (DICO_LINE) (l->element) ) ->code_bin_1);
        free( ( (DICO_LINE) (l->element) ) ->code_bin_2);
        free( ( (DICO_LINE) (l->element) ) ->code_bin_3);
        free( ( (DICO_LINE) (l->element) ) ->code_bin_4);
        free( ( (DICO_LINE) (l->element) ) ->code_bin_5);

        free(l->element);
        free(l);
    }
}

void free_list_inst(LIST l) {
    if (l==NULL) return;
    else {
        free_list_inst(l->next);
        // lex free avec free_list_lex
        /* free ARG_INT */   // on suppose val.char_chain déjà free
        free( ( (INSTR)l->element )->arg1);
        free( ( (INSTR)l->element )->arg2);
        free( ( (INSTR)l->element )->arg3);

        free(l->element);
        free(l);
    }
}

void free_list_data(LIST l) {
    if (l==NULL) return;
    else {
        free_list_data(l->next);

        /* free data_op */
        free_data_op( ( (DATA)l->element )->D);

        free(l->element);
        free(l);
    }
}

void free_data_op(data_op d) {
    if      (d->type==PASCIIZ) free(d->val.PASCIIZ);   // avant c'était la meme str que dans le lexem, mais maintenant y'a un strdup entre
    else if (d->type==LABEL)   free(d->val.LABEL);     // impossible ? car transformé en DEC_LABEL
    free(d);
}

void free_symb_table(LIST l) {
    if (l==NULL) return;
    else {
        free_symb_table(l->next);

        free( ( (ETIQ)l->element )->name);

        free(l->element);
        free(l);
    }
}

void free_sort_symb_tab(LIST l) {
    if (l==NULL) return;
    else {
        free_sort_symb_tab(l->next);

        //free( ( (ETIQ)l->element )->name); // DEJA FREE PAR free_symb_table

        //free(l->element);                  // DEJA FREE PAR free_symb_table
        free(l);
    }
}


void free_reloc_table(LIST l) {
    if (l==NULL) return;
    else {
        free_reloc_table(l->next);

        free( ( (RELOC)l->element )->section);
        free( ( (RELOC)l->element )->addend);

        free(l->element);
        free(l);
    }
}

// void free_list_(LIST l) {
//     if (l==NULL) return;
//     else {
//         free_list_(l->next);
//
//         free(l->element);
//         free(l);
//     }
// }


/* pour l'instant elle n'est pas utilisée */
void free_lex(void* lex_ambigu) {
    LEXEM lex = (LEXEM)lex_ambigu;
    free(lex->value);
}
