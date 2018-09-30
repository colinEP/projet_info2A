#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <strings.h>

#include <global.h>
#include <notify.h>
#include <lex.h>
#include <queue.h>
#include <test_queue.h>

void read_queue_lex(QUEUE Q)
{
    if (Q == NULL) printf("VIDE! \n");
    Q = Q->next;
    QUEUE first_memory = Q;
    LEXEM lex = Q->element;
    printf("lecture de la liste Queue : \n");
    printf("valeur : %s type: %d numero de ligne : %d \n", lex->value, lex->lex_type, lex->nline);
    Q = Q->next;
    while (Q != first_memory)
    {
        LEXEM lex = Q->element;
        printf("valeur : %s \ntype: %d \nnumero de ligne : %d \n", lex->value, lex->lex_type, lex->nline);
        Q = Q->next;

    }
    return;
}
