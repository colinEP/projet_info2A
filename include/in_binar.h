#ifndef _IN_BINAR_H_
#define _IN_BINAR_H_


int look_for_instr_and_return_binar_info( LIST dictionnaire, char* instruction, char ** from_25_21, char ** from_20_16, char ** from_15_11, char ** from_10_6, char ** from_5_0);
int* instr_in_binar(LIST list_instr, int size_list, LIST dictionnaire);
int* data_in_binar(LIST list_data, int size_list);
int lengh_of_tab_data_in_binar(LIST l );
int lengh_of_space_in_bss(LIST l);
int swap(int value);


#endif
