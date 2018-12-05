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


void analyse_synth(LIST* p_list_instr, LIST* p_list_data, LIST* p_list_bss, LIST* p_symb_table, LIST list_lex, LIST dictionnaire)
{
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
        lexem      = list_lex->element;
        type_lexem = lexem->lex_type;
        val_lexem  = lexem->value;
        line       = lexem->nline;


        switch (S)
        {
            case START: // cas lexeme de début de ligne ou après étiquette ou après dernier arg d'instruction
            /* ------------ CASE S = START ----------------- */
                switch(type_lexem) {
                    case SYMBOLE:
                        // test si section=NONE sinon risque de pointeur vers NULL ( pdecalage / pcurrent_list )
                        if (section==NONE) ERROR_MSG("ERR LINE %d : aucune section définie !\n", line);
                        // CAS 1 : une déclaration d'étiquette ? (on commence par ce cas car certaines etiquettes ont le même nom que des instructions)
                        else if ( ( (LEXEM)(list_lex->next->element))->lex_type == DEUX_PTS) {
                            if (look_for_etiq(*p_symb_table, val_lexem)) {
                                ERROR_MSG("ERR LINE %d : Redefinition d'etiquette !\n", line);
                            }

                            // si la prochaine directive est un .word, il faut aligner l'adresse de l'etiquette
                            LIST test_word = list_lex;
                            while (     ( ((LEXEM)test_word->next->element)->lex_type==DEUX_PTS )
                                   || ( ( ((LEXEM)test_word->next->element)->lex_type==SYMBOLE ) && ( ((LEXEM)test_word->next->next->element)->lex_type==DEUX_PTS ) )  // pour exclure instruction MEME SI ON EST DANS DATA et qu'a priori c'est pas possible
                                   ||   ( ((LEXEM)test_word->next->element)->lex_type==NL )
                                   ||   ( ((LEXEM)test_word->next->element)->lex_type==COMMENT ) )
                            {
                                test_word = test_word->next;
                                if ( strcmp( ((LEXEM)test_word->next->element)->value , ".word" ) ) {
                                    // alignement en mémoire du mot
                                    *pdecalage = *pdecalage + 3 - ((*pdecalage-1+4)%4);  // +4 car pour gérer le cas dec=0  (en c : -1%4 = -1)
                                    break; // marche sans mais bon
                                }
                            }

                            *p_symb_table = add_to_symb_table(val_lexem, *pdecalage, line, section, TRUE, *p_symb_table);
                            list_lex = list_lex->next; // on va passer les DEUX_PTS
                        }
                        // CAS 2 : une instruction ?
                        else if ( look_for_inst(val_lexem, dictionnaire, &nb_arg_needed, &type_arg_expected_1, &type_arg_expected_2, &type_arg_expected_3) ) {
                            // pas d'instruction possible hors de la section .text
                            if (section != TEXT) ERROR_MSG("ERR LINE %d : instruction dans la mauvaise section !\n", line);

                            // Les instructions sont toujours codées sur des adresses alignées sur un mot
                            if (*pdecalage%4) ERROR_MSG("ERR LINE %d : problème d'alignement d'addressage en mémoire de l'instruction !\n", line);

                            else *p_list_instr =  add_to_list_instr(lexem, *pdecalage, nb_arg_needed, *p_list_instr, type_arg_expected_1, type_arg_expected_2, type_arg_expected_3);
                            *pdecalage += 4;
                            S = INSTRUCTION;
                        }
                        // cas 3 : autres. C'est une erreur car une ligne ne peut pas commencer par un appel d'etiquette !
                        else ERROR_MSG("ERR LINE %d : symbole non valide en debut de ligne ! (debut après étiquette)\n", line);

                        break;

                    case COMMENT: //on ne considère pas les commentaires, on ne les stocke pas
                        break;  // il y a tjrs un NL en fin de ligne   => case NL

                    case NL:
                        break;

                    case DIRECTIVE:
                        /* ---- modification de la section et de la liste courante ---- */
                        if      (strcmp(val_lexem, ".data") == 0) {
                            section = PDATA;
                            pcurrent_list = p_list_data;
                            pdecalage = &dec_data;
                            if ( ( ( (LEXEM)(list_lex->next->element))->lex_type != NL) && ( ( (LEXEM)(list_lex->next->element))->lex_type != COMMENT) ) {
                                ERROR_MSG("ERR LINE %d : invalide element apres un .data !\n", line);
                            }
                        }
                        else if (strcmp(val_lexem, ".bss") == 0) {
                            section = BSS;
                            pcurrent_list = p_list_bss;
                            pdecalage = &dec_bss;
                            if ( ( ( (LEXEM)(list_lex->next->element))->lex_type != NL) && ( ( (LEXEM)(list_lex->next->element))->lex_type != COMMENT) ) {
                                ERROR_MSG("ERR LINE %d : invalide element apres un .bss !\n", line);
                            }
                        }
                        else if (strcmp(val_lexem, ".text") == 0) {
                            section = TEXT;
                            pcurrent_list = p_list_instr;
                            pdecalage = &dec_txt;
                            if ( ( ( (LEXEM)(list_lex->next->element))->lex_type != NL) && ( ( (LEXEM)(list_lex->next->element))->lex_type != COMMENT) ) {
                                ERROR_MSG("ERR LINE %d : invalide element apres un .text !\n", line);
                            }
                        }
                        /* ---- fin gestion directives de section ---- */

                        // ---- cas du .set ----
                        else if (strcmp(val_lexem, ".set") == 0){
                            list_lex = list_lex->next;   //pas de risque de faire ça, meme s'il y a pas l'arg apres le .set il y a au moins un NL
                                                         //et une erreur sera levé donc pas de risque à la fin du switch quand on refait list_lex = list_lex->next
                            if (section != NONE) {
                                ERROR_MSG("ERR LINE %d : .set doit etre au debut du programme (avant la 1e section) \n", line);
                            }
                            if ( strcmp( ((LEXEM)list_lex->element)->value , "noreorder") ) {
                                ERROR_MSG("ERR LINE %d : \"noreorder\" attendu après un .set\n", line);
                            }
                        }

                        /* ---- autres directives ---- */
                        else if (section == PDATA ) {
                            if      (strcmp(val_lexem, ".word")   == 0) S = PWORD;
                            else if (strcmp(val_lexem, ".byte")   == 0) S = PBYTE;
                            else if (strcmp(val_lexem, ".asciiz") == 0) S = PASCIIZ;
                            else if (strcmp(val_lexem, ".space")  == 0) S = PSPACE;   // il peut aussi y avoir des .space dans DATA
                            else    ERROR_MSG("ERR LINE %d : Directive inconnue ou dans mauvaise section !\n", line);
                        }
                        else if (section == BSS ){
                            if      (strcmp(val_lexem, ".space")  == 0) S = PSPACE;
                            else    ERROR_MSG("ERR LINE %d : Directive inconnue ou dans mauvaise section !\n", line);
                        }
                        else if (section == TEXT ){
                            // Les directives (autres que celles de section) sont interdites dans la section TEXT
                            ERROR_MSG("ERR LINE %d : Directive interdites dans la section TEXT !\n", line);
                        }

                        break;

                    default :
                        ERROR_MSG("ERR LINE %d : Element non acceptable en debut de ligne ou après étiquette !\n", line);
                } // fin switch(type_lexem)
                break;
            /* ------------ FIN CASE S = START ----------------- */

            case INSTRUCTION:

                if ( (type_lexem == NL) || (type_lexem == COMMENT) ) {
                    if (previous_type_lexem == MOINS) {
                        ERROR_MSG("ERR LINE %d : Manque chiffre apres signe moins !\n", line);
                    }
                    if (nb_arg_ligne != ((INSTR)((*p_list_instr)->element))->nb_arg ) {  //on ne mettre nb_arg_needed car il peut changer de valeur (avec les base offset)
                        if (previous_type_lexem == VIRGULE) ERROR_MSG("ERR LINE %d : Argument attendu après une virgule\n", line);
                        else                                ERROR_MSG("ERR LINE %d : mauvais nombre d'arguments pour cette instruction !\n", line);
                    }
                    if (previous_type_lexem == VIRGULE) {
                        ERROR_MSG("ERR LINE %d : Virgule de trop à la fin!\n", line);
                    }

                    // cas pseudo instruction NOP (0 opérande)
                    char* val_instr = strdup( ((LEXEM)(((INSTR)((*p_list_instr)->element))->lex)) -> value);
                    if ( strcmp( val_instr, "NOP") ==  0){
                        *p_list_instr = change_pseudo_instr(*p_list_instr, pdecalage);
                    }

                    S = START;
                    nb_arg_ligne = 0;
                    nb_arg_needed= 0;
                    break;
                }

                if (type_lexem == MOINS) break;

                if ( (type_lexem == REGISTRE) || (type_lexem == SYMBOLE) || (type_lexem == HEXA) || (type_lexem == OCTAL) || (type_lexem == DECIMAL) || (type_lexem == AIBD) ) {
                    nb_arg_ligne = nb_arg_ligne + 1;

                    if (type_lexem == SYMBOLE) { // cas etiquette
                        if (look_for_etiq(*p_symb_table, val_lexem) == 1) {         // si cette etiq est deja définie
                            *p_list_instr = fill_arguments(lexem, *p_list_instr, previous_type_lexem, 1, &nb_arg_ligne);
                        }
                        else  *p_list_instr = fill_arguments(lexem, *p_list_instr, previous_type_lexem, 0, &nb_arg_ligne);  // si cette etiq n'est PAS deja définie
                    }
                    else *p_list_instr = fill_arguments(lexem, *p_list_instr, previous_type_lexem, -1, &nb_arg_ligne);



                    // ici on doit vérifier que l'on a pas une pseudo_instruction !
                    *p_list_instr = change_pseudo_instr(*p_list_instr, pdecalage);
                    // A ce moment, le decalage été déjà incrémenté => il faudra l'incrementer qu'APRES insertion de la 2e instruction
                    *p_list_instr = change_pseudo_SW_LW(*p_list_instr, pdecalage);


                    if      ( ( (LEXEM)(list_lex->next->element))->lex_type == VIRGULE) {
                        list_lex = list_lex->next;   // on saute la virgule
                        lexem = list_lex->element;   // pour que previous_type_lexem enregistre la virgule
                        type_lexem = lexem->lex_type;
                    }
                    else if ( ( ( (LEXEM)(list_lex->next->element))->lex_type == NL) || ( ( (LEXEM)(list_lex->next->element))->lex_type == COMMENT) );  // ne rien faire
                    else    ERROR_MSG("ERR LINE %d : Element non acceptable apres une opérande !\n", line);
                }
                else ERROR_MSG("ERR LINE %d : Opérande non valide !\n", line);
                break;

                ///////////////



                // if ((type_lexem == VIRGULE) || (type_lexem == DEUX_PTS) || (type_lexem == DIRECTIVE) || (type_lexem == STRING)) {
                //     ERROR_MSG("ERR LINE %d : Element non acceptable apres cette instruction !\n", line);
                // }
                //
                // if (type_lexem == MOINS) break;
                //
                //
                // if (((type_lexem == NL)||(type_lexem == COMMENT)) ) { // plus d'argument apres l'instruction ou apres la virgule
                //     if ((nb_arg_ligne == nb_arg_needed) && (previous_type_lexem != VIRGULE) && (previous_type_lexem != MOINS)) // cas où 0 arg
                //     {
                //         // ici si l'on a bien un NOP alors il faut la remplacer (pseudo_instruction): peut-on avoir autre chose qu'un NOP ??
                //
                //         char* val_instr = strdup( ((LEXEM)(((INSTR)(*p_list_instr->element))->lex)) -> value);
                //
                //         if ( strcmp( val_instr, "NOP") ==  0){
                //             *p_list_instr = change_pseudo_instr(*p_list_instr);
                //         }
                //
                //         *p_list_instr = change_pseudo_SW_LW(*p_list_instr);   // inutile ? que instruction
                //         S = START;
                //         nb_arg_ligne = 0;
                //         nb_arg_needed= 0;
                //         break;
                //     }
                //     if ((nb_arg_ligne == nb_arg_needed) && (previous_type_lexem == VIRGULE)) {
                //         ERROR_MSG("ERR LINE %d : Virgule de trop à la fin!\n", line);
                //     }
                //     if (previous_type_lexem == VIRGULE) {
                //         ERROR_MSG("ERR LINE %d : Argument attendu après une virgule\n", line);
                //     }
                //     if (previous_type_lexem == MOINS) {
                //         ERROR_MSG("ERR LINE %d : Manque chiffre apres signe moins !\n", line);
                //     }
                //
                // }
                // if ( ( ( (LEXEM)(list_lex->next->element))->lex_type == NL) || ( ( (LEXEM)(list_lex->next->element))->lex_type == COMMENT) || ( ( (LEXEM)(list_lex->next->element))->lex_type == VIRGULE) ) {
                //
                //     nb_arg_ligne = nb_arg_ligne + 1;
                //
                //
                //     if ( ( (LEXEM)(list_lex->next->element))->lex_type == VIRGULE) {
                //         if (type_lexem == SYMBOLE){ // cas etiquette
                //             if (look_for_etiq(*p_symb_table, val_lexem) == 1){         // si cette etiq est deja définie
                //                 *p_list_instr = fill_arguments(lexem, *p_list_instr, previous_type_lexem, 1, &nb_arg_ligne);
                //             }
                //             else  *p_list_instr = fill_arguments(lexem, *p_list_instr, previous_type_lexem, 0, &nb_arg_ligne);  // si cette etiq n'est PAS deja définie
                //         }
                //
                //         else *p_list_instr = fill_arguments(lexem, *p_list_instr, previous_type_lexem, -1, &nb_arg_ligne); //renvoit -1 car pas une étiquette
                //         S = INSTRUCTION;
                //
                //         if ( ( ( (LEXEM)(list_lex->next->next->element))->lex_type != NL) && ( ( (LEXEM)(list_lex->next->next->element))->lex_type != COMMENT) ) { // pour éviter le cas intr arg1, arg2, NL
                //             list_lex = list_lex->next; // saute VIRGULE
                //         }
                //         break;
                //     }
                //
                //     if ( ( ( (LEXEM)(list_lex->next->element))->lex_type == NL) || ( ( (LEXEM)(list_lex->next->element))->lex_type == COMMENT) ) {
                //         if (nb_arg_ligne != nb_arg_needed) {
                //             ERROR_MSG("ERR LINE %d : mauvais nombre d'arguments pour instruction !\n", line);
                //         }
                //         if (type_lexem == SYMBOLE) { // cas etiquette
                //             if (look_for_etiq(*p_symb_table, val_lexem) == 1){         // si cette etiq est deja définie
                //                 *p_list_instr = fill_arguments(lexem, *p_list_instr, previous_type_lexem, 1, &nb_arg_ligne);
                //             }
                //             else  *p_list_instr = fill_arguments(lexem, *p_list_instr, previous_type_lexem, 0, &nb_arg_ligne);  // si cette etiq n'est PAS deja définie
                //         }
                //
                //         else *p_list_instr = fill_arguments(lexem, *p_list_instr, previous_type_lexem, -1, &nb_arg_ligne);
                //
                //         // ici on doit vérifier que l'on a pas une pseudo_instruction !
                //         *p_list_instr = change_pseudo_instr(*p_list_instr);
                //         *p_list_instr = change_pseudo_SW_LW(*p_list_instr);
                //
                //         S = START;
                //         nb_arg_ligne = 0;
                //         nb_arg_needed= 0;
                //         list_lex = list_lex->next; // saute NL
                //         break;
                //     }
                //
                // }
                // else ERROR_MSG("ERR LINE %d : Virgule manquante apres un argument !\n", line);
                //

                //ERROR_MSG("ERR LINE %d : Element non acceptable apres cette instruction !\n", line);

            case PWORD:
                if (type_lexem == MOINS) break;

                if (type_lexem == SYMBOLE) { // recherche étiquette dans symb_table
                    // alignement en mémoire du mot
                    *pdecalage = *pdecalage + 3 - ((*pdecalage-1+4)%4);  // +4 car pour gérer le cas dec=0  (en c : -1%4 = -1)

                    // normalement jamais le cas si ligne prec OK
                    if (*pdecalage%4) ERROR_MSG("ERR LINE %d : problème d'alignement d'addressage en mémoire du mot !\n", line);

                    *pcurrent_list = add_to_current_list(LABEL, val_lexem, *pdecalage, line, *pcurrent_list);// stocke. Par defaut etiq_def est à 0;
                    *pdecalage += 4;

                    if (look_for_etiq(*p_symb_table, val_lexem) == 1){ // si etiq deja définie
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
                    else ERROR_MSG("ERR LINE %d : Element invalide apres un argument !\n", line);
                }

                if ((type_lexem == HEXA) || (type_lexem == OCTAL) || (type_lexem == DECIMAL) ){
                    // alignement en mémoire du mot
                    *pdecalage = *pdecalage + 3 - ((*pdecalage-1+4)%4);  // +4 car pour gérer le cas dec=0  (en c : -1%4 = -1)
                    if (*pdecalage%4) ERROR_MSG("ERR LINE %d : problème d'alignement d'addressage en mémoire du mot !\n", line);

                    // conversion de la string en nombre
                    val_convert = strtol(val_lexem, NULL, 0);  // renvoie un long int => nécessaire pour savoir le nombre rentre dans un entier
                    if (previous_type_lexem == MOINS) val_convert = -val_convert;
                    if ( (val_convert<-2147483648) || (2147483647<val_convert) ) {
                        ERROR_MSG("ERR LINE %d : Nombre trop grand pour etre stocké dans un word (32 bits : -2147483648 < x < 2147483647)\n", line);
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
                    else ERROR_MSG("ERR LINE %d : Virgule manquante apres un argument !\n", line);
                }
                if (type_lexem == NL) {
                    ERROR_MSG("ERR LINE %d : Element manquant apres un .word !\n", line);
                }
                ERROR_MSG("ERR LINE %d : Element non acceptable apres un .word !\n", line);


            case PASCIIZ:
                if (type_lexem == STRING) {

                    *pcurrent_list = add_to_current_list(PASCIIZ, val_lexem, *pdecalage, line, *pcurrent_list);
                    *pdecalage += strlen(val_lexem)+1;   //+1 pour le \0

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
                    else ERROR_MSG("ERR LINE %d : Element invalide apres un argument !\n", line);
                }
                else ERROR_MSG("ERR LINE %d : Element non acceptable apres un .asciiz !\n", line);
                break;

            case PSPACE :
                // Seuls les entiers non signés sont acceptés
                if (type_lexem == MOINS) {
                    ERROR_MSG("ERR LINE %d : Seuls entiers non signés acceptés après .space !\n", line);
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
                else ERROR_MSG("ERR LINE %d : Element non acceptable apres un .space !\n", line);
                break;

            case PBYTE:
                if (type_lexem == MOINS) break;

                if ((type_lexem == HEXA) || (type_lexem == OCTAL) || (type_lexem == DECIMAL)) {
                    // conversion de la string en nombre
                    val_convert = strtol(val_lexem, NULL, 0);
                    if (previous_type_lexem == MOINS) val_convert = -val_convert;
                    if ( (val_convert<-128) || (127<val_convert) ) {
                        ERROR_MSG("ERR LINE %d : Nombre trop grand pour etre stocké dans un byte (32 bits : -128 < x < 127)\n", line);
                    }
                    *pcurrent_list = add_to_current_list(PSPACE, &val_convert, *pdecalage, line, *pcurrent_list);
                    (*pdecalage)++;
                }

                else ERROR_MSG("ERR LINE %d : Element non acceptable apres un .byte !\n", line);

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
    *p_symb_table=look_for_undefined_etiq_in_instr(*p_list_instr, *p_symb_table);
    // list_data
    *p_symb_table=look_for_undefined_etiq_in_data(*p_list_data, *p_symb_table);
    // list_bss
    *p_symb_table=look_for_undefined_etiq_in_bss(*p_list_bss, *p_symb_table);


    // il faut inverser l'ordre des éléments de ces liste car on a ajouter les élément en tete de liste
    *p_list_instr = revers_list(*p_list_instr);
    *p_list_data  = revers_list(*p_list_data);
    *p_list_bss   = revers_list(*p_list_bss);

    return;
}
