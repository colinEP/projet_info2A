#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <strings.h>

#include <fct_analyse_1.h>
#include <global.h>
#include <notify.h>
#include <lex.h>
#include <queue_list.h>
#include <test.h>
#include <etiq.h>

#include <error.h>



int* instr_in_binar(LIST list_instr, int size_list, QUEUE dictionnaire)
{
    int tab_instr_bin[size_list]; // faut-il une taille plus grande que le nombre d'instructions ?
    int i=0;
    unsigned int binar_value;

    // parcourir la liste
    while (list_instr != NULL){
        binar_value = 0;
        INSTR I = list_instr->element;
        char* instruction =  strdup( ((LEXEM)(I->lex))->value );
        instruction = put_in_uppercase (instruction);
        // ajouter chaque instruction dans le tableau : tab_instr_bin[i]
        // méthode ajout :
            // opcode : easy, le laisser tel quel
            // if un entier laisser tel quel
            // if _ ne rien faire
            // if a1, a2 ou a3 ajouter la valeur stockée dans la liste:
            // l'ajout se fera sur une taille qui dépendra du type de l'argument (5 bits si Reg, 16 bits si c'est un Rel ou 26 si c'est un Abs, etc. )







        list_instr = list_instr->next;




    }



    return tab_instr_bin ;

}
