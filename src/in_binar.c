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
#include <dictionnaire.h>
#include <in_binar.h>

#include <error.h>



void instr_in_binar(LIST list_instr, int size_list, QUEUE dictionnaire)
{
    int tab_instr_bin[size_list];
    int i=0;
    unsigned int binar_value;

    // parcourir la liste
    while (list_instr != NULL){
        int binar_value = 0;
        INSTR I = list_instr->element;
        char* instruction =  strdup( ((LEXEM)(I->lex))->value );
        instruction = strdup(put_in_uppercase (instruction));
        int opc;
        char * from_25_21;
        char * from_20_16;
        char * from_15_11;
        char * from_10_6;
        char * from_5_0;
        int val_1,val_2,val_3,val_4,val_5;

        opc = look_for_instr_and_return_binar_info(dictionnaire, "BGTZ", &from_25_21, &from_20_16, &from_15_11, &from_10_6, &from_5_0); // fonctionne !
        // ajouter chaque instruction dans le tableau : tab_instr_bin[i]
        // méthode ajout :
            // opcode : easy, le laisser tel quel
            // if un entier laisser tel quel
            // if _ ne rien faire
            // if a1, a2 ou a3 ajouter la valeur stockée dans la liste:
            // l'ajout se fera sur une taille qui dépendra du type de l'argument (5 bits si Reg, 16 bits si c'est un Rel ou 26 si c'est un Abs, etc. )


        // opcode:
        binar_value = binar_value | ( (opc << 26) & FC000000 );

        // from_25_21
        if (strcmp(from_25_21, "_")==0){
            // ne rien faire
        }
        if (from_25_21[0] == 'a'){
            // aller chercher l'argument correspondant
            if (from_25_21[1] == '1'){
            }
            if (from_25_21[1] == '2'){
            }
            if (from_25_21[1] == '3'){
            }
            // check le TYPE : si != Reg -> error car seuls 5 bits sont dispos --> logiquement
            // ajouter la valeur
        }
        if (isdigit(from_25_21)){
            val_1 = strtol(from_25_21, NULL, 0);
            if (){
                //error si > 5bits !
            }
            binar_value = binar_value | ( (opc << 21) & 3E00000 );
        }






        i = i+1;
        list_instr = list_instr->next;

    }



    return ;

}


int look_for_instr_and_return_binar_info( LIST dictionnaire, char** instruction, char ** from_25_21, char ** from_20_16, char ** from_15_11, char ** from_10_6, char ** from_5_0)
// renvoit l'opcode et les autres en arg
 {
     int a;
     DICO_LINE D;
     while (dictionnaire != NULL)
     {
         D = dictionnaire->element;
         a = strcmp(D->instruction, instruction);

         if (a == 0) {
             *from_25_21 = strdup(D->code_bin_1);
             *from_20_16 = strdup(D->code_bin_2);
             *from_15_11 = strdup(D->code_bin_3);
             *from_10_6 = strdup(D->code_bin_4);
             *from_5_0 = strdup(D->code_bin_5);
             return D->opcode;
         }
         dictionnaire = dictionnaire->next;
     }
     ERROR_MSG("Probleme d'instruction non trouvee dans dictionnaire !\n");
 }
