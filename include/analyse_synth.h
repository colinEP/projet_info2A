#ifndef _ANALYSE_SYNTH_H_
#define _ANALYSE_SYNTH_H_



void analyse_synth(LIST list_instr, LIST list_data, LIST list_bss, LIST symb_table, LIST list_lex, LIST dictionnaire);
int check_and_convert_register(char* reg);
int check_type_arg_inst(int type_lexem, char* val_lexem, int type_arg_expected);


#endif
