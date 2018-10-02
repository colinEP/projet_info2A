#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include <notify.h>

#include <error.h>


/* WARNING
pos != column : pos = 0..len-1
                column = 1..len
*/
void print_error_lex(char* msg, unsigned int nline, int pos, char* line) {

    //int pos = 210; //tmp
    int column = pos+1;
    int i;

    fprintf( stderr, "%c[%d;%dm", 0x1B, STYLE_BOLD, COLOR_WHITE );  // set style ans color
    fprintf( stderr, "\nERROR : %d:%d:   ", nline, column );
    fprintf( stderr, "%c[%dm", 0x1B, 0 );  // reset style and color
    fprintf( stderr, "%s\n", msg );


    if ( column > strlen(line)+1 ) {  // +1 car la column peut aller à strlen(line)+1 pour afficher le curseur une column après la fin de la ligne
                                      // pas de risque de seg fault car on affichera '\0' en dernier caractere
        /* normalement ne devrait jamais arriver */
        ERROR_MSG("Erreur de position : column > strlen(line)");
    }

    /* affiche le debut de la ligne */
    fprintf( stderr, "  ");
    for ( i=0 ; i<pos ; i++ ) fprintf( stderr, "%c", line[i]);
    /* affiche le caractere ou l'erreur est apparue */
    fprintf( stderr, "%c[%d;%dm", 0x1B, STYLE_BOLD, COLOR_RED );  // set style ans color
    fprintf( stderr, "%c", line[i]);
    fprintf( stderr, "%c[%dm", 0x1B, 0 );  // reset style and color
    i++;
    /* affiche la fin de la ligne */
    for ( ; i<strlen(line) ; i++ ) fprintf( stderr, "%c", line[i]);
    fprintf( stderr, "\n");

    /* affiche une ligne avec un curseur qui montre ou est l'erreur */
    fprintf( stderr, "  ");
    for (i=0 ; i<pos ; i++) fprintf( stderr, " ");
    fprintf( stderr, "%c[%d;%dm", 0x1B, STYLE_BOLD, COLOR_RED );  // set style ans color
    fprintf( stderr, "^\n");
    fprintf( stderr, "%c[%dm", 0x1B, 0 );  // reset style and color

    exit( EXIT_FAILURE );
}
