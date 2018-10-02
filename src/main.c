
/**
 * @file main.c
 * @author François Portet <francois.portet@imag.fr> from François Cayre
 * @brief Main entry point for MIPS assembler.
 */
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>

#include <global.h>
#include <notify.h>
#include <lex.h>
#include <queue_list.h>
#include <test.h>

#include <error.h>

#include <assert.h>


/**
 * @param exec Name of executable.
 * @return Nothing.
 * @brief Print usage.
 *
 */
void print_usage( char *exec ) {
    fprintf(stderr, "Usage: %s file.s\n",
            exec);
}



/**
 * @param argc Number of arguments on the command line.
 * @param argv Value of arguments on the command line.
 * @return Whether this was a success or not.
 * @brief Main entry point for MIPS assembler.
 *
 */
int main ( int argc, char *argv[] ) {


    if ( !strcmp(argv[argc-1], "-t") ) {

        /* ---------------- TEST ----------------*/
        type_lex_test();

        exit( EXIT_SUCCESS );
    }

    unsigned int 	nlines 	= 0;
    char         	 *file 	= NULL;

    // /* exemples d'utilisation des macros du fichier notify.h */
    // /* WARNING_MSG : sera toujours affiche */
    // WARNING_MSG("Un message WARNING_MSG !");
    //
    // /* macro INFO_MSG : uniquement si compilé avec -DVERBOSE. Cf. Makefile*/
    INFO_MSG("Debut du programme %s", argv[0]);
    //
    // /* macro DEBUG_MSG : uniquement si compilé avec -DDEBUG (ie : compilation avec make debug). Cf. Makefile */
    // DEBUG_MSG("Un message DEBUG_MSG !");
    //
    // /* La macro suivante provoquerait l'affichage du message
    //    puis la sortie du programme avec un code erreur non nul (EXIT_FAILURE) */
    // /* ERROR_MSG("Erreur. Arret du programme"); */

    if ( argc <2 ) {
        print_usage(argv[0]);
        exit( EXIT_FAILURE );
    }

    file  	= argv[argc-1];

    if ( NULL == file ) {
        fprintf( stderr, "Missing ASM source file, aborting.\n" );
        exit( EXIT_FAILURE );
    }


    LIST list_lex = new_list();


    /* ---------------- do the lexical analysis -------------------*/
    INFO_MSG("Début de l'analyse lexical");
    list_lex = lex_load_file( file, &nlines , list_lex );

    DEBUG_MSG("source code got %d lines",nlines);



    INFO_MSG("Affichage du résultat de l'analyse lexical");
    print_list_lex(list_lex);


    /* ---------------- Free memory and terminate -------------------*/
    INFO_MSG("Libération de la mémoire");
    free_list_lex(list_lex);

    INFO_MSG("Fin du programme %s", argv[0]);
    exit( EXIT_SUCCESS );
}
