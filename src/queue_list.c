#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <strings.h>

#include <global.h>
#include <notify.h>
#include <lex.h>
#include <queue_list.h>





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



QUEUE add_to_queue(QUEUE Q, char* token, int type, int numline)
{
    LEXEM lex = calloc (1, sizeof(*lex));
    lex->nline = numline;
    lex->lex_type = type;
    lex->value = strdup(token);
    Q = ajouter_fin(Q, lex);

    return Q;
}


LIST new_list() {
  return NULL;
}

LIST queue_to_list( QUEUE q ) {
    LIST l=q->next;
    q->next=NULL;
    return l;
}

/** Cette fonction n'est pas générique pour le type abstrait list
 *  Elle libère seulement les listes de lexeme
 */
void free_list_lex(QUEUE l) {
    if (l==NULL) return;
	else {
		free_list_lex( l->next);
        free( ( (LEXEM) (l->element) ) ->value);    // conversion du type void* en type LEXEM
        //free_lex(l->element);      // en passant free_lex en paramètre, on pourra rendre la fonction générique
		free(l->element);
		free(l);
	}
}

/* pour l'instant elle n'est pas utilisée */
void free_lex(void* lex_ambigu) {
    LEXEM lex = (LEXEM)lex_ambigu;
    free(lex->value);
}


void print_list_lex( LIST l ) {
    LIST p;
    LEXEM lex;
    printf("\n====== Liste des lexemes ======\n");
    printf("\n       |            |\n");
    printf(" nline |    Type    |    Value\n");
    printf("       |            |\n");
    for ( p=l ; p!=NULL ; p=p->next ) {
        lex = p->element;
        //printf("valeur : %s\ntype: %s  \nnum_ligne : %d \n\n", lex->value, lex_type_string(lex->lex_type, TRUE), lex->nline);
        printf("   %d \t  %s  %s\n", lex->nline , lex_type_string(lex->lex_type, TRUE) , lex->value);
    }
}
