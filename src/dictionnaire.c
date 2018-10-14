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
#include <dictionnaire.h>


void open_dict(char *file, LISTE l_dico)
{
    FILE *fp = NULL;
    char inst[10];
    int nb_arg;
    FILE l_dico;

    fp = fopen( file, "r" );
    if (fp != NULL)
    {
        fscanf (fp, "%s %d", inst, &nb_arg);
        printf (" instruction %s et arguments: %d", inst, nb_arg); // pour tester
        add_to_list(l_dico, inst, nb_arg); // fonction à créer
    }
}


int look_for_inst(char* lex, LISTE l_dico)
{
    
}
