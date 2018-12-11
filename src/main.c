
/**
 * @file main.c
 * @author François Portet <francois.portet@imag.fr> from François Cayre
 * @brief Main entry point for MIPS assembler.
 */
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>

#include <ctype.h>

#include <global.h>
#include <notify.h>
#include <lex.h>
#include <queue_list.h>
#include <test.h>
#include <etiq.h>
#include <dictionnaire.h>
#include <analyse_synth.h>
#include <print_functions.h>
#include <fct_analyse_1.h>
#include <reloc.h>
#include <make_elf.h>
#include <error.h>
//#include <etiq.h>

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

    /*   ./as-mips -t   permet de lancer les tests */
    if ( !strcmp(argv[argc-1], "-t") ) {

        /* ---------------- TEST ----------------*/
        // int nb_arg = -1;
        // int Retour = look_for_inst("GAA", dictionnaire, &nb_arg);
        // printf("%d  %d", Retour, nb_arg);

        type_lex_test();
        test_register();

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



    /* ----------- Chargement du dico d'instructions -------------*/
    INFO_MSG("Chargement du dictionnaire d'instructions");
    LIST dictionnaire = open_dict("dictionnaire.txt");
    INFO_MSG("Affichage du dictionnaire d'instructions");
    print_list_dico( dictionnaire );



    /* ---------------- do the lexical analysis -------------------*/
    INFO_MSG("Début de l'analyse lexical");
    LIST list_lex = new_list();
    list_lex = lex_load_file( file, &nlines , list_lex );
    DEBUG_MSG("source code got %d lines",nlines);
    INFO_MSG("Affichage du résultat de l'analyse lexical");
    print_list_lex(list_lex);

    /* ---------------- do the grammatical analysis -------------------*/
    INFO_MSG("Début de l'analyse grammatical");
    LIST list_instr = new_list();
    LIST list_data  = new_list();
    LIST list_bss   = new_list();
    LIST symb_table = new_list();
    // On doit passer les pointeurs des listes car leur addresse de début change dans analyse_synth!
    analyse_synth(&list_instr, &list_data, &list_bss, &symb_table, list_lex, dictionnaire);

    /* ----- Relocation ------- */    //NOTE prototype dans etiq.h ?????
    //TODO
    LIST reloc_table_text = reloc_and_replace_etiq_by_dec_in_instr (list_instr, symb_table);
    LIST reloc_table_data = reloc_and_replace_etiq_by_dec_in_data (list_data, symb_table);

    print_symb_table(symb_table);
    print_list_instr(list_instr);
    print_list_data(list_data);
    print_list_bss(list_bss);
    print_reloc_table( reloc_table_text);
    print_reloc_table( reloc_table_data);


    /* ---------------- test_binaire -------------------*/

    // int bin = 0x022;
    // printf("EXEMPLE : en decimal, 0x022 vaut : %d \n",bin );
    //
    // int bin2 = 34;
    // printf("EXEMPLE : en decimal, 34 vaut : %hx \n",bin );


    instr_in_binar(list_instr, 50 , dictionnaire); // WARNING a changer
    data_in_binar(list_data, 50);   // WARNING WARNING à changer

    /* ---------------- make mips_elf -------------------*/
     main_init_function();



    // char* str = ((LEXEM)list_lex->element)->value;
    // str = "ils : \n\"au ru!\"";
    // printf("%s\n", str);
    // char* tmp;
    // printf("%ld\n", strlen(str));
    // printf("%d\n", sprintf(tmp, str));

    //TODO TODO gestion \ dans asciiz

    /* ---------------- Free memory and terminate -------------------*/
    INFO_MSG("Libération de la mémoire");
    free_list_dico(dictionnaire);
    free_list_lex(list_lex);
    // free_list_inst  //TODO
    // free_list_data  //TODO
    // free_list_bss   //TODO
    // free_symb_table //TODO


    INFO_MSG("Fin du programme %s", argv[0]);
    exit( EXIT_SUCCESS );
}
