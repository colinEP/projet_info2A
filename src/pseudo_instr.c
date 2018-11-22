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




// TODO TODO TODO
// mettre à jour le décalage suite à ces pseudo_instruction !!
// + le cas LW / SW 

LIST change_pseudo_instr(LIST list_instr)
// normalement cette fonction est appelée alors que les arguments des pseudo_instruction ont déjà été vérifiés donc c'est bon !

 {
     INSTR I = list_instr->element;
     printf("Instruction actuelle : %s\n", ((LEXEM)(I->lex))->value);
     char* instruction =  strdup( ((LEXEM)(I->lex))->value );
     char* char1;
     int int1;
     int is_label = 0;
     int is_def = 0;

     if (strcmp(instruction,"NOP")==0){ // OK
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
         return list_instr;
     }

     if (strcmp(instruction,"MOVE")==0){ //OK
         //  ici peu de cas car pour arg1 et arg2, ils restent les mêmes, qu'ils soient Reg ou Label
         (I->lex)->value = strdup("ADD");
         I-> nb_arg = 3;
         I->Exp_Type_3 = Reg;
         (I-> arg3)->type = Reg;
         (I-> arg3)->val.entier = 0;
         (I-> arg3)->etiq_def = -1;
         return list_instr;
     }

     if (strcmp(instruction,"NEG")==0){ //OK
         // cas arg 1 :  il reste le même, qu'il soit Label ou Reg
         // idem pour Exp_Type_2

         ((LEXEM)(I->lex))->value = strdup("SUB");
          I-> nb_arg = 3;
          I->Exp_Type_3 = Reg;

         if ((I-> arg2)->type == Label) {
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
         return list_instr;
       }


     if (strcmp(instruction,"LI")==0){ //OK
         // cas arg 1 :  il reste le même, qu'il soit Label ou Reg

         ((LEXEM)(I->lex))->value = strdup("ADDI");
          I-> nb_arg = 3;
          I->Exp_Type_2 = Reg;
          I->Exp_Type_3 = Imm;

         if ((I-> arg2)->type == Label) {
             (I-> arg3)->type = Label;
             (I-> arg3)->etiq_def = (I-> arg2)->etiq_def;
             (I-> arg3)->val.char_chain = strdup((I-> arg2)->val.char_chain);
         }
         else {
             (I-> arg3)->type = Imm;
             (I-> arg3)->val.entier = (I-> arg2)->val.entier;
         }

         (I-> arg2)->type = Reg;
         (I-> arg2)->val.entier = 0;
         return list_instr;

     }

     if (strcmp(instruction,"BLT")==0){
         // ici c'est arg 2 qui ne change pas
         // arg 1  n'est pas à enregistrer car il reste dans la premiere cellule instr =! target

         if ((I->arg3)->type == Label){
             is_label = 1;
             is_def = (I-> arg3)-> etiq_def;
             char1 = strdup ((I-> arg3)->val.char_chain);
         }
         else {
             is_def = -1;
             int1 = (I-> arg3)->val.entier;
         }

         ((LEXEM)(I->lex))->value = strdup("SLT");
         I-> nb_arg = 3;
         I->Exp_Type_1 = Reg; // normalement inutile
         I->Exp_Type_2 = Reg; // normalement inutile
         I->Exp_Type_3 = Reg;

         // --- copie de arg1 en arg3 ----
         if ((I->arg1)->type == Label){
             (I-> arg3)->type = Label;
             (I-> arg3)->etiq_def =  (I-> arg1)->etiq_def;
             (I-> arg3)->val.char_chain = strdup ((I-> arg1)->val.char_chain);
        }
        else {
            (I-> arg3)->type = Reg; //etiq def reste idem
            (I-> arg3)->val.entier = (I-> arg1)->val.entier;

        }
         // --- modification de arg1 ----
        (I-> arg1)->type = Reg;  //utile au cas où c'était jadis une étiquette
        (I-> arg1)->val.entier = 1;
        (I-> arg1)->etiq_def = -1;

         LEXEM second_instr = calloc(1, sizeof(*second_instr));
         second_instr->nline = I->lex->nline;
         second_instr->lex_type = SYMBOLE;
         second_instr->value = strdup("BNE") ;

         list_instr = add_to_list_instr(second_instr, I-> decalage, 3, list_instr, Reg, Reg, Rel); //etiq def est à -1 par defaut
         I = list_instr->element;
         (I-> arg1)->type = Reg;
         (I-> arg1)->val.entier = 1;
         (I-> arg2)->type = Reg;
         (I-> arg2)->val.entier = 0;
         if (is_label){
             (I-> arg3)->type = Label;
             (I-> arg3)->etiq_def = is_def;
             (I-> arg3)->val.char_chain = strdup(char1);
         }
         else {
             (I-> arg3)->type = Rel;
             (I-> arg3)->val.entier = int1;
         }
         return list_instr;
     }

     return list_instr;
 }
