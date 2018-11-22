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
    int S = START;

    long int val_convert = 0;

    int type_arg_expected_1;
    int type_arg_expected_2;
    int type_arg_expected_3;

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
                        if (section==NONE) {  // sinon risque de pointeur vers NULL ( pdecalage / pcurrent_list )
                            printf("ERREUR LIGNE : %d\n", line);
                            ERROR_MSG("Erreur, aucune section définie !\n");
                        }
                        // cas 1 : une déclaration d'étiquette ? (on commence par ce cas car certaines etiquettes ont le même nom que des instructions)
                        if ( ( (LEXEM)(list_lex->next->element))->lex_type == DEUX_PTS) {
                            if (look_for_etiq(symb_table, val_lexem)) {
                                printf("ERREUR LIGNE : %d\n", line);
                                ERROR_MSG("Redefinition d'etiquette !\n");
                                // TODO msg d'erreur donnant les lignes de définition et rédéfinition
                            }
                            symb_table = add_to_symb_table(val_lexem, *pdecalage, line, section, symb_table);
                            list_lex = list_lex->next; // on va passer les DEUX_PTS
                            break;
                        }

                        // cas 2 : une instruction ?
                        if ( look_for_inst(val_lexem, dictionnaire, &nb_arg_needed, &type_arg_expected_1, &type_arg_expected_2, &type_arg_expected_3) )   // le else ne sert a rien normalement
                        {
                            if (section != TEXT) // pas d'instruction possible hors de la section .Text
                                {
                                    printf("ERREUR LIGNE : %d\n", line);
                                    ERROR_MSG("Erreur, instruction dans la mauvaise section !\n");
                                    // TODO msg erreur : donner la section courante
                                }
                            if (*pdecalage%4) {  // Les instructions sont toujours codées sur des adresses alignées sur un mot
                                printf("ERREUR LIGNE : %d\n", line);
                                ERROR_MSG("Erreur, problème d'alignement d'addressage en mémoire de l'instruction !\n");
                            }
                            else list_instr =  add_to_list_instr(lexem, *pdecalage, nb_arg_needed, list_instr, type_arg_expected_1, type_arg_expected_2, type_arg_expected_3);
                            *pdecalage += 4;
                            S = INSTRUCTION;
                            break;
                        }
                        // cas 3 : autres
                        else { // c'est une erreur car une ligne ne peut pas commencer par un appel d'etiquette !
                            printf("ERREUR LIGNE : %d\n", line);
                            ERROR_MSG("Erreur, symbole non valide en debut de ligne ! (debut après étiquette)\n");
                        }

                    case COMMENT: //on ne considère pas les commentaires, on ne les stocke pas
                        if ( ( (LEXEM)(list_lex->next->element))->lex_type == NL) {
                            list_lex = list_lex->next; // on saute le NL qui suit
                            break;
                        }
                        // else  // est-ce possible ??? je crois pas, mais bon ...
                        printf("ERREUR LIGNE : %d\n", line);
                        ERROR_MSG("Erreur, après le commentaire !\n");


                    case NL:
                        //ON Y PASSE AVEC .set => on peut généraliser utilisation => NON !!!
                        //printf("AAAAAAA  %s\n\n\n\n\n\n\n\n",  ( (LEXEM)(list_lex->next->element))->value);
                        break;

                    case DIRECTIVE:
                        /* ---- modification de la section et de la liste courante ---- */
                        if (strcmp(val_lexem, ".data") == 0){
                            section = PDATA;
                            pcurrent_list = &list_data;
                            pdecalage = &dec_data;
                            if ( ( ( (LEXEM)(list_lex->next->element))->lex_type == NL) || ( ( (LEXEM)(list_lex->next->element))->lex_type == COMMENT) ) {
                                 list_lex = list_lex-> next; // on va passer le NL ou le COMMENT (on sera dans le case(NL) )
                                 S = START;
                                 break;
                            }
                            // else
                            printf("ERREUR LIGNE : %d\n", line);
                            ERROR_MSG("invalide element apres un .data !\n");
                        }

                        else if (strcmp(val_lexem, ".bss") == 0){
                            section = BSS;
                            pcurrent_list = &list_bss;
                            pdecalage = &dec_bss;
                            if ( ( ( (LEXEM)(list_lex->next->element))->lex_type == NL) || ( ( (LEXEM)(list_lex->next->element))->lex_type == COMMENT) ) {
                                 list_lex = list_lex-> next; // on va passer le NL ou le COMMENT (on sera dans le case(NL) )L
                                 S = START;
                                 break;
                            }
                            // else
                            printf("ERREUR LIGNE : %d\n", line);
                            ERROR_MSG("invalide element apres un .bss !\n");
                        }

                        else if (strcmp(val_lexem, ".text") == 0){
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
                        /* --- fin gestion directives de section --- */

                        // ---- cas du .set ----
                        else if (strcmp(val_lexem, ".set") == 0){
                            // if ( ( (LEXEM)(list_lex->next->element))->lex_type == NL) {
                            //      list_lex = list_lex-> next; // on va passer l'argument
                            //      printf("AAAAAAA\n\n\n\n\n\n\n\n");
                            // }
                            S = START;
                            list_lex = list_lex->next; // on saute l'argument qui suit => case NL
                                                       // OU le NL si oublie de l'argument par le codeur
                            // TODO si y'en a pas ? erreur ?
                            // TODO erreur si pas d'argument
                            if (section != NONE) {
                                printf("ERREUR LIGNE : %d\n", line);
                                ERROR_MSG(".set doit etre au debut du programme (avant la 1e section) \n");
                            }
                            break;
                        }



                        /* ---- autres directives ---- */
                        if (section == PDATA ) {
                            if      (strcmp(val_lexem, ".word")   == 0) S = PWORD;
                            else if (strcmp(val_lexem, ".byte")   == 0) S = PBYTE;
                            else if (strcmp(val_lexem, ".asciiz") == 0) S = PASCIIZ;
                            else if (strcmp(val_lexem, ".space")  == 0) S = PSPACE;   // il peut aussi y avoir des .space dans DATA
                            else {
                                printf("ERREUR LIGNE : %d\n", line);
                                ERROR_MSG("Directive inconnue ou dans mauvaise section !\n"); // est-ce possible ???  oui par exemple .space dans section DATA
                            }
                            break;
                        }

                        if (section == BSS ){
                            if      (strcmp(val_lexem, ".space") == 0) S = PSPACE;
                            else {
                                printf("ERREUR LIGNE : %d\n", line);
                                ERROR_MSG("Directive inconnue ou dans mauvaise section !\n");
                            }
                            break;
                        }

                        if (section == TEXT ){
                            // Les directives (autres que celles de section) sont interdites dans la section TEXT
                            printf("ERREUR LIGNE : %d\n", line);
                            ERROR_MSG("Directive interdites dans la section TEXT !\n");
                            break; //
                        }

                    default :
                        printf("ERREUR LIGNE : %d\n", line);
                        //ERROR_MSG("Element non acceptable en debut de ligne !\n");
                        ERROR_MSG("Element non acceptable en debut de ligne ! (debut après étiquette)\n");
                        // TODO afficher le lexeme qui cree l'erreur
                } // fin switch(type_lexem)
                break;
            /* ------------ FIN CASE S = START ----------------- */

            case INSTRUCTION:
                if ((type_lexem == VIRGULE) || (type_lexem == DEUX_PTS) || (type_lexem == DIRECTIVE) || (type_lexem == STRING)){
                    printf("ERREUR LIGNE : %d\n", line);
                    ERROR_MSG("Element non acceptable apres cette instruction !\n");
                }

                if (type_lexem == MOINS) break;

                else {
                    if (((type_lexem == NL)||(type_lexem == COMMENT)) ) { // plus d'argument apres l'instruction ou apres la virgule
                        if ((nb_arg_ligne == nb_arg_needed) && (previous_type_lexem != VIRGULE) && (previous_type_lexem != MOINS)) // cas où 0 arg
                        {
                            if (type_lexem == COMMENT){
                                list_lex = list_lex->next; // on saute le NL qui suit
                            }

                            // ici si l'on a bien un NOP alors il faut la remplacer (pseudo_instruction): peut-on avoir autre chose qu'un NOP ??

                            char* val_instr = strdup( ((LEXEM)(((INSTR)(list_instr->element))->lex)) -> value);

                            if ( strcmp( val_instr, "NOP") ==  0){ //ICI ERREUR DE SEG
                                list_instr = change_pseudo_instr(list_instr);
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
                        if (previous_type_lexem == MOINS) {
                            printf("ERREUR LIGNE : %d\n", line);
                            ERROR_MSG("Manque chiffre apres signe moins !\n");
                        }

                    }
                    if ( ( ( (LEXEM)(list_lex->next->element))->lex_type == NL) || ( ( (LEXEM)(list_lex->next->element))->lex_type == COMMENT) || ( ( (LEXEM)(list_lex->next->element))->lex_type == VIRGULE) ) {

                        nb_arg_ligne = nb_arg_ligne + 1;


                        if ( ( (LEXEM)(list_lex->next->element))->lex_type == VIRGULE) {
                            if (type_lexem == SYMBOLE){ // cas etiquette
                                if (look_for_etiq(symb_table, val_lexem) == 1){         // si cette etiq est deja définie
                                    list_instr = fill_arguments(lexem, list_instr, previous_type_lexem, 1, nb_arg_ligne);
                                }
                                else  list_instr = fill_arguments(lexem, list_instr, previous_type_lexem, 0, nb_arg_ligne);  // si cette etiq n'est PAS deja définie
                            }

                            else list_instr = fill_arguments(lexem, list_instr, previous_type_lexem, -1, nb_arg_ligne); //renvoit -1 car pas une étiquette
                            S = INSTRUCTION;

                            if ( ( ( (LEXEM)(list_lex->next->next->element))->lex_type != NL) && ( ( (LEXEM)(list_lex->next->next->element))->lex_type != COMMENT) ) { // pour éviter le cas intr arg1, arg2, NL
                                list_lex = list_lex->next; // saute VIRGULE
                            }
                            break;
                        }
                        
                        if ( ( ( (LEXEM)(list_lex->next->element))->lex_type == NL) || ( ( (LEXEM)(list_lex->next->element))->lex_type == COMMENT) ) {
                            if (nb_arg_ligne != nb_arg_needed)
                            {
                                printf("ERREUR LIGNE : %d\n", line);
                                ERROR_MSG("Erreur, mauvais nombre d'arguments pour instruction !\n");
                            }
                            if (type_lexem == SYMBOLE){ // cas etiquette
                                if (look_for_etiq(symb_table, val_lexem) == 1){         // si cette etiq est deja définie
                                    list_instr = fill_arguments(lexem, list_instr, previous_type_lexem, 1, nb_arg_ligne);
                                }
                                else  list_instr = fill_arguments(lexem, list_instr, previous_type_lexem, 0, nb_arg_ligne);  // si cette etiq n'est PAS deja définie
                            }

                            else list_instr = fill_arguments(lexem, list_instr, previous_type_lexem, -1, nb_arg_ligne);

                            // ici on doit vérifier que l'on a pas une pseudo_instruction !
                            list_instr = change_pseudo_instr(list_instr);


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
                if (type_lexem == MOINS) break;

                if (type_lexem == SYMBOLE) { // recherche étiquette dans symb_table
                    // alignement en mémoire du mot
                    *pdecalage = *pdecalage + 3 - ((*pdecalage-1+4)%4);  // +4 car pour gérer le cas dec=0  (en c : -1%4 = -1)
                    if (*pdecalage%4) {  // normalement jamais le cas si ligne prec OK
                        printf("ERREUR LIGNE : %d\n", line);
                        ERROR_MSG("Erreur, problème d'alignement d'addressage en mémoire du mot !\n");
                    }
                    *pcurrent_list = add_to_current_list(LABEL, val_lexem, *pdecalage, line, *pcurrent_list);// stocke. Par defaut etiq_def est à 0;
                    *pdecalage += 4;

                    if (look_for_etiq(symb_table, val_lexem) == 1){ // si etiq deja définie
                         ((DATA)((*pcurrent_list)->element))->etiq_def = 1;
                    }
                    if ( ( ( (LEXEM)(list_lex->next)->element)->lex_type) == VIRGULE ) {
                        list_lex = list_lex->next; // on saute la virgule qui suit
                        break;
                    }
                    if ( ((((LEXEM)(list_lex->next)->element)->lex_type) == NL) || ((((LEXEM)(list_lex->next)->element)->lex_type) == COMMENT) ) {
                        list_lex = list_lex->next; // on saute le NL qui suit
                                                   // OU le COMMENT => case NL
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

                if ((type_lexem == HEXA) || (type_lexem == OCTAL) || (type_lexem == DECIMAL) ){
                    // alignement en mémoire du mot
                    *pdecalage = *pdecalage + 3 - ((*pdecalage-1+4)%4);  // +4 car pour gérer le cas dec=0  (en c : -1%4 = -1)
                    if (*pdecalage%4) {
                        printf("ERREUR LIGNE : %d\n", line);
                        ERROR_MSG("Erreur, problème d'alignement d'addressage en mémoire du mot !\n");
                    }
                    // conversion de la string en nombre
                    val_convert = strtol(val_lexem, NULL, 0);
                    if (previous_type_lexem == MOINS) val_convert = -val_convert;
                    // TODO TODO TODO TODO
                    // TODO TODO TODO TODO
                    printf("\nAAAAAAAAAAAAAAAAAA %ld\n",val_convert) ;
                    if ( (val_convert<-2147483648) || (2147483647<val_convert) ) {
                        printf("ERREUR LIGNE : %d\n", line);
                        ERROR_MSG("Nombre trop grand pour etre stocké dans un word (32 bits : -2147483648 < x < 2147483647)\n");
                    }
                    *pcurrent_list = add_to_current_list(PWORD, &val_convert, *pdecalage, line, *pcurrent_list);
                    *pdecalage += 4;

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
                if (type_lexem == NL) {
                    printf("ERREUR LIGNE : %d\n", line);
                    ERROR_MSG("Element manquant apres un .word !\n");
                }
                printf("ERREUR LIGNE : %d\n", line);
                ERROR_MSG("Element non acceptable apres un .word !\n");


            case PASCIIZ:
                if (type_lexem == STRING) {
                    // /!\ le lexeme vaut ""chaine de char""   => donc il faut enlever des guillements aux extrémités
                    char* str = strdup(val_lexem);
                    str[strlen(str)-1] = '\0';   // on envèle le dernier guillement
                    *pcurrent_list = add_to_current_list(PASCIIZ, str+1, *pdecalage, line, *pcurrent_list);   // str+1 pour enlever le 1er guillement
                    *pdecalage += strlen(str+1)+1;   //+1 pour le \0
                    free(str);

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
                else {
                    printf("ERREUR LIGNE : %d\n", line);
                    ERROR_MSG("Element non acceptable apres un .asciiz !\n");
                }
                break;

            case PSPACE :
                // Seuls les entiers non signés sont acceptés
                if (type_lexem == MOINS)
                {
                    printf("ERREUR LIGNE : %d\n", line);
                    ERROR_MSG("Seuls entiers non signés acceptés après .space !\n");
                }
                if ((type_lexem == HEXA) || (type_lexem == OCTAL) || (type_lexem == DECIMAL)) {
                    // conversion de la string en nombre
                    val_convert = (int) strtol(val_lexem, NULL, 0);
                    *pcurrent_list = add_to_current_list(PSPACE, &val_convert, *pdecalage, line, *pcurrent_list);
                    *pdecalage += val_convert;

                    // Il peut y avoir qu'un argument avec les .space
                    list_lex = list_lex->next; // on saute la NL qui suit
                    S = START;

                    break;
                }
                else {
                    printf("ERREUR LIGNE : %d\n", line);
                    ERROR_MSG("Element non acceptable apres un .space !\n");
                }
                break;

            case PBYTE:
                if (type_lexem == MOINS) break;

                if ((type_lexem == HEXA) || (type_lexem == OCTAL) || (type_lexem == DECIMAL)) {
                    // conversion de la string en nombre
                    val_convert = strtol(val_lexem, NULL, 0);
                    if (previous_type_lexem == MOINS) val_convert = -val_convert;
                    if ( (val_convert<-128) || (127<val_convert) ) {
                        printf("ERREUR LIGNE : %d\n", line);
                        ERROR_MSG("Nombre trop grand pour etre stocké dans un byte (32 bits : -128 < x < 127)\n");
                    }
                    *pcurrent_list = add_to_current_list(PSPACE, &val_convert, *pdecalage, line, *pcurrent_list);
                    (*pdecalage)++;
                }

                else {
                    printf("ERREUR LIGNE : %d\n", line);
                    ERROR_MSG("Element non acceptable apres un .byte !\n");
                }
                if ( ( ( (LEXEM)(list_lex->next)->element)->lex_type) == VIRGULE ) {
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
                break;
        } // fin switch
        previous_type_lexem = type_lexem; // on garde en mémoire pour le cas du MOINS
        list_lex = list_lex->next;

    } // fin while


    // --- deuxième parcours : on cherche les étiquettes ----
    // list_instr
    look_for_undefined_etiq_in_instr(list_instr, symb_table);
    // list_data
    look_for_undefined_etiq_in_data(list_data, symb_table);
    // list_bss
    look_for_undefined_etiq_in_data(list_bss, symb_table);

    print_list_instr(list_instr);
    print_symb_table(symb_table);
    print_list_data(list_data);
    // TODO
    printf("\n\nci dessous c'est la liste des .bss meme si c'est marqué .data (à corriger)\n");
    print_list_data(list_bss);  // faire une fct différente ou envoyer en paramètre la section


    return;
}