#ifndef _ETIQ_H_
#define _ETIQ_H_

typedef enum {R_MIPS_32=2, R_MIPS_26=4, R_MIPS_HI16=5, R_MIPS_LO16=6} reloc_type;

typedef struct {
    char* name;
    int nline;
    int section;
    int decalage;
    int def_in_file;
}* ETIQ;

typedef struct {
    char* section; //INUTILE NON ? Puisque spécifié par reloc_instr ou reloc_data
    unsigned int adress;
    reloc_type type_r;
    char* addend; // valeur de la section de definition OU nom de l'etiq si non def
}* RELOC;


int look_for_etiq( LIST symb_table, char* val_lexem);
LIST add_to_symb_table(char* etiq, int decalage, int line, int section, int def, LIST symb_table);
LIST reloc_and_replace_etiq_by_dec_in_data (LIST l, LIST symb_table);
LIST reloc_and_replace_etiq_by_dec_in_instr (LIST l, LIST symb_table);
ETIQ look_for_etiq_and_return( LIST symb_table, char* lexem);
reloc_type find_R_type(char* instruction);

#endif
