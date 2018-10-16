#ifndef _DICO_H_
#define _DICO_H_


typedef struct
{
    int arg;
    char* instruction;
} *WORD; // structure qui prend les mots du dictionnaire stockés


QUEUE open_dict(char *file);

int look_for_inst(char* lex, QUEUE l_dico);

QUEUE add_definition ( QUEUE Q, int nb_arg, char* instruction);

QUEUE ajouter_def_fin(QUEUE Q, WORD def);


#endif
