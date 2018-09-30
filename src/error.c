#include <stdlib.h>
#include <stdio.h>

#include <notify.h>

#include <error.h>

void print_error(char* msg, unsigned int nline) {
    ERROR_MSG("ATTENTION ERREUR line %d:", nline);
    fprintf( stderr, "%c[%d;%dm", 0x1B, STYLE_BLINK, COLOR_RED );
    fprintf( stderr, "%s\n", msg);


    /* style ans color reset */
    fprintf( stderr, "%c[%dm", 0x1B, 0 );

    exit( EXIT_FAILURE );
}
