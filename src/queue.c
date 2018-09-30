#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <strings.h>

#include <global.h>
#include <notify.h>
#include <lex.h>
#include <queue.h>


QUEUE add_to_queue(char* token, int type, int numline, QUEUE Q)
{
    LEXEM lex = calloc (1, sizeof(struct lexeme));
    lex->nline = numline;
    lex->lex_type = type;
    lex->value = token;
    Q = ajouter_fin(Q, lex);
    return Q;
}


QUEUE new_queue()
{
  return NULL;
}

QUEUE ajouter_fin(QUEUE l, LEXEM lex) //à vérfier
{
  if (l == NULL)
    {
      l = calloc (1, sizeof(struct queue));
      l->element = lex;
      l->next = l;
    }
  QUEUE adresse_retour = l->next;
  l->next = calloc (1, sizeof(struct queue));
  l = l->next;
  l->element = lex;
  l->next = adresse_retour;
  return l;
}
