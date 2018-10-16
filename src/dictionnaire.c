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




QUEUE open_dict(char *file) //fonctionne ! 
{
    FILE *fp = NULL;
    char inst[10];
    int nb_arg;
    QUEUE list_dico = new_queue();

    fp = fopen( file, "r" );
    //printf (" ----LISTE INSTRUCTIONS----\n");
    if (fp != NULL)
    {
        while (fscanf (fp, "%s %d", inst, &nb_arg) != EOF)
            {
                //printf (" Instruction %s et arguments: %d \n", inst, nb_arg); // pour tester
                list_dico = add_definition(list_dico, nb_arg, inst);
            }
    }
    read_queue_word(list_dico); // test
    list_dico = queue_to_list(list_dico); // peu optimisé ?
    return list_dico;
}



int look_for_inst(char* lex, QUEUE l_dico) //renvoit 1 si instruction trouvée, 0 sinon
{
    return 0;
}




QUEUE add_definition ( QUEUE Q, int nb_arg, char* inst) // ressemble fortement à add_to_queue
{
    WORD def = calloc (1, sizeof(def));
    def->arg = nb_arg;
    def->instruction = strdup(inst);
    Q = ajouter_def_fin(Q, def);
    return Q;
}



QUEUE ajouter_def_fin(QUEUE Q, WORD def) // quasi idem que ajouter_fin
{
  if (Q == NULL)
    {
      Q = calloc (1, sizeof(Q));
      Q->element = def;
      Q->next = Q;
      return Q;
    }
  QUEUE adresse_retour = Q->next;
  Q->next = calloc (1, sizeof(*Q));
  Q = Q->next;
  Q->element = def;
  Q->next = adresse_retour;
  return Q;
}
