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

#include <error.h>
#include <assert.h>




QUEUE open_dict(char *file) //fonctionne ! Cette fonction ouvre le dictionnaire et stocke les instructions et leur nb arg dans une liste renvoyée
{
    FILE *fp = NULL;
    char inst[10];
    char arg1[10];
    char arg2[10];
    char arg3[10];
    int nb_arg;
    QUEUE list_dico = new_queue();

    fp = fopen( file, "r" );
    //printf (" ----LISTE INSTRUCTIONS----\n");

    if ( NULL == fp ) {
        /*macro ERROR_MSG : message d'erreur puis fin de programme ! */
        ERROR_MSG("Error while trying to open %s file --- Aborts",file);
    }

    while (fscanf (fp, "%s %d %s %s %s", inst, &nb_arg, arg1, arg2, arg3) != EOF){
            //printf (" Instruction %s et arguments: %d \n", inst, nb_arg); // pour tester
            list_dico = add_definition(list_dico, nb_arg, inst, arg1, arg2, arg3);
        }
    read_queue_word(list_dico); // test
    list_dico = queue_to_list(list_dico); // peu optimisé ?
    return list_dico;
}



int look_for_inst(char* lex, LIST l_dico, int* pnb_arg) //renvoit 1 si instruction lex trouvée dans le dictionnaire, 0 sinon
{
    if (l_dico == NULL)
    {
        ERROR_MSG("Erreur, liste dictionnaire vide !\n");
    }

    int a;
    int i = 0;
    while (l_dico != NULL)
    {
        a = strcmp(((WORD) l_dico->element)->instruction, lex); // conversion en WORD de (dico->element)
        i ++;
        if (a == 0)

        {
            *pnb_arg = ((WORD) l_dico->element)->arg;
            printf("valeur arguments : %d et compteur de boucle %d\n", *pnb_arg, i);
            return 1; //alors l'instruction a été trouvée !
        }
        l_dico = l_dico->next;
    }

    return 0; //oups ! instruction non trouvée
}






QUEUE add_definition ( QUEUE Q, int nb_arg, char* inst, char* a_type_1, char* a_type_2, char* a_type_3) // EVENTUELLEMENT A METTRE DANS LE CODE AVEC SEUL APPEL A ajouter_fin
{
    WORD def = calloc (1, sizeof(def));
    def->arg = nb_arg;
    def->instruction = strdup(inst);
    def->arg_type_1 = strdup(a_type_1);
    def->arg_type_2 = strdup(a_type_2);
    def->arg_type_3 = strdup(a_type_3);
    Q = ajouter_fin(Q, def);
    return Q;
}
