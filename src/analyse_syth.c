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

#include <error.h>
#include <assert.h>

enum{TEXT, DATA, BSS, NONE};
enum{INIT, INSTRUCTION, PWORD, PBYTE, PASCIIZ, PSPACE}


int check_instruction(char* lex, LIST dictionnaire) // vérifie que l'instruction existe et renvoit son nb d'arg = INUTILE ?
{
    int val;
    int nb_arg_needed;
    val = look_for_inst(lex, dictionnaire, &nb_arg_needed);
    if (val == 0)
        {
            ERROR_MSG("Erreur, instruction %s non valable !\n", lex); // rajout num de ligne ?
        }
    return nb_arg_needed;

}

void analyse_synth(LIST list_instr, LIST list_data, LIST list_bss, LIST symb_table, LIST list_lex )
{

        LIST dictionnaire;
        dictionnaire = open_dict("dictionnaire.txt");

        // --- initialisation des variables ----
        int dec_data = 0;
        int dec_bss = 0;
        int dec_txt = 0;
        int section = NONE ;
        LEXEM lexem = NULL;
        int type_lexem;
        char* val_lexem;
        LIST* current_list = NULL; // est-ce que cela peut nous poser de soucis ??
        int nb_arg_ligne = 0;
        int nb_arg_needed = 0;
        val = 0 ; // variable pour look_for_inst



        // --- premier parcours de la liste ----

        while (list_lex != NULL)
        {   lexem = list_lex-> element;
            type_lexem = lexem->lex_type;
            val_lexem = lexem->value;

            if (type_lexem == DIRECTIVE)
            {
                // ---- modification de la section et de la liste courante ----
                if (strcmp(val_lexem, ".data") == 0)
                {
                    section = DATA;
                    *current_list = list_data;
                }
                if (strcmp(val_lexem, ".bss") == 0)
                {
                    section = BSS;
                    *current_list = list_bss;
                }
                if (strcmp(val_lexem, ".text") == 0)
                {
                    section = TEXT;
                    *current_list = list_instr;
                }
                // passer au suivant
            }

            int S = INIT;

            switch (S)
            {
                case INIT: // cas lexeme de début de ligne

                // ------------ CASE S = INIT -----------------
                    switch(type_lexem) {
                        case SYMBOLE:
                        // cas 1 : une déclaration d'étiquette ? (on commence par ce cas car certaines etiquettes ont le même nom que des instructions)
                            if ( ( ((LEXEM)((LIST)(list_lex->next))->element)->lex_type) == DEUX_PTS) {
                                 // check qu'elle n'a pas déjà été définie
                                 // ajout dans symb_table
                                 list_lex = list_lex->next; // on va passer les DEUX_PTS parce que du coup ils ne nous importent pas
                                 break;
                             }

                             // cas 2 : une instruction ?
                            val = look_for_inst(lex, dictionnaire, &nb_arg_needed);
                            if (val == 1)
                            {
                                if (section != TEXT) // pas d'instruction possible hors de la section .Text
                                    {
                                        ERROR_MSG("Erreur, instruction dans mauvaise section !\n");
                                    }
                                //add_to_list(current_list, lexem, nb_arg_needed); //fonction a créer !
                                S = INSTRUCTION;
                                break;
                            }
                            // cas 3
                            else { // c'est une erreur car une ligne ne peut pas commencer par un appel d'etiquette !
                                ERROR_MSG("Erreur, symbole non valide en debut de ligne !\n");
                            }


                        case COMMENT: //on ne considère pas les commentaires, on ne les stocke pas

                            if (( ((LEXEM)((LIST)(list_lex->next))->element)->lex_type) == NL)) {
                                list_lex = list_lex->next; // on saute le NL qui suit
                                S = INIT;
                                break;
                            }

                            ERROR_MSG("Erreur, après le commentaire !\n"); // est-ce possible ???


                        case DIRECTIVE:
                            if (section == DATA ){
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

                        // ------------ FIN CASE S = INIT -----------------


                case NL:
                    if (nb_arg_ligne != nb_arg_needed)
                    {
                        ERROR_MSG("Erreur, mauvais nombre d'arguments pour instruction !\n"); //retrouver cette instruction pour erreur ou la ligne
                    }
                    nb_arg_ligne = 0;
                    nb_arg_needed= 0;


            }

        list_lex = list_lex->next;
        }

        // --- deuxième parcours de la liste ----
        if (list_lex == NULL)
        {
            ERROR_MSG("Erreur, liste lexemes a analyser vide !\n");
        }

        symb_table = build_tab_etiq(list_lex ); //cette fonction remplie la table des symboles avec les etiquettes et la renvoit


        return;
}
