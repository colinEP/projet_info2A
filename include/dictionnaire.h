#ifndef _DICO_H_
#define _DICO_H_


typedef struct {   // structure qui stocke une ligne du dico d'instructions
    int arg;
    char* instruction;
    char* arg_type_1;
    char* arg_type_2;
    char* arg_type_3;
    int opcode;
    char* code_bin_1;
    char* code_bin_2;
    char* code_bin_3;
    char* code_bin_4;
    char* code_bin_5;
} *DICO_LINE;

LIST open_dict(char *file);

int look_for_inst(char* lex_init, LIST l_dico, int* pnb_arg, int* exp_typ_1, int* exp_typ_2, int* exp_typ_3);

QUEUE add_definition ( QUEUE Q, char* inst, int nb_arg, char* a_type_1, char* a_type_2, char* a_type_3,int opcode, char* c_b_1, char* c_b_2, char* c_b_3, char* c_b_4, char* c_b_5);

char* put_in_uppercase (char* chaine);

#endif
