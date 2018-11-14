#ifndef _DICO_H_
#define _DICO_H_


typedef struct
{
    int arg;
    char* instruction;
    char* arg_type_1;
    char* arg_type_2;
    char* arg_type_3;
} *WORD; // structure qui prend les mots du dictionnaire stockés


QUEUE open_dict(char *file);

int look_for_inst(char* lex, LIST l_dico, int* nb_arg);

QUEUE add_definition ( QUEUE Q, int nb_arg, char* instruction);



#endif
