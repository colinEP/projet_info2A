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
#include <print_functions.h>

#include <error.h>
#include <assert.h>


LIST change_pseudo_instr(LIST list_instr, int* pdecalage)
// normalement cette fonction est appelée alors que les arguments des pseudo_instruction ont déjà été vérifiés donc c'est bon !

 {
     INSTR I = list_instr->element;
     char* instruction =  strdup( ((LEXEM)(I->lex))->value );
     instruction = put_in_uppercase (instruction);
     char* char1;
     int int1;
     int is_label = 0;
     int is_def = 0;

    if (strcmp(instruction,"NOP")==0){ // OK

         free(((LEXEM)(I->lex))->value);
         ((LEXEM)(I->lex))->value = strdup("SLL");
         I-> nb_arg = 3;
         I->Exp_Type_1 = Reg;
         I->Exp_Type_2 = Reg;
         I->Exp_Type_3 = Sa;
         (I-> arg1)->type = Reg;
         (I-> arg1)->val.entier = 0;
         (I-> arg2)->type = Reg;
         (I-> arg2)->val.entier = 0;
         (I-> arg3)->type = Sa;
         (I-> arg3)->val.entier = 0;
         (I-> arg1)->etiq_def = -1;
         (I-> arg2)->etiq_def = -1;
         (I-> arg3)->etiq_def = -1;
     }

    else if (strcmp(instruction,"MOVE")==0){ //OK
         //  ici peu de cas car pour arg1 et arg2, ils restent les mêmes, qu'ils soient Reg ou Label
         free(((LEXEM)(I->lex))->value);
         (I->lex)->value = strdup("ADD");
         I-> nb_arg = 3;
         I->Exp_Type_3 = Reg;
         (I-> arg3)->type = Reg;
         (I-> arg3)->val.entier = 0;
         (I-> arg3)->etiq_def = -1;
     }

    else if (strcmp(instruction,"NEG")==0){ //OK
         // cas arg 1 :  il reste le même, qu'il soit Label ou Reg
         // idem pour Exp_Type_2

         free(((LEXEM)(I->lex))->value);
         ((LEXEM)(I->lex))->value = strdup("SUB");
          I-> nb_arg = 3;
          I->Exp_Type_3 = Reg;

         if ((I-> arg2)->type == Label) {    ERROR_MSG(" AVOIR PLUS EN DETAIL !! C'est impossible d'avoir une étiquette pour un registre !");
             (I-> arg3)->type = Label;
             (I-> arg3)->etiq_def = (I-> arg2)->etiq_def;
             (I-> arg3)->val.char_chain = strdup((I-> arg2)->val.char_chain);
         }
         else {
             (I-> arg3)->type = Reg;
             (I-> arg3)->val.entier = (I-> arg2)->val.entier;
         }

         (I-> arg2)->type = Reg;
         (I-> arg2)->val.entier = 0;
       }

    else if (strcmp(instruction,"LI")==0){ //OK
         // cas arg 1 :  il reste le même, qu'il soit Label ou Reg

         free(((LEXEM)(I->lex))->value);

         ((LEXEM)(I->lex))->value = strdup("ADDI");
          I-> nb_arg = 3;
          I->Exp_Type_2 = Reg;
          I->Exp_Type_3 = Imm;

         if      ((I-> arg2)->type == Label) {
             (I-> arg3)->type = Label;
             (I-> arg3)->etiq_def = (I-> arg2)->etiq_def;
             (I-> arg3)->val.char_chain = strdup((I-> arg2)->val.char_chain);
         }
         else if ((I-> arg2)->type == Imm){
             (I-> arg3)->type = Imm;
             (I-> arg3)->val.entier = (I-> arg2)->val.entier;
         }
         else ERROR_MSG("Error de type dans la pseudo instr LI, normalement impossible :)");

         if ((I-> arg2)->type == Label) free((I-> arg2)->val.char_chain);

         (I-> arg2)->type = Reg;
         (I-> arg2)->val.entier = 0;
         (I-> arg2)->etiq_def = -1;

     }

    else if (strcmp(instruction,"BLT")==0){
         // sauvegarde de la valeur de target
         if ((I->arg3)->type == Label){
             is_label = 1;
             is_def = (I-> arg3)-> etiq_def;
             char1 = strdup ((I-> arg3)->val.char_chain);
         }
         else {
             is_def = -1;
             int1 = (I-> arg3)->val.entier;
         }
         free(((LEXEM)(I->lex))->value);
         ((LEXEM)(I->lex))->value = strdup("SLT");
         I-> nb_arg = 3;
         I->Exp_Type_1 = Reg;
         I->Exp_Type_2 = Reg;
         I->Exp_Type_3 = Reg;

        if ((I-> arg3)->type == Label) free((I-> arg3)->val.char_chain);
        (I-> arg3)->type = Reg;
        (I-> arg3)->val.entier = (I-> arg2)->val.entier;  //copie de $rs
        (I-> arg3)->etiq_def = -1;

        (I-> arg2)->type = Reg;
        (I-> arg2)->val.entier = (I-> arg1)->val.entier;  //copie de $rt
        (I-> arg2)->etiq_def = -1; //logiquement c'est inutile, tout comme la redefinition du type Reg

         // --- modification de arg1 ----
        (I-> arg1)->type = Reg;
        (I-> arg1)->val.entier = 1;
        (I-> arg1)->etiq_def = -1; //logiquement c'est inutile, tout comme la redefinition du type Reg

         LEXEM second_instr = calloc(1, sizeof(*second_instr));
         second_instr->nline = I->lex->nline;
         second_instr->lex_type = SYMBOLE;
         second_instr->value = strdup("BNE");

         list_instr = add_to_list_instr(second_instr, *pdecalage, 3, list_instr, Reg, Reg, Rel); //etiq def est à -1 par defaut
         free_lex(second_instr);  // add_to_list_instr a fait une copie

         I = list_instr->element;
         (I-> arg1)->type = Reg;
         (I-> arg1)->val.entier = 1;
         (I-> arg2)->type = Reg;
         (I-> arg2)->val.entier = 0;
         if (is_label){
             (I-> arg3)->type = Label;
             (I-> arg3)->etiq_def = is_def;
             (I-> arg3)->val.char_chain = char1;
         }
         else {
             (I-> arg3)->type = Rel;
             (I-> arg3)->val.entier = int1;
         }

         // maj du decalage car instruction insérée
         *pdecalage += 4;

     }
     free(instruction);
     return list_instr;
}

LIST change_pseudo_SW_LW(LIST list_instr, int* pdecalage) {
    INSTR I = list_instr->element;
    char* instruction =  strdup( ((LEXEM)(I->lex))->value );
    instruction = put_in_uppercase (instruction);
    int reg;
    int is_def;
    char* name_etiq;

    // ------- cas LW ---------
    if (strcmp(instruction,"LW")==0){
        if ((I->arg2)->type == Label){ // cas où arg2 est une target
            if ((I->arg1)->type =! Reg){ // bizarre de mettre cette erreur ici, certes.... WARNING WARNING
                ERROR_MSG("Arg1 de LW doit etre un registre !\n");
            }
            reg = (int)((I->arg1)->val.entier);
            free(((LEXEM)(I->lex))->value);
            (I->lex)->value = strdup("LUI");
            I-> nb_arg = 2;
            I->Exp_Type_1 = Reg;
            I->Exp_Type_2 = Imm;
            I->Exp_Type_3 = None;

            // arg1 reste inchangé
            (I-> arg2)->type = Target; //type Target permettra de remplacer par sa valeur après relocation
            //etiq_def2 reste le même
            is_def = (I->arg2)->etiq_def;
            name_etiq = strdup((I->arg2)->val.char_chain);
            // PB:  pour avoir l'adresse du poiteur, il faut qu'il y ait eu relocation !

            (I-> arg1)->type = Reg; // UTILE
            (I-> arg1)->val.entier = 1; // = $at
            (I-> arg3)->type = None;

            LEXEM second_instr = calloc(1, sizeof(*second_instr));
            second_instr->nline = I->lex->nline;
            second_instr->lex_type = SYMBOLE;
            second_instr->value = strdup("LW") ;

            list_instr = add_to_list_instr(second_instr, *pdecalage, 2, list_instr, Reg, Bas, None); //etiq def est à -1 par defaut
            free_lex(second_instr);  // add_to_list_instr a fait une copie

            I = list_instr->element;
            (I-> arg1)->type = Reg;
            (I-> arg1)->val.entier = reg;

            (I-> arg2)->type = Bas_Target;
            (I-> arg2)->etiq_def = is_def;
            (I-> arg2)->val.char_chain = name_etiq;   // strdup déjà fait

            (I-> arg3)->type = None;

            // maj du decalage car instruction insérée
            *pdecalage += 4;
        }
    }
    // ------- cas SW ---------
    else if (strcmp(instruction,"SW")==0){
        if (((I->arg2)->type) == Label){ // cas où arg2 est une target
            if (((I->arg1)->type) =! Reg){ // bizarre de mettre cette erreur ici, certes.... WARNING WARNING
                ERROR_MSG("Arg1 de SW doit etre un registre !\n");
            }
            reg = (int)((I->arg1)->val.entier);
            free(((LEXEM)(I->lex))->value);
            (I->lex)->value = strdup("LUI");
            I-> nb_arg = 2;
            I->Exp_Type_1 = Reg;
            I->Exp_Type_2 = Imm;
            I->Exp_Type_3 = None;

            // arg1 reste inchangé
            (I-> arg2)->type = Target; //type Target permettra de remplacer par sa valeur après relocation
            //etiq_def2 reste le même
            is_def = (I->arg2)->etiq_def;
            name_etiq = strdup((I->arg2)->val.char_chain);
            // PB:  pour avoir l'adresse du poiteur, il faut qu'il y ait eu relocation !

            (I-> arg1)->type = Reg; // UTILE
            (I-> arg1)->val.entier = 1; // = $at
            (I-> arg3)->type = None;

            LEXEM second_instr = calloc(1, sizeof(*second_instr));
            second_instr->nline = I->lex->nline;
            second_instr->lex_type = SYMBOLE;
            second_instr->value = strdup("SW") ;

            list_instr = add_to_list_instr(second_instr, *pdecalage, 2, list_instr, Reg, Bas, None); //etiq def est à -1 par defaut
            free_lex(second_instr);  // add_to_list_instr a fait une copie

            I = list_instr->element;
            (I-> arg1)->type = Reg;
            (I-> arg1)->val.entier = reg;

            (I-> arg2)->type = Bas_Target;
            (I-> arg2)->etiq_def = is_def;
            (I-> arg2)->val.char_chain = name_etiq;   // strdup déjà fait

            (I-> arg3)->type = None;

            // maj du decalage car instruction insérée
            *pdecalage += 4;
        }
    }
    free(instruction);
    return list_instr;
}

int upper_16(int val_32b){
     // il suffit de faire un décalage !
     int val_16b;
     val_16b = val_32b >>16;
     return val_16b;
 }

int lower_16(int val_32b){
     // il suffit de mettre un masque !
     int val_16b;
     val_16b = val_32b & 0xFFFF; // car 0xFFFF correspond en binaire à (0) 1111 1111 1111 1111
     return val_16b;
 }
