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
#include <print_functions.h>

#include <error.h>
#include <assert.h>

enum{TEXT, PDATA, BSS, NONE};


void analyse_synth(LIST list_instr, LIST list_data, LIST list_bss, LIST symb_table, LIST list_lex )
{

        LIST dictionnaire;
        dictionnaire = open_dict("dictionnaire.txt"); // pas de seg fault

        // --- initialisation des variables ----
        int dec_data = 0; //décalage
        int dec_bss = 0;
        int dec_txt = 0;
        int* pdecalage = NULL;
        int section = NONE ;
        LEXEM lexem = NULL;
        int type_lexem;
        int previous_type_lexem;
        char* val_lexem;
        int line;
        LIST* pcurrent_list = NULL; // considérer le cas où il est à NULL alors qu'on rentre dans une autre instruction que definition de SECTION
        int nb_arg_ligne = 0;
        int nb_arg_needed = 0;
        int val = 0 ; // variable pour look_for_inst
        int S = START;

        // --- premier parcours de la liste ----

        while (list_lex != NULL)
        {   lexem = list_lex-> element;
            type_lexem = lexem->lex_type;
            val_lexem = lexem->value;
            line = lexem -> nline;

            switch (S)
            {
                case START: // cas lexeme de début de ligne
                // ------------ CASE S = START -----------------
                    switch(type_lexem) {
                        case SYMBOLE:
                        // cas 1 : une déclaration d'étiquette ? (on commence par ce cas car certaines etiquettes ont le même nom que des instructions)
                            if ( ( ((LEXEM)((LIST)(list_lex->next))->element)->lex_type) == DEUX_PTS) {
                                if (look_for_etiq(symb_table, val_lexem) == 1){
                                    printf("ERREUR LIGNE : %d\n", line);
                                    ERROR_MSG("Redefinition d'etiquette !\n");
                                }
                                symb_table = add_to_symb_table(val_lexem, *pdecalage, line, section, symb_table);

                                list_lex = list_lex->next; // on va passer les DEUX_PTS


                                {
                                 if (( ((LEXEM)((LIST)(list_lex->next))->element)->lex_type) == NL) {
                                     list_lex = list_lex->next; // on saute le NL qui suit
                                     S = START;
                                     break;
                                 }

                                 else {
                                     printf("ERREUR LIGNE : %d\n", line);
                                     ERROR_MSG("Erreur, elements non valides apres un symbole deux-points !\n");
                                 }

                                }
                            }

                             // cas 2 : une instruction ?
                            val = look_for_inst(val_lexem, dictionnaire, &nb_arg_needed);

                            if (val == 1)
                            {
                                if (section != TEXT) // pas d'instruction possible hors de la section .Text
                                    {
                                        printf("ERREUR LIGNE : %d\n", line);
                                        ERROR_MSG("Erreur, instruction dans mauvaise section !\n");
                                    }
                                list_instr =  add_to_list_instr(lexem, *pdecalage, nb_arg_needed, list_instr);
                                S = INSTRUCTION;
                                break;
                            }
                            // cas 3
                            else { // c'est une erreur car une ligne ne peut pas commencer par un appel d'etiquette !
                                printf("ERREUR LIGNE : %d\n", line);
                                ERROR_MSG("Erreur, symbole non valide en debut de ligne !\n");
                            }


                        case COMMENT: //on ne considère pas les commentaires, on ne les stocke pas

                            if (( ((LEXEM)((LIST)(list_lex->next))->element)->lex_type) == NL) {
                                list_lex = list_lex->next; // on saute le NL qui suit
                                break;
                            }

                            printf("ERREUR LIGNE : %d\n", line);
                            ERROR_MSG("Erreur, après le commentaire !\n"); // est-ce possible ???


                        case NL: //on ne considère pas les commentaires, on ne les stocke pas

                            break;


                        case DIRECTIVE:

                            // ---- modification de la section et de la liste courante ----
                            if (strcmp(val_lexem, ".data") == 0){
                                section = PDATA;
                                pcurrent_list = &list_data;
                                pdecalage = &dec_data;
                                if ( ( ((LEXEM)((LIST)(list_lex->next))->element)->lex_type) == NL) {
                                     list_lex = list_lex-> next; // on va passer la NL
                                     S = START;
                                     break;
                                }
                                printf("ERREUR LIGNE : %d\n", line);
                                ERROR_MSG("invalide element apres un .data !\n");
                            }

                            if (strcmp(val_lexem, ".bss") == 0){
                                section = BSS;
                                pcurrent_list = &list_bss;
                                pdecalage = &dec_bss;
                                if ( ( ((LEXEM)((LIST)(list_lex->next))->element)->lex_type) == NL) {
                                     list_lex = list_lex-> next; // on va passer la NL
                                     S = START;
                                     break;
                                }

                                printf("ERREUR LIGNE : %d\n", line);
                                ERROR_MSG("invalide element apres un .bss !\n");
                            }

                            if (strcmp(val_lexem, ".text") == 0){
                                section = TEXT;
                                pcurrent_list = &list_instr;
                                pdecalage = &dec_txt;
                                if ( ( ((LEXEM)((LIST)(list_lex->next))->element)->lex_type) == NL) {
                                     list_lex = list_lex-> next; // on va passer la NL
                                     S = START;
                                     break;
                                }

                                printf("ERREUR LIGNE : %d\n", line);
                                ERROR_MSG("invalide element apres un .text !\n");
                            }

                            // ----cas du .set ---

                            if (strcmp(val_lexem, ".set") == 0){
                                if ( ( ((LEXEM)((LIST)(list_lex->next))->element)->lex_type) != NL) {
                                     list_lex = list_lex-> next; // on va passer l'argument
                                }
                                S = START;
                                list_lex = list_lex->next; // on saute le NL qui suit
                                break;
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
                                    if ( ( ((LEXEM)((LIST)(list_lex->next))->element)->lex_type) != NL) {
                                         list_lex = list_lex-> next; // on va passer l'argument
                                    }
                                    S = START;
                                    list_lex = list_lex->next; // on saute le NL qui suit
                                    break;
                                }
                                else{
                                    printf("ERREUR LIGNE : %d\n", line);
                                    ERROR_MSG("Directive inconnue ou dans mauvaise section !\n"); // est-ce possible ???
                                }
                            }

                            if (section == BSS ){
                                if (strcmp(val_lexem, ".space") == 0){
                                    S = PSPACE;
                                    break;
                                }
                                if (strcmp(val_lexem, ".set") == 0){ // peut-il y avoir un set ici ??
                                    if ( ( ((LEXEM)((LIST)(list_lex->next))->element)->lex_type) != NL) {
                                         list_lex = list_lex-> next; // on va passer l'argument
                                    }
                                    S = START;
                                    list_lex = list_lex->next; // on saute le NL qui suit
                                    break;
                                }

                                else {
                                    printf("ERREUR LIGNE : %d\n", line);
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
                                    if ( ( ((LEXEM)((LIST)(list_lex->next))->element)->lex_type) != NL) {
                                         list_lex = list_lex-> next; // on va passer l'argument
                                    }
                                    S = START;
                                    list_lex = list_lex->next; // on saute le NL qui suit
                                    break;
                                }

                                else{
                                    printf("ERREUR LIGNE : %d\n", line);
                                    ERROR_MSG("Directive inconnue ou dans mauvaise section !\n"); // est-ce possible ???
                                }
                            }

                        default :
                            printf("ERREUR LIGNE : %d\n", line);
                            ERROR_MSG("Element non acceptable en debut de ligne !\n");
                    }
                    break;

                        // ------------ FIN CASE S = START -----------------

                case INSTRUCTION:
                    if ((type_lexem == VIRGULE) || (type_lexem == DEUX_PTS)  || (type_lexem == DIRECTIVE) || (type_lexem == STRING)){
                        printf("ERREUR LIGNE : %d\n", line);
                        ERROR_MSG("Element non acceptable apres une instruction !\n");
                    }

                    if (type_lexem == MOINS){
                        break;
                    }

                    else {
                        if (((type_lexem == NL)||(type_lexem == COMMENT)) && (previous_type_lexem != MOINS)) { // plus d'argument apres l'instruction ou apres la virgule
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
                                printf("ERREUR LIGNE : %d\n", line);
                                ERROR_MSG("Virgule de trop !\n");
                            }
                            if (previous_type_lexem == VIRGULE) {
                                printf("ERREUR LIGNE : %d\n", line);
                                ERROR_MSG("Argument manquant (ou de trop) apres instruction!\n");
                            }

                        }
                        if ((( ((LEXEM)((LIST)(list_lex->next))->element)->lex_type) == NL)|| (( ((LEXEM)((LIST)(list_lex->next))->element)->lex_type) == COMMENT) || (( ((LEXEM)((LIST)(list_lex->next))->element)->lex_type) == VIRGULE)) {
                            list_instr = fill_arguments(lexem, list_instr, previous_type_lexem);
                            nb_arg_ligne = nb_arg_ligne + 1;

                            if (type_lexem == SYMBOLE){ // cas etiquette
                                if (look_for_etiq(symb_table, val_lexem) == 1){         // si cette etiq est deja définie
                                     ((INSTR)((list_instr)->element))->etiq_def = 1;
                                }
                            }

                            if (( ((LEXEM)((LIST)(list_lex->next))->element)->lex_type) == VIRGULE) {
                                S = INSTRUCTION;
                                list_lex = list_lex->next; // saute NL
                                break;
                            }
                            if ((( ((LEXEM)((LIST)(list_lex->next))->element)->lex_type) == NL)|| (( ((LEXEM)((LIST)(list_lex->next))->element)->lex_type) == COMMENT) ) {
                                if (nb_arg_ligne != nb_arg_needed)
                                {
                                    printf("ERREUR LIGNE : %d\n", line);
                                    ERROR_MSG("Erreur, mauvais nombre d'arguments pour instruction !\n");
                                }
                                S = START;
                                nb_arg_ligne = 0;
                                nb_arg_needed= 0;
                                list_lex = list_lex->next; // saute Virgule
                                break;
                            }

                        }

                        else {
                            printf("ERREUR LIGNE : %d\n", line);
                            ERROR_MSG("Virgule manquante apres un argument !\n");
                        }
                    }


                case PWORD:

                    if ((type_lexem == VIRGULE) || (type_lexem == DEUX_PTS) || (type_lexem == DIRECTIVE) || (type_lexem == STRING)){
                        printf("ERREUR LIGNE : %d\n", line);
                        ERROR_MSG("Element non acceptable apres un .word !\n");
                    }

                    if (type_lexem == SYMBOLE) { // recherche étiquette dans symb_table
                        *pcurrent_list = add_to_current_list(LABEL, val_lexem, *pdecalage, line, *pcurrent_list);// stocke. Par defaut etiq_def est à 0;
                        if (look_for_etiq(symb_table, val_lexem) == 1){ // si etiq deja définie
                             ((DATA)((*pcurrent_list)->element))->etiq_def = 1;
                        }
                        if ( (( (LEXEM)((LIST)(list_lex->next))->element) ->lex_type) == VIRGULE ) {
                            list_lex = list_lex->next; // on saute la virgule qui suit
                            break;
                        }
                        if ((( ((LEXEM)((LIST)(list_lex->next))->element)->lex_type) == NL)|| (( ((LEXEM)((LIST)(list_lex->next))->element)->lex_type) == COMMENT) ) {
                            list_lex = list_lex->next; // on saute la NL qui suit
                            S = START;
                            nb_arg_ligne = 0;
                            nb_arg_needed= 0;
                            break;
                        }
                        else {
                            printf("ERREUR LIGNE : %d\n", line);
                            ERROR_MSG("Element invalide apres un argument !\n");
                        }
                    }

                    if ((type_lexem == HEXA) || (type_lexem == OCTAL) || (type_lexem == DECIMAL) || (type_lexem == AIBD)|| (type_lexem == REGISTRE)){
                        // conversion
                        // considère le cas previous_type_lexem = MOINS
                        // Stockage : *pcurrent_list = add_to_current_list(PWORD, "valeur", decalage, ligne, *pcurrent_list);

                        if ( (( (LEXEM)((LIST)(list_lex->next))->element) ->lex_type) == VIRGULE ) {
                            list_lex = list_lex->next; // on saute la virgule qui suit
                            break;
                        }
                        if ((( ((LEXEM)((LIST)(list_lex->next))->element)->lex_type) == NL)|| (( ((LEXEM)((LIST)(list_lex->next))->element)->lex_type) == COMMENT) ) {
                            list_lex = list_lex->next; // on saute la NL qui suit
                            S = START;
                            nb_arg_ligne = 0;
                            nb_arg_needed= 0;
                            break;
                        }
                        else {
                            printf("ERREUR LIGNE : %d\n", line);
                            ERROR_MSG("Virgule manquante apres un argument !\n");
                        }
                    }

                case PASCIIZ:

                        if (type_lexem == STRING) {
                            // CONVERSION TO DO ! car pour l'instant on stocke : ""chaine de char""
                            *pcurrent_list = add_to_current_list(PASCIIZ, val_lexem, *pdecalage, line, *pcurrent_list);
                            if ( (( (LEXEM)((LIST)(list_lex->next))->element) ->lex_type) == VIRGULE ) {
                                list_lex = list_lex->next; // on saute la virgule qui suit
                                break;
                            }
                            if ((( ((LEXEM)((LIST)(list_lex->next))->element)->lex_type) == NL)|| (( ((LEXEM)((LIST)(list_lex->next))->element)->lex_type) == COMMENT) ) {
                                list_lex = list_lex->next; // on saute la NL qui suit
                                S = START;
                                nb_arg_ligne = 0;
                                nb_arg_needed= 0;
                                break;
                            }
                            else {
                                printf("ERREUR LIGNE : %d\n", line);
                                ERROR_MSG("Element invalide apres un argument !\n");
                            }
                        }

                        if (type_lexem == SYMBOLE) { // recherche étiquette dans symb_table
                            *pcurrent_list = add_to_current_list(LABEL, val_lexem, *pdecalage, line, *pcurrent_list);// stocke. Par defaut etiq_def est à 0;
                            if (look_for_etiq(symb_table, val_lexem) == 1){ // si etiq deja définie
                                 ((DATA)((*pcurrent_list)->element))->etiq_def = 1;
                            }
                            if ( (( (LEXEM)((LIST)(list_lex->next))->element) ->lex_type) == VIRGULE ) {
                                list_lex = list_lex->next; // on saute la virgule qui suit
                                break;
                            }
                            if ((( ((LEXEM)((LIST)(list_lex->next))->element)->lex_type) == NL)|| (( ((LEXEM)((LIST)(list_lex->next))->element)->lex_type) == COMMENT) ) {
                                list_lex = list_lex->next; // on saute la NL qui suit
                                S = START;
                                nb_arg_ligne = 0;
                                nb_arg_needed= 0;
                                break;
                            }
                            else {
                                printf("ERREUR LIGNE : %d\n", line);
                                ERROR_MSG("Element invalide apres un argument !\n");
                            }
                        }

                        else {
                            printf("ERREUR LIGNE : %d\n", line);
                            ERROR_MSG("Element non acceptable apres un .asciiz !\n");
                        }

                case PSPACE :

                    // Seuls les entiers non signés sont acceptés
                    if (previous_type_lexem == MOINS)
                    {
                        printf("ERREUR LIGNE : %d\n", line);
                        ERROR_MSG("Seuls entiers non signes acceptes apres .space !\n");
                    }

                    if (type_lexem == SYMBOLE) { // recherche étiquette dans symb_table --> il faudra vérifier que son type CONVIENT
                        *pcurrent_list = add_to_current_list(LABEL, val_lexem, *pdecalage, line, *pcurrent_list);// stocke. Par defaut etiq_def est à 0;
                        if (look_for_etiq(symb_table, val_lexem) == 1){ // si etiq deja définie
                             ((DATA)((*pcurrent_list)->element))->etiq_def = 1;
                        }
                        break;
                    }

                    if ((type_lexem == VIRGULE) || (type_lexem == DEUX_PTS) || (type_lexem == DIRECTIVE) || (type_lexem == STRING)){
                        printf("ERREUR LIGNE : %d\n", line);
                        ERROR_MSG("Element non acceptable apres un .word !\n");
                    }


                    // autres cas : faut il tout convertir aussi et les accepter ???

                case PBYTE:
                        break; // a changer


                // autres cas ?



        } // fin switch

        previous_type_lexem = type_lexem; // on garde en mémoire pour le cas du MOINS
        list_lex = list_lex->next;

    } // fin while

    print_list_instr(list_instr);
    print_symb_table(symb_table);
    print_list_data(list_data);

    // --- deuxième parcours de la liste ----



        return;
}
