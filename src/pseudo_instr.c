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



LIST change_pseudo_instr(LIST list_instr)
// normalement cette fonction est appelée alors que les arguments des pseudo_instruction ont déjà été vérifiés donc c'est bon !
 {
     printf("COUCOU ! \n");
     INSTR I = list_instr->element;
     printf("Instruction actuelle : %s\n", ((LEXEM)(I->lex))->value);
     char* instruction =  strdup( ((LEXEM)(I->lex))->value );
     //char* char1;
     //char* char2;
     int int1;
     int int2;

     if (strcmp(instruction,"NOP")==0){
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
         return list_instr;
     }

     if (strcmp(instruction,"MOVE")==0){
         int1 = (I-> arg1)->val.entier;
         int2 = (I-> arg2)->val.entier;
         ((LEXEM)(I->lex))->value = strdup("ADD");
         I-> nb_arg = 3;
         I->Exp_Type_1 = Reg;
         I->Exp_Type_2 = Reg;
         I->Exp_Type_3 = Reg;
         (I-> arg1)->type = Reg;
         (I-> arg1)->val.entier = int1;
         (I-> arg2)->type = Reg;
         (I-> arg2)->val.entier = int2;
         (I-> arg3)->type = Reg;
         (I-> arg3)->val.entier = 0;
         return list_instr;
     }

     if (strcmp(instruction,"NEG")==0){
         int1 = (I-> arg1)->val.entier;
         int2 = (I-> arg2)->val.entier;
         ((LEXEM)(I->lex))->value = strdup("SUB");
         I-> nb_arg = 3;
         I->Exp_Type_1 = Reg;
         I->Exp_Type_2 = Reg;
         I->Exp_Type_3 = Reg;
         (I-> arg1)->type = Reg;
         (I-> arg1)->val.entier = int1;
         (I-> arg2)->type = Reg;
         (I-> arg2)->val.entier = 0;
         (I-> arg3)->type = Reg;
         (I-> arg3)->val.entier = int2;
         return list_instr;

     }

     if (strcmp(instruction,"LI")==0){
         int1 = (I-> arg1)->val.entier;
         int2 = (I-> arg2)->val.entier;
         ((LEXEM)(I->lex))->value = strdup("ADDI");
         I-> nb_arg = 3;
         I->Exp_Type_1 = Reg;
         I->Exp_Type_2 = Reg;
         I->Exp_Type_3 = Imm;
         (I-> arg1)->type = Reg;
         (I-> arg1)->val.entier = int1;
         (I-> arg2)->type = Reg;
         (I-> arg2)->val.entier = 0;
         (I-> arg3)->type = Imm;
         (I-> arg3)->val.entier = int2;
         return list_instr;

     }

     if (strcmp(instruction,"BLT")==0){
         // WARNING WARNING ici arg = target

         return list_instr;
     }

     return list_instr;

 }
