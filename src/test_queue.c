#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <strings.h>

#include <global.h>
#include <notify.h>
#include <lex.h>
#include <test.h>
#include <queue_list.h>
#include <dictionnaire.h>


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


void read_queue_word(QUEUE Q)
{
    if (Q == NULL) printf("VIDE! \n");
    Q = Q->next;
    QUEUE first_memory = Q;
    WORD mot = Q->element;
    printf("lecture de la liste Queue : \n");
    printf("instruction : %s arguments: %d  \n", mot->instruction, mot->arg);
    Q = Q->next;
    while (Q != first_memory)
    {
        WORD mot = Q->element;
        printf("instruction : %s arguments: %d  \n", mot->instruction, mot->arg);
        Q = Q->next;

    }
    return;
}
