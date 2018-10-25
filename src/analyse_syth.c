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

            switch (type_lexem)
            {
                case SYMBOLE: // les symboles peuvent être des instr, des declarations d'etiq ou des appels d'etiq

                    // ---cas 1 : une instruction ?
                    val = look_for_inst(lex, dictionnaire, &nb_arg_needed);

                    if (val == 1) //ci c'est une instruction
                    {
                        if (section != TEXT) // pas d'instruction possible hors de la section .Text
                            {
                                ERROR_MSG("Erreur, instruction dans mauvaise section !\n");
                            }

                        //add_to_list(current_list, lexem, nb_arg_needed); //fonction a créer !

                        break;
                    }

                    // si on arrive ici, c'est que ce n'est pas une instruction

                    // ----cas 2 : une déclaration d'étiquette
                    if ( ( ((LEXEM)((LIST)(list_lex->next))->element)->lex_type) == DEUX_PTS) // c'est une déclaration d'étiquette
                         {
                             // check qu'elle n'a pas déjà été définie
                             // ajout dans symb_table
                             list_lex = list_lex->next; // on va passer les DEUX_PTS parce que du coup ils ne nous importent pas
                             break;
                         }

                    // sinon ce n'est pas une définition d'étiquette et il faut chercher dans la liste des etiquettes
                    // en fct de si elle est trouvée ou pas, mettre à jour une variable True/False
                    // compter un argument

                    else
                    {
                        // defined_etiq = fonction_chercher_dans_symb_table; (true ou false)
                    }


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
