#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <strings.h>

#include <global.h>
#include <notify.h>
#include <lex.h>
#include <queue.h>


QUEUE add_to_queue(QUEUE Q, char* token, int type, int numline)
{
    LEXEM lex = calloc (1, sizeof(*lex));
    lex->nline = numline;
    lex->lex_type = type;
    lex->value = strdup(token);
    Q = ajouter_fin(Q, lex);

    return Q;
}


QUEUE new_queue()
{
  return NULL;
}

QUEUE ajouter_fin(QUEUE Q, LEXEM lex) 
{
  if (Q == NULL)
    {
      Q = calloc (1, sizeof(*Q));
      Q->element = lex;
      Q->next = Q;
      return Q;
    }
  QUEUE adresse_retour = Q->next;
  Q->next = calloc (1, sizeof(*Q));
  Q = Q->next;
  Q->element = lex;
  Q->next = adresse_retour;
  return Q;
}
