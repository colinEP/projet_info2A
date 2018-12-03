#ifndef _FCT_ANALYSE_H_
#define _FCT_ANALYSE_H_

typedef enum{START, INSTRUCTION, PWORD, PBYTE, PASCIIZ, PSPACE, LABEL, DEC_LABEL} operand_type;

typedef enum{None, Reg, Imm, Sa, Bas, Rel, Abs, Label, Target, Bas_Target} inst_op_type;

enum{TEXT, PDATA, BSS, NONE};

typedef struct{
    operand_type type;
    union {
        char PBYTE;
        int PWORD;
        char* PASCIIZ;
        char* LABEL;
        unsigned int PSPACE;
        int DEC_LABEL; // pour stocker le decalage de l'étiquette
    } val;
}* data_op;


typedef struct {
    data_op D;
    int decalage;
    int line;
    int etiq_def;
}* DATA;


typedef struct {
    inst_op_type type;
    union {
        // int Reg;
        // int Imm;
        // int Sa;
        // int Rel;
        // int Abs;
        // char* Bas;
        // char* Label ou int Label selon av ou ap replace_etiq_by_dec
        long int entier; // WARNING long ?!
        char* char_chain;
    } val;
    int etiq_def;
}* ARG_INST;


typedef struct {
    LEXEM lex;   //NOTE utiliser name car confusion avec lex ci dessus
    int nb_arg;
    int decalage;
    ARG_INST arg1;
    ARG_INST arg2;
    ARG_INST arg3;
    // il faut stocker ici le type d'arg expected pour pouvoir, après relocation, vérifier le bon type pointé par l'étiquette ?!
    inst_op_type Exp_Type_1;
    inst_op_type Exp_Type_2;
    inst_op_type Exp_Type_3;

}* INSTR;


INSTR new_instr();
//LIST add_to_list_instr(LEXEM l, int dec, int nbarg, LIST list_instr);
LIST add_to_list_instr(LEXEM l, int dec, int nbarg, LIST list_instr, int exp_typ_1, int exp_typ_2, int exp_typ_3);
//LIST fill_arguments(LEXEM lexem, LIST list_instr, int previous_type_lexem, int etiq_definition);
LIST fill_arguments(LEXEM lexem, LIST list_instr, int previous_type_lexem, int etiq_definition, int nb_arg_ligne);
LIST add_label(int nb_arg_ligne, inst_op_type type, char* value, int etiq_definition, LIST list_instr);
LIST add_int(int nb_arg_ligne, inst_op_type type, int valeur, int etiq_definition,LIST list_instr);
LIST add_to_current_list(operand_type type_op, void* pvalue, int dec, int line, LIST current_list);
DATA new_data();
data_op fill_val_op(void* pvalue, operand_type type_op);
LIST look_for_undefined_etiq_in_data(LIST l, LIST symb_table);
LIST look_for_undefined_etiq_in_instr(LIST l, LIST symb_table);
LIST look_for_undefined_etiq_in_bss(LIST l, LIST symb_table);
char *mystrcat( char *start, char *addend );
inst_op_type convert_inst_op_type(char* type);
int upper_16(int val_32b);
int lower_16(int val_32b);

// dans pseudo_instr.c
LIST change_pseudo_instr(LIST list_instr, int* pdecalage);
LIST change_pseudo_SW_LW(LIST list_instr, int* pdecalage);
int upper_16(int val_32b);
int lower_16(int val_32b);


#endif
