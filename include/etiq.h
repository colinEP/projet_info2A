#ifndef _ETIQ_H_
#define _ETIQ_H_

typedef struct {
    char* name;
    int nline;
    int section;
    int decalage;
}* ETIQ;





int look_for_etiq( LIST symb_table, char* val_lexem);
LIST add_to_symb_table(char* etiq, int decalage, int line, int section, LIST symb_table);


#endif
