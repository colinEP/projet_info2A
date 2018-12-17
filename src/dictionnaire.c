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
#include <dictionnaire.h>
#include <print_functions.h>
#include <fct_analyse_1.h>

#include <error.h>
#include <assert.h>




QUEUE open_dict(char *file) //fonctionne ! Cette fonction ouvre le dictionnaire et stocke les instructions et leur nb arg dans une liste renvoyée
{
    int nline = 1;
    FILE *fp = NULL;
    char inst[16];  // taille max des instructions = 14   (voir boucle while ci-dessous)
    int nb_arg;
    char arg1[10];
    char arg2[10];
    char arg3[10];
    int opcode;
    char c_b_1[10];
    char c_b_2[10];
    char c_b_3[10];
    char c_b_4[10];
    char c_b_5[10];

    QUEUE list_dico = new_queue();

    fp = fopen( file, "r" );
    //printf (" ----LISTE INSTRUCTIONS----\n");

    if ( NULL == fp ) {
        /*macro ERROR_MSG : message d'erreur puis fin de programme ! */
        ERROR_MSG("Error while trying to open %s file --- Aborts",file);
    }

    while (fscanf (fp, "%s %d %s %s %s %d %s %s %s %s %s", inst, &nb_arg, arg1, arg2, arg3, &opcode, c_b_1, c_b_2, c_b_3, c_b_4, c_b_5 ) != EOF){
    // on recupère au plus les 15 premiers caracteres de l'inst => pas de risque de buffer-overflow
            // fscanf (fp,"%*[^\n]");      // si buffer excedent, on le vide
            // if ( strlen(inst) == 15) {  // test si taille inst trop longue
            //     ERROR_MSG("Instruction dans le dico (line %d) trop longue (length max = 14 characters)", nline);
            // }
            //printf (" Instruction %s et nb_arguments: %d \n", inst, nb_arg); // pour DEBUG

            ////////////////////////////////////////////////////////////
            ///  WARNING  ///// SAISIE PAS SECURISEE /////  WARNING  ///
            ////////////////////////////////////////////////////////////

            /* il faut pas que qq'un s'amuse à modifier le dico, sinon ça peut partir en cacahuète !!  */

            list_dico = add_definition(list_dico, inst, nb_arg, arg1, arg2, arg3, opcode, c_b_1, c_b_2, c_b_3, c_b_4, c_b_5);
            nline++;
        }

    //read_queue_dico(list_dico); // test
    list_dico = queue_to_list(list_dico);
    fclose(fp);
    return list_dico;
}



int look_for_inst(char* lex_init, LIST l_dico, int* pnb_arg, int* exp_typ_1, int* exp_typ_2, int* exp_typ_3) //renvoit 1 si instruction lex trouvée dans le dictionnaire, 0 sinon
{
    if (l_dico == NULL)
    {
        ERROR_MSG("Erreur, liste dictionnaire vide !\n");
    }

    int a;
    int i = 0;
    // --- conversion en majuscules ---
    char* lex;
    lex = strdup(lex_init); // pour ne pas modifier la chaine initiale
    lex = put_in_uppercase(lex);

    while (l_dico != NULL)
    {
        a = strcmp(((DICO_LINE) l_dico->element)->instruction, lex); // conversion en DICO_LINE de (l_dico->element)
        i ++;
        if (a == 0)

        {
            //printf("valeur arguments : %d et compteur de boucle %d\n", *pnb_arg, i);
            *pnb_arg = ((DICO_LINE) l_dico->element)->arg;
            *exp_typ_1 = convert_inst_op_type(((DICO_LINE) l_dico->element)->arg_type_1);
            *exp_typ_2 = convert_inst_op_type(((DICO_LINE) l_dico->element)->arg_type_2);
            *exp_typ_3 = convert_inst_op_type(((DICO_LINE) l_dico->element)->arg_type_3);
            free(lex);
            return 1; //alors l'instruction a été trouvée !
        }
        l_dico = l_dico->next;
    }
    free(lex);
    return 0; //oups ! instruction non trouvée
}


char* put_in_uppercase (char* chaine)
{
    int lgr = strlen(chaine);
    int i;
    for (i=0 ; i <lgr ; i++) {
        char c = chaine[i];
        if ((c>= 'a') && (c<='z')) {
            chaine[i] = c - 32; // on met en majuscule
        }
    }
    return chaine;
}




QUEUE add_definition ( QUEUE Q, char* inst, int nb_arg, char* a_type_1, char* a_type_2, char* a_type_3,int opcode, char* c_b_1, char* c_b_2, char* c_b_3, char* c_b_4, char* c_b_5) // EVENTUELLEMENT A METTRE DANS LE CODE AVEC SEUL APPEL A ajouter_fin
{
    DICO_LINE def = calloc (1, sizeof(*def));

    def->instruction = strdup(inst);
    def->arg         = nb_arg;
    def->arg_type_1  = strdup(a_type_1);
    def->arg_type_2  = strdup(a_type_2);
    def->arg_type_3  = strdup(a_type_3);
    def->opcode         = opcode;
    def->code_bin_1  = strdup(c_b_1);
    def->code_bin_2  = strdup(c_b_2);
    def->code_bin_3  = strdup(c_b_3);
    def->code_bin_4  = strdup(c_b_4);
    def->code_bin_5  = strdup(c_b_5);

    Q = ajouter_fin(Q, def);
    return Q;
}
