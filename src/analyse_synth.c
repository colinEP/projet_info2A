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
    dictionnaire = open_dict("dictionnaire.txt");

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
    //int val = 0 ; // variable pour look_for_inst
    int S = START;

    // --- premier parcours de la liste ----

    /* pourquoi suppr ça ? */
    // if (list_lex == NULL)
    // {
    //     ERROR_MSG("Erreur, liste lexemes a analyser vide !\n");
    // }



    while (list_lex != NULL)
    {
        lexem = list_lex-> element;
        type_lexem = lexem->lex_type;
        val_lexem = lexem->value;
        line = lexem -> nline;

        switch (S)
        {
            case START: // cas lexeme de début de ligne
            /* ------------ CASE S = START ----------------- */
                switch(type_lexem) {
                    case SYMBOLE:
                        // cas 1 : une déclaration d'étiquette ? (on commence par ce cas car certaines etiquettes ont le même nom que des instructions)
                        //if ( ( ((LEXEM)((LIST)(list_lex->next))->element)->lex_type) == DEUX_PTS) {
                        if ( ( (LEXEM)(list_lex->next->element))->lex_type == DEUX_PTS) {
                            //if (look_for_etiq(symb_table, val_lexem) == 1){
                            if (look_for_etiq(symb_table, val_lexem)) {
                                printf("ERREUR LIGNE : %d\n", line);
                                ERROR_MSG("Redefinition d'etiquette !\n");
                                // TODO msg d'erreur donnant les lignes de définition et rédéfinition
                            }
                            symb_table = add_to_symb_table(val_lexem, *pdecalage, line, section, symb_table);

                            list_lex = list_lex->next; // on va passer les DEUX_PTS
                            //NOTE pourquoi pas ->next->next

                            if ( ( ( (LEXEM)(list_lex->next->element))->lex_type == NL) || ( ( (LEXEM)(list_lex->next->element))->lex_type == COMMENT) ) {
                                list_lex = list_lex-> next; // on va passer le NL ou le COMMENT (on sera dans le case(NL) )
                                S = START;  // inutile ?
                                break;
                            }


                            // WARNING NOTE WARNING TODO TODO TODO
                            // WARNING NOTE WARNING TODO TODO TODO
                            // il peut y avoir plusieur étiquette a la suite !!!!!! ou l'instruction
                            // => ne pas test si NL mais passer direct au prochain case
                            // non faut sauter le NL
                            // WARNING NOTE WARNING TODO TODO TODO
                            // WARNING NOTE WARNING TODO TODO TODO
                            else {
                                printf("ERREUR LIGNE : %d\n", line);
                                ERROR_MSG("Erreur, elements non valides apres un symbole deux-points !\n");
                            }
                        }

                        // cas 2 : une instruction ?

                        //val = look_for_inst(val_lexem, dictionnaire, &nb_arg_needed);
                        //if (val == 1)
                        if ( look_for_inst(val_lexem, dictionnaire, &nb_arg_needed) )   // le else sert a rien normalement
                        {
                            if (section != TEXT) // pas d'instruction possible hors de la section .Text
                                {
                                    printf("ERREUR LIGNE : %d\n", line);
                                    ERROR_MSG("Erreur, instruction dans la mauvaise section !\n");
                                }
                            list_instr =  add_to_list_instr(lexem, *pdecalage, nb_arg_needed, list_instr);
                            S = INSTRUCTION;
                            break;
                        }
                        // cas 3 : erreur CAR LIGNE VIDE DEJA IGNORE DANS L'ANALYSE LEXICAL
                        else { // c'est une erreur car une ligne ne peut pas commencer par un appel d'etiquette !
                            printf("ERREUR LIGNE : %d\n", line);
                            ERROR_MSG("Erreur, symbole non valide en debut de ligne !\n");
                        }

                    case COMMENT: //on ne considère pas les commentaires, on ne les stocke pas

                        //if (( ((LEXEM)((LIST)(list_lex->next))->element)->lex_type) == NL) {
                        if ( ( (LEXEM)(list_lex->next->element))->lex_type == NL) {
                            list_lex = list_lex->next; // on saute le NL qui suit
                            break;
                        }
                        // else
                        printf("ERREUR LIGNE : %d\n", line);
                        ERROR_MSG("Erreur, après le commentaire !\n"); // est-ce possible ??? je crois pas, mais bon ...


                    case NL: // impossible je crois
                        //ON Y PASSE AVEC .set => on peut généraliser utilisation => NON !!!
                        //printf("AAAAAAA  %s\n\n\n\n\n\n\n\n",  ( (LEXEM)(list_lex->next->element))->value);
                        break;

                    case DIRECTIVE:
                        /* ---- modification de la section et de la liste courante ---- */
                        if (strcmp(val_lexem, ".data") == 0){
                            section = PDATA;
                            pcurrent_list = &list_data;
                            pdecalage = &dec_data;
                            //if ( ( ((LEXEM)((LIST)(list_lex->next))->element)->lex_type) == NL) {
                            if ( ( ( (LEXEM)(list_lex->next->element))->lex_type == NL) || ( ( (LEXEM)(list_lex->next->element))->lex_type == COMMENT) ) {
                                 list_lex = list_lex-> next; // on va passer le NL ou le COMMENT (on sera dans le case(NL) )
                                 S = START;
                                 break;
                            }
                            // else
                            printf("ERREUR LIGNE : %d\n", line);
                            ERROR_MSG("invalide element apres un .data !\n");
                        }

                        if (strcmp(val_lexem, ".bss") == 0){
                            section = BSS;
                            pcurrent_list = &list_bss;
                            pdecalage = &dec_bss;
                            //if ( ( ((LEXEM)((LIST)(list_lex->next))->element)->lex_type) == NL) {
                            if ( ( ( (LEXEM)(list_lex->next->element))->lex_type == NL) || ( ( (LEXEM)(list_lex->next->element))->lex_type == COMMENT) ) {
                                 list_lex = list_lex-> next; // on va passer le NL ou le COMMENT (on sera dans le case(NL) )L
                                 S = START;
                                 break;
                            }
                            // else
                            printf("ERREUR LIGNE : %d\n", line);
                            ERROR_MSG("invalide element apres un .bss !\n");
                        }

                        if (strcmp(val_lexem, ".text") == 0){
                            section = TEXT;
                            pcurrent_list = &list_instr;
                            pdecalage = &dec_txt;
                            if ( ( ( (LEXEM)(list_lex->next->element))->lex_type == NL) || ( ( (LEXEM)(list_lex->next->element))->lex_type == COMMENT) ) {
                                 list_lex = list_lex-> next; // on va passer le NL ou le COMMENT (on sera dans le case(NL) )
                                 S = START;
                                 break;
                            }
                            // else
                            printf("ERREUR LIGNE : %d\n", line);
                            ERROR_MSG("invalide element apres un .text !\n");
                        }

                        // ----cas du .set ---

                        if (strcmp(val_lexem, ".set") == 0){
                            //if ( ( ((LEXEM)((LIST)(list_lex->next))->element)->lex_type) == NL) {
                            // if ( ( (LEXEM)(list_lex->next->element))->lex_type == NL) {
                            //      list_lex = list_lex-> next; // on va passer l'argument
                            //      printf("AAAAAAA\n\n\n\n\n\n\n\n");
                            // }
                            S = START;
                            list_lex = list_lex->next; // on saute l'argument qui suit
                            // TODO si y'en a pas ? erreur ?
                            break;
                        }
                        /* --- fin gestion directives de section --- */


                        // ---- autres directives ----
                        if (section == PDATA ) {
                            if      (strcmp(val_lexem, ".word")   == 0) S = PWORD;
                            else if (strcmp(val_lexem, ".byte")   == 0) S = PBYTE;
                            else if (strcmp(val_lexem, ".asciiz") == 0) S = PASCIIZ;
                            else if (strcmp(val_lexem, ".space")  == 0) S = PSPACE;   // il peut aussi y avoir des .space dans DATA
                            else if (strcmp(val_lexem, ".set")    == 0) {
                                S = START;
                                list_lex = list_lex->next; // on saute l'argument qui suit
                                // TODO si y'en a pas ? erreur ?
                            }
                            else {
                                printf("ERREUR LIGNE : %d\n", line);
                                ERROR_MSG("Directive inconnue ou dans mauvaise section !\n"); // est-ce possible ???  oui par exemple .space dans section DATA
                            }
                            break;
                        }

                        if (section == BSS ){
                            if      (strcmp(val_lexem, ".space") == 0) S = PSPACE;
                            else if (strcmp(val_lexem, ".set")   == 0) {  // peut il y avoir .set ici ??
                                S = START;
                                list_lex = list_lex->next; // on saute l'argument qui suit
                                // TODO si y'en a pas ? erreur ?
                                break;
                            }
                            else {
                                printf("ERREUR LIGNE : %d\n", line);
                                ERROR_MSG("Directive inconnue ou dans mauvaise section !\n");
                            }
                            break;
                        }

                        if (section == TEXT ){
                            // WARNING NOTE WARNING TODO TODO TODO
                            // WARNING NOTE WARNING TODO TODO TODO
                            // ces directives sont interdites dans .text non ???
                            // WARNING NOTE WARNING TODO TODO TODO
                            // WARNING NOTE WARNING TODO TODO TODO
                            if      (strcmp(val_lexem, ".word")   == 0) S = PWORD;
                            else if (strcmp(val_lexem, ".byte")   == 0) S = PBYTE;
                            else if (strcmp(val_lexem, ".asciiz") == 0) S = PASCIIZ;
                            else if (strcmp(val_lexem, ".space")  == 0) S = PSPACE;
                            else if (strcmp(val_lexem, ".set")    == 0) {
                                S = START;
                                list_lex = list_lex->next; // on saute l'argument qui suit
                                // TODO si y'en a pas ? erreur ?
                                break;
                            }
                            else {
                                printf("ERREUR LIGNE : %d\n", line);
                                ERROR_MSG("Directive inconnue ou dans mauvaise section !\n");
                            }
                            break;
                        }

                    default :
                        printf("ERREUR LIGNE : %d\n", line);
                        //ERROR_MSG("Element non acceptable en debut de ligne !\n");
                        ERROR_MSG("Element non acceptable en debut de ligne !\n");
                        // TODO afficher le lexeme qui cree l'erreur
                }
                break;
            /* ------------ FIN CASE S = START ----------------- */

            case INSTRUCTION:
                if ((type_lexem == VIRGULE) || (type_lexem == DEUX_PTS) || (type_lexem == DIRECTIVE) || (type_lexem == STRING)){
                    printf("ERREUR LIGNE : %d\n", line);
                    ERROR_MSG("Element non acceptable apres une instruction !\n");
                }

                if (type_lexem == MOINS){
                    // TODO TODO TODO  ??
                    // TODO TODO TODO  ??
                    // TODO TODO TODO  ??
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
                    if ( ( ( (LEXEM)(list_lex->next->element))->lex_type == NL) || ( ( (LEXEM)(list_lex->next->element))->lex_type == COMMENT) || ( ( (LEXEM)(list_lex->next->element))->lex_type == VIRGULE) ) {

                        nb_arg_ligne = nb_arg_ligne + 1;

                        if (type_lexem == SYMBOLE){ // cas etiquette
                            if (look_for_etiq(symb_table, val_lexem) == 1){         // si cette etiq est deja définie
                                list_instr = fill_arguments(lexem, list_instr, previous_type_lexem, 1);
                            }
                            else  list_instr = fill_arguments(lexem, list_instr, previous_type_lexem, 0);  // si cette etiq n'est PAS deja définie

                            break;
                        }

                        if ( ( (LEXEM)(list_lex->next->element))->lex_type == VIRGULE) {
                            list_instr = fill_arguments(lexem, list_instr, previous_type_lexem, -1);
                            S = INSTRUCTION;
                            list_lex = list_lex->next; // saute VIRGULE
                            break;
                        }
                        if ( ( ( (LEXEM)(list_lex->next->element))->lex_type == NL) || ( ( (LEXEM)(list_lex->next->element))->lex_type == COMMENT) ) {
                            if (nb_arg_ligne != nb_arg_needed)
                            {
                                printf("ERREUR LIGNE : %d\n", line);
                                ERROR_MSG("Erreur, mauvais nombre d'arguments pour instruction !\n");
                            }
                            list_instr = fill_arguments(lexem, list_instr, previous_type_lexem, -1);
                            S = START;
                            nb_arg_ligne = 0;
                            nb_arg_needed= 0;
                            list_lex = list_lex->next; // saute NL
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
                    if ( ( ( (LEXEM)(list_lex->next)->element)->lex_type) == VIRGULE ) {
                        list_lex = list_lex->next; // on saute la virgule qui suit
                        break;
                    }
                    if ( ((((LEXEM)(list_lex->next)->element)->lex_type) == NL) || ((((LEXEM)(list_lex->next)->element)->lex_type) == COMMENT) ) {
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
                    // TODO TODO TODO TODO TODO TODO
                    // TODO TODO TODO TODO TODO TODO
                    // considère le cas previous_type_lexem = MOINS
                    // Stockage : *pcurrent_list = add_to_current_list(PWORD, "valeur", decalage, ligne, *pcurrent_list);

                    if ( (( (LEXEM)(list_lex->next)->element) ->lex_type) == VIRGULE ) {
                        list_lex = list_lex->next; // on saute la virgule qui suit
                        break;
                    }
                    if ((( ((LEXEM)(list_lex->next)->element)->lex_type) == NL) || (( ((LEXEM)(list_lex->next)->element)->lex_type) == COMMENT) ) {
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
                        // TODO TODO TODO TODO TODO TODO
                        // TODO TODO TODO TODO TODO TODO
                        *pcurrent_list = add_to_current_list(PASCIIZ, val_lexem, *pdecalage, line, *pcurrent_list);
                        if ( (( (LEXEM)(list_lex->next)->element) ->lex_type) == VIRGULE ) {
                            list_lex = list_lex->next; // on saute la virgule qui suit
                            break;
                        }
                        if ((( ((LEXEM)(list_lex->next)->element)->lex_type) == NL)|| (( ((LEXEM)(list_lex->next)->element)->lex_type) == COMMENT) ) {
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
/*
                    // WARNING WARNING TODO TODO NOTE
                    // WARNING WARNING TODO TODO NOTE
                    // .asciiz valeur   =>doit commencer et terminer par un "
                    // faire erreur
                    // WARNING WARNING TODO TODO NOTE
                    // WARNING WARNING TODO TODO NOTE
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
                    }*/

                    else {
                        printf("ERREUR LIGNE : %d\n", line);
                        ERROR_MSG("Element non acceptable apres un .asciiz !\n");
                    }

            case PSPACE :

                // Seuls les entiers non signés sont acceptés
                if (previous_type_lexem == MOINS)
                {
                    printf("ERREUR LIGNE : %d\n", line);
                    ERROR_MSG("Seuls entiers non signés acceptés après .space !\n");
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
                    ERROR_MSG("Element non acceptable apres un .space !\n");
                }


                // autres cas : faut il tout convertir aussi et les accepter ???
                // TODO TODO
                // TODO TODO
                // TODO TODO

            case PBYTE:
                    break; // TODO TODO a changer
                    // TODO TODO
                    // TODO TODO
                    // TODO TODO

            // autres cas ?



        } // fin switch

        previous_type_lexem = type_lexem; // on garde en mémoire pour le cas du MOINS
        list_lex = list_lex->next;

    } // fin while

    print_list_instr(list_instr);
    print_symb_table(symb_table);
    print_list_data(list_data);

    // --- deuxième parcours : on cherche les étiquettes ----

    // list_instr
    look_for_undefined_etiq_in_instr(list_instr, symb_table);

    // list_data
    look_for_undefined_etiq_in_data(list_data, symb_table);

    // list_bss
    look_for_undefined_etiq_in_data(list_bss, symb_table);



    return;
}
