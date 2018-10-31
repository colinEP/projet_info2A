#ifndef _ANALYSE_SYNTH_H_
#define _ANALYSE_SYNTH_H_



void analyse_synth(LIST list_instr, LIST list_data, LIST list_bss, LIST symb_table, LIST list_lex );

int check_instruction(char* lex, LIST dictionnaire);


#endif
