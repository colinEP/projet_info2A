#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <strings.h>

#include <global.h>
#include <notify.h>
#include <lex.h>
#include <queue_list.h>
#include <test.h>
#include <fct_analyse_1.h>
#include <analyse_synth.h>

#include <etiq.h>

#include <error.h>



INSTR new_instr(){ //initialisation de la cellule instruction
    INSTR I = calloc(1, sizeof(*I));
    I-> arg1 = NULL;
    I-> arg2 = NULL;
    I-> arg3 = NULL;
    return I;
}

LIST add_to_list_instr(LEXEM l, int dec, int nbarg, LIST list_instr)
 {
     INSTR I = new_instr();
     I-> decalage = dec;
     I-> nb_arg = nbarg;
     I-> lex = l;
     list_instr = add_to_list(list_instr, I);
     return list_instr;
 }


 DATA new_data(){ //initialisation de la cellule DATA
     DATA data = calloc(1, sizeof(*data));
     data->etiq_def = 0; // initialisation de la variable qui indique si etiquette definie dans symb_tab
     return data;
 }

data_op fill_val_op(void* pvalue, operand_type type_op)
{
    data_op D = calloc (1, sizeof(*D));
    D->type = type_op;
    switch(D->type)
       {
           case PWORD:
               D->val.PWORD = (*(int*)pvalue);
               break;
           case PBYTE:
               D->val.PBYTE = *(char*)pvalue;
               break;
           case PASCIIZ:
               D->val.PASCIIZ = (char*)pvalue;
               break;
           case PSPACE:
               D->val.PSPACE = (*(unsigned int*)pvalue);
               break;
           case LABEL:
               D->val.LABEL = (char*)pvalue;
               break;
       }
    return D;
}


LIST add_to_current_list(operand_type type_op, void* pvalue, int dec, int line, LIST current_list){ // FONCTIONNE
    DATA data = new_data();
    data-> decalage = dec;
    data-> line = line;
    data -> D = fill_val_op(pvalue, type_op);
    current_list = add_to_list(current_list, data);
    return current_list;
}





LIST fill_arguments(LEXEM lexem, LIST list_instr, int previous_type_lexem)
{
    // considérer le cas où prévious_type_lexeme est un MOINS !!!
     if (((INSTR)(list_instr->element))->arg1 == NULL){  // à mettre sous forme d'une fonction
         ((INSTR)(list_instr->element))->arg1 = lexem;
     }
     else {
         if (((INSTR)(list_instr->element))->arg2 == NULL){
             ((INSTR)(list_instr->element))->arg2 = lexem;
         }
         else {
             if (((INSTR)(list_instr->element))->arg3 == NULL){
                 ((INSTR)(list_instr->element))->arg3 = lexem;
             }
             else ERROR_MSG("Trop d'arguments !\n");
         }
     }
     return list_instr;
 }
