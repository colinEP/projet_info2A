#ifndef _RELOC_H_
#define _RELOC_H_
#include <fct_analyse_1.h>

typedef enum {R_MIPS_32=2, R_MIPS_26=4, R_MIPS_HI16=5, R_MIPS_LO16=6} reloc_type;

reloc_type find_R_type(inst_op_type type) ;
LIST reloc_and_replace_etiq_by_dec_in_data (LIST l, LIST symb_table);
LIST reloc_and_replace_etiq_by_dec_in_instr (LIST l, LIST symb_table);


#endif
