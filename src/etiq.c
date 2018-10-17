#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <strings.h>

#include <global.h>
#include <notify.h>
#include <lex.h>
#include <queue_list.h>
#include <test.h>

#include <etiq.h>

#include <error.h>



LIST build_list_etiq( LIST list_lex ) {
    LIST tab_etiq = new_list();

    LEXEM lex;
    ETIQ etiq = calloc(1, sizeof(*etiq));   //calloc ??????????

    // TODO compteur de décalage

    // TODO verif si etiq pas déjà definie OU ecraser si deja definie

    LIST l;
    for ( l=list_lex ; l!=NULL ; l=l->next ) {
        lex = (LEXEM) l->element;
        if ( lex->lex_type==SYMBOLE) {
            if (l->next!=NULL) {  // normalement il aura toujours un \n à la fin,
                                  // mais on sait jamais (si on ne rajoute plus le \n à la fin de chaque ligne)
                if ( ((LEXEM) l->next->element)->lex_type==DEUX_PTS) {
                    // ajoute à tab_etiq
                    printf("%d   %s\n", lex->nline, lex->value);
                    etiq->name = strdup(lex->value);   //strdup ou ne pas free 2x
                    etiq->nline = lex->nline;
                    // add_head ou add_end   (ETIQ)
                }
            }
            else ;// probleme
        }
    }

    return tab_etiq;
}

enum {NONE, TEXT, DATA, BSS}

void section_counter( LEXEM lex , int curr_section, int* text_counter, int* data_counter, int* bss_counter ) {

    if // .text
    else if
    else if
    else
        if // no comm
        if // no none

    if // \n


    //pas compter les comm
    return section;
}
