//#define _POSIX_C_SOURCE 200112L
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
    LEXEM lex = calloc (1, sizeof(LEXEM));
    lex->nline = numline;
    lex->lex_type = type;
    lex->value = token;
    ajouter_fin(Q, lex);
}


QUEUE new_queue()
{
  return NULL;
}

QUEUE ajouter_fin(QUEUE l, LEXEM lex) //à vérfier
{
  if (l == NULL)
    {
      l = calloc (1, sizeof(QUEUE));
      l->element = lex;
      l->next = l;
    }
  QUEUE adresse_retour = l->next;
  l->next = calloc (1, sizeof(QUEUE));
  l = l->next;
  l->element = lex;
  l->next = adresse_retour;
  return l;
}
