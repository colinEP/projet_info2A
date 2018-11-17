#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <ctype.h>

#include <global.h>
#include <notify.h>
#include <lex.h>
#include <queue_list.h>
#include <test.h>
#include <dictionnaire.h>
#include <etiq.h>
#include <fct_analyse_1.h>
#include <analyse_synth.h>
#include <print_functions.h>

#include <error.h>
#include <assert.h>

// par définition, un registre commence nécessairement par $ mais on va quand même s'en assurer

int check_and_convert_register(char* reg)
{
    int convert_register;

    if (reg[0] != '$')
    {
        ERROR_MSG("Erreur, mauvaise attribution du type registre !\n");
    }

    if (strlen(reg) == 2) // il n'y a qu'un terme après le &
    {
        if (isdigit(reg[1]))
        {
            convert_register = reg[1]-'0';
            return convert_register;
        }
        ERROR_MSG("Erreur, registre inexistant !\n");
    }


    if (strlen(reg) == 3) // il y a deux termes après
    {
        if ((isdigit(reg[1])) && (isdigit(reg[2]))) // deux chiffres
        {
            convert_register = 10*(reg[1]-'0') + reg[2]-'0';

            if ((convert_register > 9) && (convert_register < 32) ) return convert_register;
            ERROR_MSG("Erreur, registre inexistant !\n");
        }

        if (reg[1] == 'v'){
            if (isdigit(reg[2])){
                convert_register = reg[2]-'0' + 2 ;
                if ((convert_register == 2) || (convert_register == 3) ) return convert_register;
            }
            ERROR_MSG("Erreur, registre inexistant !\n");
        }

        if (reg[1] == 'a'){
            if (isdigit(reg[2])){
                convert_register = reg[2]-'0' + 4 ;
                if ((convert_register > 3) && (convert_register < 8) ) return convert_register;
                ERROR_MSG("Erreur, registre inexistant !\n"); //Inutile ?
            }
            if (reg[2] == 't') return 1;
            ERROR_MSG("Erreur, registre inexistant !\n");
        }

        if (reg[1] == 't'){
            if (isdigit(reg[2])){
                convert_register = reg[2]-'0' + 8 ;
                if ((convert_register > 7) && (convert_register < 16) ) return convert_register;
                if ((convert_register == 16) || (convert_register == 17) ) return convert_register + 8;
            }
            ERROR_MSG("Erreur, registre inexistant !\n");
        }

        if (reg[1] == 's'){
            if (isdigit(reg[2])){
                convert_register = reg[2]-'0' + 16 ;
                if ((convert_register > 15) && (convert_register < 24) ) return convert_register;
                ERROR_MSG("Erreur, registre inexistant !\n"); // inutile ?
            }
            if (reg[2] == 'p') return 29;
            ERROR_MSG("Erreur, registre inexistant !\n");
        }

        if (reg[1] == 'k'){
            if (isdigit(reg[2])){
                convert_register = (reg[2]-'0') + 26 ;
                if ((convert_register == 26) || (convert_register == 27) ) return convert_register;
            }
            ERROR_MSG("Erreur, registre inexistant !\n");
        }
        if ((reg[1]== 'g') && (reg[2] == 'p')) return 28;
        if ((reg[1]== 'f') && (reg[2] == 'p')) return 30;
        if ((reg[1]== 'r') && (reg[2] == 'a')) return 31;
        ERROR_MSG("Erreur, registre inexistant !\n");
    }

    if (strcmp(reg, "$zero") == 0 ) return 0;
    ERROR_MSG("Erreur, registre inexistant !\n");

}


// TODO

int check_type_arg_inst(int type_lexem, char* val_lexem, int type_arg_expected)
{
    if (type_arg_expected == Reg){
        if (type_lexem == REGISTRE) {
            // int new_val_reg;
            // new_val_reg = check_and_convert_register(val_lexem); // a mettre ici ??
            // return new_val_reg;
            return 1;
        }
        // le cas d'étiquette n'est pas à prendre en compte ici car considéré dans la machine à états
        ERROR_MSG("Erreur, type_registre attendu pour argument !\n");
    }

    if (type_arg_expected == Imm){
        // TODO
        return 1;
    }

    if (type_arg_expected == Rel){
        // TODO
        return 1;
    }

    if (type_arg_expected == Abs){
        // TODO
        return 1;
    }

    if (type_arg_expected == Sa){
        // TODO
        return 1;
    }

    if (type_arg_expected == Bas){
        // TODO
        return 1;
    }

    if (type_arg_expected == None){
        ERROR_MSG("Erreur, argument en trop !\n");
    }

    else ERROR_MSG("Erreur dans fonction check_type_arg_inst !\n");
}
