#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <strings.h>

#include <fct_analyse_1.h>
#include <global.h>
#include <notify.h>
#include <lex.h>
#include <queue_list.h>
#include <test.h>
#include <etiq.h>

#include <error.h>



int look_for_etiq( LIST symb_table, char* lexem) // renvoit 1 si trouvée, 0 sinon
 {
     int a;
     while (symb_table != NULL)
     {
         a = strcmp(((ETIQ) symb_table->element)->name, lexem); // conversion en ETIQ de (symb_table->element)
         if (a == 0)

         {
             return 1; //alors l'étiquette a été trouvée
         }
         symb_table = symb_table->next;
     }

     return 0; //étiquette non trouvée
 }

LIST add_to_symb_table(char* name_etiq, int dec, int line, int sect, int def, LIST symb_table)
{
    ETIQ etiq = calloc(1, sizeof(*etiq));
    etiq-> decalage = dec;
    etiq-> nline = line;
    etiq-> section = sect;
    etiq-> name = strdup(name_etiq);
    etiq-> def_in_file = def;
    symb_table = add_to_end_list(symb_table, etiq);
    return symb_table;
}


ETIQ look_for_etiq_and_return( LIST symb_table, char* lexem)
 {
     int a;
     while (symb_table != NULL)
     {
         a = strcmp(((ETIQ) (symb_table->element))->name, lexem);
         if (a == 0) {
             return symb_table->element;
         }
         symb_table = symb_table->next;
     }
     ERROR_MSG("Probleme etiquette %s non trouvee dans table des symboles !\n",((ETIQ) (symb_table->element))->name );
 }


char** make_sym_char_table(LIST symb_table, int size_list){
    char** sym_char = calloc(size_list, sizeof(char*));
    int i=0;

    // parcourir la liste
    while (symb_table != NULL){
        sym_char[i] = strdup(((ETIQ) (symb_table->element))->name);
        i = i+1;
        symb_table = symb_table->next;
    }

    return sym_char;

}


ETIQ look_for_etiq_and_return_no_error( LIST symb_table, char* lexem) {
    while (symb_table != NULL) {
        if ( !strcmp(((ETIQ) (symb_table->element))->name, lexem) ) {
            return symb_table->element;
        }
        symb_table = symb_table->next;
    }
    return NULL;
}

/* prend sort_list, liste vide en paramètre et la replit */
LIST sort_symb_table(LIST sort_list, LIST symb_table, LIST list_lex) {
    LEXEM lex;
    LEXEM old_lex;
    ETIQ symb;
    ETIQ cp_symb;
    LIST p;
    for ( p=list_lex ; p!=NULL ; p=p->next ) {
        lex = (LEXEM)p->element;
        if (lex->lex_type == SYMBOLE) {

            //chercher symbole dans symb_table
            symb = look_for_etiq_and_return_no_error( symb_table, lex->value );

            // WARNING old_lex == NULL   OR   p->next == NULL
            if ( (old_lex!=NULL) && (p->next!=NULL) ) {
                if ( (old_lex->lex_type==NL) && ( ((LEXEM)p->next->element)->lex_type!=DEUX_PTS) ) {    // c'est une instruction avec une étiquette qui à le m^eme nom !!
                    symb = NULL;
                }
            }

            if (symb!=NULL) {   // c'est une étiquette
                if ( !look_for_etiq(sort_list, lex->value) ) {  // il n'y est pas encore
                    // WARNING Pas de copie pour l'instant => pas besoin
                    // cp_symb = calloc(1,sizeof(*cp_symb));
                    // memcpy(cp_symb, symb, sizeof(*cp_symb));
                    sort_list = add_to_end_list( sort_list , symb );
                }
            }
            //else c'est une instruction
        }
        old_lex=lex;
    }
    return sort_list;
}
