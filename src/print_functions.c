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
    if (Q == NULL) printf("VIDE! \n");    // TODO ajouter return ??
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


void read_queue_dico(QUEUE Q)   // la version list existe !
{
    if (Q == NULL) printf("VIDE! \n");    // TODO ajouter return ??
    Q = Q->next;
    QUEUE first_memory = Q;
    DICO_LINE mot = Q->element;
    printf("lecture de la liste Queue : \n");
    printf("Instruction : %s nb arguments: %d  type arg1 : %s  type arg2 : %s  type arg3: %s \n", mot->instruction, mot->arg, mot->arg_type_1, mot->arg_type_2, mot->arg_type_3);
    Q = Q->next;
    while (Q != first_memory)
    {
        DICO_LINE mot = Q->element;
        printf("Instruction : %s nb arguments: %d  type arg1 : %s  type arg2 : %s  type arg3: %s \n", mot->instruction, mot->arg, mot->arg_type_1, mot->arg_type_2, mot->arg_type_3);
        Q = Q->next;

    }
    return;
}

void print_list_dico( LIST l ) {
    LIST p;
    DICO_LINE dico;
    printf("\n====== Dico d'instruction ======\n");
    printf("Instruct | nb_arg | type_arg1 | type_arg2 | type_arg3\n");
    //printf("         |        |           |           |\n");
    for ( p=l ; p!=NULL ; p=p->next ) {
        dico = p->element;
        printf("  %s\t      %d\t       %s\t   %s\t      %s\t\n", dico->instruction, dico->arg, dico->arg_type_1, dico->arg_type_2, dico->arg_type_3);
    }
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
    printf("\n====== Liste des instructions ====== \n \n");
    while (l!= NULL)
    {
        I = l->element;
        char* a1;
        char* a2;
        char* a3;
        ARG_INST A1 = I->arg1;
        ARG_INST A2 = I->arg2;
        ARG_INST A3 = I->arg3;
        printf(" Instruction : %s \n", ((LEXEM)(I->lex))->value);
        if (( A1->type) == None) {
            a1 = strdup("NONE");
            printf(" Arg1 : %s \n", a1);
        }
        else {
            if ( ((A1->type) == Bas_Target)||((A1->type) == Target)){
                a1 = strdup(((char*)(A1->val.char_chain) )) ;
                printf(" Arg1 : %s \n", a1);
            }
            else printf(" Arg1 : %ld \n", ((long int)(A1->val.entier)));
        }

        if ((A2->type)== None){
             a2 = strdup("NONE");
             printf(" Arg1 : %s \n", a2);
         }
        else {
            if ( ((A2->type) == Bas_Target)||((A2->type) == Target)){
                a2 = strdup(((char*)(A2->val.char_chain) )) ;
                printf(" Arg2 : %s \n", a2);
            }
            else printf(" Arg2 : %ld \n", ((long int)(A2->val.entier)));
        }

        if ((A3->type) == None) {
            a3 = strdup("NONE");
            printf(" Arg3 : %s \n", a3);
        }
        else {
            if ( ((A3->type) == Bas_Target)||((A3->type) == Target)){
                a3 = strdup(((char*)(A3->val.char_chain))) ;
                printf(" Arg3 : %s \n", a3);
            }
            else printf(" Arg3 : %ld \n", ((long int)(A3->val.entier)));
        }
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
