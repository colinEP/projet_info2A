#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>

#include <global.h>
#include <notify.h>
#include <lex.h>
#include <queue_list.h>
#include <test.h>
#include <dictionnaire.h>
#include <etiq.h>
#include <fct_analyse_1.h>
#include <analyse_synth.h>

#include <error.h>
#include <assert.h>

enum{TEXT, PDATA, BSS, NONE};


// int check_instruction(char* lex, LIST dictionnaire) // vérifie que l'instruction existe et renvoit son nb d'arg = INUTILE ?
// {
//     int val;
//     int nb_arg_needed;
//     val = look_for_inst(lex, dictionnaire, &nb_arg_needed);
//     if (val == 0)
//         {
//             ERROR_MSG("Erreur, instruction %s non valable !\n", lex); // rajout num de ligne ?
//         }
//     return nb_arg_needed;
//
// }

void analyse_synth(LIST list_instr, LIST list_data, LIST list_bss, LIST symb_table, LIST list_lex )
{

        LIST dictionnaire;
        dictionnaire = open_dict("dictionnaire.txt");

        // --- initialisation des variables ----
        int dec_data = 0; //décalage
        int dec_bss = 0;
        int dec_txt = 0;
        int section = NONE ;
        LEXEM lexem = NULL;
        int type_lexem;
        int previous_type_lexem;
        char* val_lexem;
        LIST* current_list = NULL; // est-ce que cela peut nous poser de soucis ??
        int nb_arg_ligne = 0;
        int nb_arg_needed = 0;
        int val = 0 ; // variable pour look_for_inst
        int S = START;


        // --- premier parcours de la liste ----

        while (list_lex != NULL)
        {   lexem = list_lex-> element;
            type_lexem = lexem->lex_type;
            val_lexem = lexem->value;

            switch (S)
            {
                case START: // cas lexeme de début de ligne

                // ------------ CASE S = START -----------------
                    switch(type_lexem) {
                        case SYMBOLE:
                        // cas 1 : une déclaration d'étiquette ? (on commence par ce cas car certaines etiquettes ont le même nom que des instructions)
                            if ( ( ((LEXEM)((LIST)(list_lex->next))->element)->lex_type) == DEUX_PTS) {
                                if (look_for_etiq(symb_table, val_lexem) == 1){
                                    ERROR_MSG("Redefinition d'etiquette !\n");
                                }
                                symb_table = add_to_symb_table(val_lexem, decalage, line, section, symb_table);

                                list_lex = list_lex->next; // on va passer les DEUX_PTS

                                {
                                 if (( ((LEXEM)((LIST)(list_lex->next))->element)->lex_type) == NL) {
                                     list_lex = list_lex->next; // on saute le NL qui suit
                                     S = START;
                                     break;
                                 }

                                 else {
                                     ERROR_MSG("Erreur, elements non valides apres un symbole deux-points !\n");
                                 }

                             }

                             // cas 2 : une instruction ?
                            val = look_for_inst(val_lexem, dictionnaire, &nb_arg_needed);
                            if (val == 1)
                            {
                                if (section != TEXT) // pas d'instruction possible hors de la section .Text
                                    {
                                        ERROR_MSG("Erreur, instruction dans mauvaise section !\n");
                                    }
                                list_instr =  add_to_list_instr(lexem, decalage, nb_arg_needed, list_instr); // check !
                                S = INSTRUCTION;
                                break;
                            }
                            // cas 3
                            else { // c'est une erreur car une ligne ne peut pas commencer par un appel d'etiquette !
                                ERROR_MSG("Erreur, symbole non valide en debut de ligne !\n");
                            }


                        case COMMENT: //on ne considère pas les commentaires, on ne les stocke pas

                            if (( ((LEXEM)((LIST)(list_lex->next))->element)->lex_type) == NL) {
                                list_lex = list_lex->next; // on saute le NL qui suit
                                break;
                            }

                            ERROR_MSG("Erreur, après le commentaire !\n"); // est-ce possible ???


                        case DIRECTIVE:

                            // ---- modification de la section et de la liste courante ----
                            if (strcmp(val_lexem, ".data") == 0){
                                section = PDATA;
                                *current_list = list_data;
                                if ( ( ((LEXEM)((LIST)(list_lex->next))->element)->lex_type) == NL) {
                                     list_lex = list_lex-> next; // on va passer la NL
                                     break;
                                }
                                ERROR_MSG("invalide element apres un .data !\n");
                            }

                            if (strcmp(val_lexem, ".bss") == 0){
                                section = BSS;
                                *current_list = list_bss;
                                if ( ( ((LEXEM)((LIST)(list_lex->next))->element)->lex_type) == NL) {
                                     list_lex = list_lex-> next; // on va passer la NL
                                     break;
                                }
                                ERROR_MSG("invalide element apres un .bss !\n");
                            }

                            if (strcmp(val_lexem, ".text") == 0){
                                section = TEXT;
                                *current_list = list_instr;
                                if ( ( ((LEXEM)((LIST)(list_lex->next))->element)->lex_type) == NL) {
                                     list_lex = list_lex-> next; // on va passer la NL
                                     break;
                                }
                                ERROR_MSG("invalide element apres un .text !\n");
                            }


                            // ---- autres directives ----
                            if (section == PDATA ){
                                if (strcmp(val_lexem, ".word") == 0){
                                    S = PWORD;
                                    break;
                                }
                                if (strcmp(val_lexem, ".byte") == 0){
                                    S = PBYTE;
                                    break;
                                }
                                if (strcmp(val_lexem, ".asciiz") == 0){
                                    S = PASCIIZ;
                                    break;
                                }
                                if (strcmp(val_lexem, ".set") == 0){
                                    list_lex = (LIST)(list_lex->next)->next; // on saute l'argument et le NL qui suit ??
                                    break;
                                }
                                else{
                                    ERROR_MSG("Directive inconnue ou dans mauvaise section !\n"); // est-ce possible ???
                                }
                            }

                            if (section == BSS ){
                                if (strcmp(val_lexem, ".space") == 0){
                                    S = PSPACE;
                                    break;
                                }
                                if (strcmp(val_lexem, ".set") == 0){ // peut-il y avoir un set ici ??
                                    list_lex = (LIST)(list_lex->next)->next; // on saute l'argument et le NL qui suit ??
                                    break;
                                }

                                else {
                                    ERROR_MSG("Directive inconnue ou dans mauvaise section !\n"); // est-ce possible ???
                                }
                            }

                            if (section == TEXT ){
                                if (strcmp(val_lexem, ".word") == 0){
                                    S = PWORD;
                                    break;
                                }
                                if (strcmp(val_lexem, ".byte") == 0){
                                    S = PBYTE;
                                    break;
                                }
                                if (strcmp(val_lexem, ".asciiz") == 0){
                                    S = PASCIIZ;
                                    break;
                                }
                                if (strcmp(val_lexem, ".space") == 0){
                                    S = PSPACE;
                                    break;
                                }
                                if (strcmp(val_lexem, ".set") == 0){
                                    list_lex = (LIST)(list_lex->next)->next; // on saute l'argument et le NL qui suit ??
                                    break;
                                }

                                else{
                                    ERROR_MSG("Directive inconnue ou dans mauvaise section !\n"); // est-ce possible ???
                                }
                            }

                        default :
                            ERROR_MSG("Element non acceptable en debut de ligne !\n");
                    }
                    break;

                        // ------------ FIN CASE S = START -----------------

                case INSTRUCTION:
                    if ((type_lexem == VIRGULE) || (type_lexem == DEUX_PTS)  || (type_lexem == DIRECTIVE) || (type_lexem == STRING)){
                        ERROR_MSG("Element non acceptable apres une instruction !\n");
                    }

                    if (type_lexem == MOINS){
                        break;
                    }

                    else {
                        if (((type_lexem == NL)||(type_lexem == COMMENT)) && (previous_type_lexem != MOINS)) { // aucun argument apres l'instruction ou apres la virgule
                            if ((nb_arg_ligne == nb_arg_needed) && (previous_type_lexem != VIRGULE)) // cas où 0 arg
                            {
                                if (type_lexem == COMMENT){
                                    list_lex = list_lex->next; // on saute le NL qui suit
                                }
                                S = START;
                                nb_arg_ligne = 0;
                                nb_arg_needed= 0;
                                break;
                            }
                            if ((nb_arg_ligne == nb_arg_needed) && (previous_type_lexem == VIRGULE)) {
                                ERROR_MSG("Virgule de trop !\n");
                            }
                            if (previous_type_lexem == VIRGULE) {
                                ERROR_MSG("Argument manquant (ou de trop) apres instruction!\n");
                            }

                        }

                        if (( ((LEXEM)((LIST)(list_lex->next))->element)->lex_type) == VIRGULE) {
                            // prendre en considération le "-" !
                            list_instr = fill_arguments(lexem, list_instr, previous_type_lexem);
                            nb_arg_ligne = nb_arg_ligne + 1;
                            list_lex = list_lex->next; // on saute la virgule qui suit
                            S = INSTRUCTION;
                            break;
                        }
                        if ((( ((LEXEM)((LIST)(list_lex->next))->element)->lex_type) == NL)|| (( ((LEXEM)((LIST)(list_lex->next))->element)->lex_type) == COMMENT) ) {
                            // prendre en considération le "-" !
                            list_instr = fill_arguments(lexem, list_instr, previous_type_lexem);
                            nb_arg_ligne = nb_arg_ligne + 1;
                            if (nb_arg_ligne != nb_arg_needed)
                            {
                                ERROR_MSG("Erreur, mauvais nombre d'arguments pour instruction !\n");
                            }
                            list_lex = list_lex->next; // on saute la NL qui suit
                            S = START;
                            nb_arg_ligne = 0;
                            nb_arg_needed= 0;
                            break;
                        }
                        else {
                            ERROR_MSG("Virgule manquante apres un argument !\n");
                        }
                    }


                case PWORD:

                    if ((type_lexem == VIRGULE) || (type_lexem == DEUX_PTS) || (type_lexem == DIRECTIVE) || (type_lexem == STRING)){
                        ERROR_MSG("Element non acceptable apres un .word !\n");
                    }

                    if (type_lexem == SYMBOLE){ // recherche étiquette dans symb_table
                        if (look_for_etiq(symb_table, val_lexem) == 1){
                            // mettre à jour une variable qui indique que l'étiquette est bien déjà définie
                        }
                        // mettre à jour une variable qui indique que l'étiquette n'est pas (encore) définie
                    }

                    if ((type_lexem == HEXA) || (type_lexem == OCTAL) || (type_lexem == DECIMAL) || (type_lexem == AIBD)|| (type_lexem == REGISTRE)){
                        if ( (( (LEXEM)((LIST)(list_lex->next))->element) ->lex_type) == VIRGULE ) {
                            //Stockage
                            // mise à jour nb arg
                            // considère le cas MOINS
                            // etc.
                        }
                        if ((( ((LEXEM)((LIST)(list_lex->next))->element)->lex_type) == NL)|| (( ((LEXEM)((LIST)(list_lex->next))->element)->lex_type) == COMMENT) ) {
                            // Stockage
                            // mise à jour nb arg
                            // considère le cas MOINS
                            // etc.
                            // check nb arg
                            S = START;
                            nb_arg_ligne = 0;
                            nb_arg_needed= 0;
                        }
                    }


                // autres case directives to do idem


            }
        } // fin switch

        previous_type_lexem = type_lexem; // on garde en mémoire pour le cas du MOINS
        list_lex = list_lex->next;
    } // fin while

        // --- deuxième parcours de la liste ----
        if (list_lex == NULL)
        {
            ERROR_MSG("Erreur, liste lexemes a analyser vide !\n");
        }

        symb_table = build_tab_etiq(list_lex ); //cette fonction remplie la table des symboles avec les etiquettes et la renvoit


        return;
}
