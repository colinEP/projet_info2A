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
#include <relocation.h>

#include <error.h>
#include <assert.h>


// LIST reloc_text(LIST list_instr, LIST symb_table){ // aurait-pu être jointe avec look_for_undefined_etiq_in_instr ?!
//     LIST reloc_table_text = calloc (1, sizeof(*reloc_table_text));
//     INSTR I;
//     RELOC R;
//
//
//     // CAS SPECIAL POUR LW ET SW ?!?! WARNING WARNING
//
//
//     while (list_instr!= NULL){
//         I = l->element;
//         char* instruction;
//         instruction = (I->lex)->value;
//         instruction = put_in_uppercase (instruction);
//
//         if ( (I->arg1)->type == Label ) {
//             R = look_for_etiq_and_return_reloc(symb_table, (I->arg1)->val.char_chain);
//
//
//             if ( (strcmp(instruction, "J")==0) || (strcmp(instruction, "JAL")==0) || (strcmp(instruction, "JR")==0) ) {
//                 R-> type = R_MIPS_26;
//             }
//              // HOHO ! Y a t il d'autres cas possibles ? WARNING
//             if ( strcmp(instruction, "LUI") == 0)  {
//                 R-> type = R_MIPS_HI16;
//             }
//             if ( (strcmp(instruction, "LW") == 0)|| (strcmp(instruction, "SW")==0))  {
//                 R-> type = R_MIPS_LO16;
//             }
//             R -> addend = I->decalage; // VRAI OU PAS ? WARNING
//             reloc_table_text = add_to_list(reloc_table_text, R, (I->arg1)->etiq_def));
//         }
//     }
//
//     return reloc_table_text;
//
// }
//
// LIST reloc_data(LIST list_data){
//     LIST reloc_table_data = calloc (1, sizeof(*reloc_table_data));
//
//     while (list_data!= NULL){
//
//
//     }
//     return reloc_table_data;
// }
//
//
//
// RELOC look_for_etiq_and_return_reloc( LIST symb_table, char* lexem, int is_def)
//  {
//      RELOC R = calloc(1, sizeof(*R));
//      int a;
//      while (symb_table != NULL)
//      {
//          a = strcmp(((ETIQ) symb_table->element)->name, lexem); // conversion en ETIQ de (symb_table->element)
//          if (a == 0) {
//              if (is_def){
//                 R->offset = ((ETIQ) symb_table->element)->decalage ;
//                 R->value = ((ETIQ) symb_table->element)-> section ;
//             }
//             else {
//                 R->offset = ;
//                 R->value = ;
//             }
//              return R;
//          }
//          symb_table = symb_table->next;
//      }
//      ERROR_MSG("Probleme d'etiquette non trouvee dans table des symboles !\n");
//  }
