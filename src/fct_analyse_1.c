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


LIST add_to_current_list(operand_type type_op, void value, int dec, int line, LIST current_list){ //arg quel type pour value ??
    DATA data = calloc(1, sizeof(*data));
    data-> decalage = dec;
    data-> line = line;
    data -> D -> type = type_op;
    // comment stocker value ???? -> le convertir ici ou le prendre comme un void*
    current_list = add_to_list(current_list, data);
    return current_list;

}





LIST fill_arguments(LEXEM lexem, LIST list_instr, int previous_type_lexem)
{
    // considérer le cas où prévious_type_lexeme est un MOINS !!!
     if ((INSTR)(list_instr->element)->arg1 == NULL){  // à mettre sous forme d'une fonction
         (INSTR)(list_instr->element)->arg1 = lexem;
     }
     else {
         if ((INSTR)(list_instr->element)->arg2 == NULL){
             (INSTR)(list_instr->element)->arg2 = lexem;
         }
         else {
             if ((INSTR)(list_instr->element)->arg3 == NULL){
                 (INSTR)(list_instr->element)->arg3 = lexem;
             }
             else ERROR_MSG("Trop d'arguments !\n");
         }
     }
     return list_instr;
 }
