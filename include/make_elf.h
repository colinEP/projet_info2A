#ifndef _MAKE_ELF_H_
#define _MAKE_ELF_H_

#include <queue_list.h>


int main_init_function(int* text_prog, int* data_prog, char** sym_char, int size_instr, int size_data, int size_table, int spaces_needed_in_bss, LIST symb_table, LIST reloc_table_text, LIST reloc_table_data, int size_reloc_text, int size_reloc_data);
//int main_init_function();



#endif
