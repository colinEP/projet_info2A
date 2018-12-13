#ifndef _ETIQ_H_
#define _ETIQ_H_
#include <fct_analyse_1.h>
#include <reloc.h>

typedef struct {
    char* name;
    int nline;
    int section;
    int decalage;
    int def_in_file; // a 1 si oui, sinon 0
}* ETIQ;


int look_for_etiq( LIST symb_table, char* val_lexem);
LIST add_to_symb_table(char* etiq, int decalage, int line, int section, int def, LIST symb_table);
ETIQ look_for_etiq_and_return( LIST symb_table, char* lexem);
char** make_sym_char_table(LIST symb_table, int size_list);


#endif
