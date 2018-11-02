#ifndef _FCT_ANALYSE_H_
#define _FCT_ANALYSE_H_

typedef enum{START, INSTRUCTION, PWORD, PBYTE, PASCIIZ, PSPACE, LABEL} operand_type; //est-ce que cela a du sens de rajouter LABEL là dedans?

typedef struct{
    operand_type type;
    union {
        char PBYTE;
        int PWORD;
        char* PASCIIZ;
        char* LABEL;
        unsigned int PSPACE;
    } val;
}* data_op;


typedef struct {
    data_op D;
    int decalage;
    int line;
    int etiq_def;
}* DATA;


typedef struct {
    LEXEM lex;
    int nb_arg;
    int decalage;
    LEXEM arg1; // à changer ?
    LEXEM arg2;
    LEXEM arg3;
}* INSTR;



INSTR new_instr();
LIST add_to_list_instr(LEXEM l, int dec, int nbarg, LIST list_instr);
LIST fill_arguments(LEXEM lexem, LIST list_instr, int previous_type_lexem);
LIST add_to_current_list(operand_type type_op, void* pvalue, int dec, int line, LIST current_list);
DATA new_data();
data_op fill_val_op(void* pvalue, operand_type type_op);
char *mystrcat( char *start, char *addend );


#endif
