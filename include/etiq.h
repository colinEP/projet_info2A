#ifndef _ETIQ_H_
#define _ETIQ_H_
#include <fct_analyse_1.h>
#include <reloc.h>

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
ETIQ look_for_etiq_and_return( LIST symb_table, char* lexem);


#endif
