#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>

#include <global.h>
#include <notify.h>
#include <lex.h>
#include <queue_list.h>
#include <test.h>
#include <dictionnaire.h>
#include <etiq.h>
#include <fct_analyse_1.h>
#include <analyse_synth.h>

#include <error.h>
#include <assert.h>




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


void print_list_instr( LIST l ) {
    INSTR I;
    printf("\n====== Liste des instructions ======\n");
    printf("\n          |          |          |\n");
    printf(" valeur   |  arg1    |  arg2    |  arg 3    |  etiq_def\n");
    printf("          |          |          |           |\n");

    while (l!= NULL)
    {
        I = l->element;
        char* a1;
        char* a2;
        char* a3;
        char* val = ((LEXEM)(I->lex))->value ;
        if ((I->arg1) == NULL) a1 = strdup("NONE");
        else a1 = strdup(((char*)((LEXEM)(I->arg1))->value)) ;

        if ((I->arg2)== NULL) a2 = strdup("NONE");
        else a2 = strdup(((char*)((LEXEM)(I->arg2))->value)) ;


        if ((I->arg3) == NULL)   a3 = strdup("NONE");
        else a3 = strdup(((char*)((LEXEM)(I->arg3))->value)) ;


        printf(" %s\t  %s\t\t  %s\t   %s\t  %d\n", val, a1, a2, a3, I->etiq_def);
        l = l->next;
    }
    return;
}

void print_symb_table( LIST l ) {
    printf("\n====== Table des symboles ======\n");
    printf("\n          |             |           \n");
    printf(" name     |  section    |   line   \n");
    printf("          |             |          \n");

    while (l!= NULL)
    {
        printf(" %s\t\t %d\t   %d\n", ((ETIQ)(l->element))-> name, ((ETIQ)(l->element))-> section,  ((ETIQ)(l->element))-> nline );
        l = l->next;
    }
    return;
}


void print_list_data( LIST l ) {
    printf("\n====== Liste des .data ======\n");

    while (l!= NULL)
    {
        printf(" decalage : %d  etiq_def: %d  op_type: %d\n", ((DATA)(l->element))-> decalage, ((DATA)(l->element))-> etiq_def,  (((data_op)(((DATA)(l->element))-> D))->type ));
        l = l->next;
    }
    return;
}
