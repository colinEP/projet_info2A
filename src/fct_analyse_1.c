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
#include <fct_analyse_1.h>
#include <analyse_synth.h>
#include <print_functions.h>
#include <dictionnaire.h>

#include <etiq.h>

#include <error.h>



INSTR new_instr(){ //initialisation de la cellule instruction
    INSTR I = calloc(1, sizeof(*I));
    I->Exp_Type_1 = None;
    I->Exp_Type_2 = None;
    I->Exp_Type_3 = None;

    I-> arg1 = calloc(1, sizeof(*(I-> arg1)));
    ((ARG_INST)(I-> arg1))->etiq_def = -1; // initialisation de la variable qui indique si etiquette definie dans symb_tab. A -1 pour faciliter deuxième passage de la boucle
    ((ARG_INST)(I-> arg1))->type = None;

    I-> arg2 = calloc(1, sizeof(*(I-> arg2)));
    ((ARG_INST)(I-> arg2))->etiq_def = -1;
    ((ARG_INST)(I-> arg1))->type = None;

    I-> arg3 = calloc(1, sizeof(*(I-> arg3)));
    ((ARG_INST)(I-> arg3))->etiq_def = -1;
    ((ARG_INST)(I-> arg1))->type = None;

    return I;
}



LIST add_to_list_instr(LEXEM l, int dec, int nbarg, LIST list_instr, int exp_typ_1, int exp_typ_2, int exp_typ_3)
{

    INSTR I = new_instr();
    I-> decalage = dec;
    I-> nb_arg = nbarg;
    // copy du lexem
    I-> lex = calloc(1, sizeof(*l));
    I-> lex = memcpy(I->lex, l, sizeof(*l));
    I-> lex->value = strdup(l->value);

    I->Exp_Type_1 = exp_typ_1;
    I->Exp_Type_2 = exp_typ_2;
    I->Exp_Type_3 = exp_typ_3;
    list_instr = add_to_head_list(list_instr, I);
    return list_instr;
}


DATA new_data(){ //initialisation de la cellule DATA
    DATA data = calloc(1, sizeof(*data));
    data->etiq_def = 0; // initialisation de la variable qui indique si etiquette definie dans symb_tab
    return data;
}


data_op fill_val_op(void* pvalue, operand_type type_op)
{
    data_op D = calloc (1, sizeof(*D));
    D->type = type_op;
    switch(D->type)
        {
            case PWORD:
                D->val.PWORD = (*(int*)pvalue);
                break;
            case PBYTE:
                D->val.PBYTE = *(char*)pvalue;
                break;
            case PASCIIZ:
                D->val.PASCIIZ = (char*)strdup(pvalue);
                break;
            case PSPACE:
                D->val.PSPACE = (*(unsigned int*)pvalue);
                break;
            case LABEL:
                D->val.LABEL = (char*)strdup(pvalue);
                break;
        }
    return D;
}


LIST add_to_current_list(operand_type type_op, void* pvalue, int dec, int line, LIST current_list){ // FONCTIONNE
    DATA data = new_data();
    data-> decalage = dec;
    data-> line = line;
    data -> D = fill_val_op(pvalue, type_op);
    current_list = add_to_head_list(current_list, data);
    return current_list;
}



 LIST add_label(int nb_arg_ligne, inst_op_type type, char* value, int etiq_definition, LIST list_instr) // cas 0 ??
{   // copie les etiquette en char* pour le moment

     ARG_INST A;
     if ( nb_arg_ligne == 1 ) {
         A = ((ARG_INST)(((INSTR)(list_instr->element))->arg1));
         A->type = type;
         A->val.char_chain = strdup(value);
         A->etiq_def = etiq_definition;
         return list_instr;
      }
      else {
          if (  nb_arg_ligne == 2 ){
               A = ((ARG_INST)(((INSTR)(list_instr->element))->arg2));
               A->type = type;
               A->val.char_chain = strdup(value);
               A->etiq_def = etiq_definition;
               return list_instr;
          }
          else {
              if (  nb_arg_ligne == 3 ){
                  A = ((ARG_INST)(((INSTR)(list_instr->element))->arg3));
                  A->type = type;
                  A->val.char_chain = strdup(value);
                  A->etiq_def = etiq_definition;
                  return list_instr;
              }
              else {
                  printf("ERREUR LIGNE : %d\n", ((LEXEM)((INSTR)(list_instr->element))->lex)->nline);
                  ERROR_MSG("Trop d'arguments !\n");
              }
          }
      }
      return list_instr;

 }
LIST add_int(int nb_arg_ligne, inst_op_type type, int valeur, int etiq_definition,LIST list_instr){
    ARG_INST A;

    if ( nb_arg_ligne == 1 ) {
        A = ((ARG_INST)(((INSTR)(list_instr->element))->arg1));
        A->type = type;
        A->val.entier = valeur;
        A->etiq_def = etiq_definition;
        return list_instr;
     }
     else {
         if (  nb_arg_ligne == 2 ){
              A = ((ARG_INST)(((INSTR)(list_instr->element))->arg2));
              A->type = type;
              A->val.entier = valeur;
              A->etiq_def = etiq_definition;
              return list_instr;
         }
         else {
             if (  nb_arg_ligne == 3 ){
                 A = ((ARG_INST)(((INSTR)(list_instr->element))->arg3));
                 A->type = type;
                 A->val.entier = valeur;
                 A->etiq_def = etiq_definition;
                 return list_instr;
             }
             else {
                 printf("ERREUR LIGNE : %d\n",  ((LEXEM)((INSTR)(list_instr->element))->lex)->nline);
                 ERROR_MSG("Trop d'arguments !\n");
             }
         }
     }
     return list_instr;
 }

 LIST fill_arguments(LEXEM lexem, LIST list_instr, int previous_type_lexem, int etiq_definition, int* p_nb_arg_ligne) // faire la verification des type d'arg ICI
 {

     if (previous_type_lexem == MOINS){
         //lexem->value = strdup(mystrcat("-", lexem->value)); //pas du dup car calloc déjà dans mystrcat
         char* old_value = lexem->value;
         lexem->value = (mystrcat("-", lexem->value));
         free(old_value);
     }

     if (etiq_definition != -1){
         // cas où l'on a une étiquette, pas besoin de checker expected_type ! ce sera fait après !
         list_instr = add_label(*p_nb_arg_ligne, Label, lexem->value, etiq_definition, list_instr);
         return list_instr;
     }


     else { // ce n'est pas une étiquette
        // là il faut checker le type_arg_expected lequel est stocké dans la list_instr
        char* val_lexem = lexem->value;
        int convert_value;
        if (*p_nb_arg_ligne == 1) {

            convert_value = check_type_arg_inst(lexem->lex_type, val_lexem, ((INSTR)(list_instr->element))->Exp_Type_1);
            // renvoit une erreur si jamais les bons types ne sont pas reliés et renvoit un entier correspondant à convert value
            list_instr = add_int(*p_nb_arg_ligne, ((INSTR)(list_instr->element))->Exp_Type_1, convert_value, etiq_definition, list_instr);
            return list_instr;
        }

        if (*p_nb_arg_ligne == 2) {
            if ( ((INSTR)list_instr->element)->Exp_Type_2 == Bas) {
                if (lexem->lex_type == AIBD) {
                    int i=0;
                    int j=0;
                    while ( val_lexem[i]!='(') i++;
                    j=i;
                    while ( val_lexem[j]!=')') j++;

                    char* base_char   = calloc( 1, j-i-1+1 );  // +1 pour le \0
                    char* offset_char = calloc( 1, i+1 );
                    memcpy( base_char, val_lexem+i+1, j-i-1 ); // ajout du \0 automatique ! car calloc initialise à 0
                    memcpy( offset_char, val_lexem, i );

                    ((INSTR)(list_instr->element))->Exp_Type_2 = Reg;
                    ((INSTR)(list_instr->element))->Exp_Type_3 = Imm;
                    ((INSTR)(list_instr->element))->nb_arg     = 3;

                    // arg2 = base (type=Reg)
                    convert_value = check_type_arg_inst(lexem->lex_type, base_char, ((INSTR)(list_instr->element))->Exp_Type_2);
                    list_instr = add_int(*p_nb_arg_ligne, ((INSTR)(list_instr->element))->Exp_Type_2, convert_value, etiq_definition, list_instr);

                    *p_nb_arg_ligne = 3;   // ()*p_nb_arg_ligne) ++

                    // arg3 = offset (type=Imm)
                    convert_value = check_type_arg_inst(lexem->lex_type, offset_char, ((INSTR)(list_instr->element))->Exp_Type_3);
                    list_instr = add_int(*p_nb_arg_ligne, ((INSTR)(list_instr->element))->Exp_Type_3, convert_value, etiq_definition, list_instr);

                    free(base_char);
                    free(offset_char);
                    return list_instr;
                }
                else ERROR_MSG("Erreur, type argument inadapte pour cette instruction (Base_offset attendu)!\n");
            }
            convert_value = check_type_arg_inst(lexem->lex_type, val_lexem, ((INSTR)(list_instr->element))->Exp_Type_2);
            // renvoit une erreur si jamais les bons types ne sont pas reliés et renvoit un entier correspondant à convert value
            list_instr = add_int(*p_nb_arg_ligne, ((INSTR)(list_instr->element))->Exp_Type_2, convert_value, etiq_definition, list_instr);
            return list_instr;
        }

        if (*p_nb_arg_ligne == 3){
            convert_value = check_type_arg_inst(lexem->lex_type, val_lexem, ((INSTR)(list_instr->element))->Exp_Type_3);
            // renvoit une erreur si jamais les bons types ne sont pas reliés et renvoit un entier correspondant à convert value
            list_instr = add_int(*p_nb_arg_ligne, ((INSTR)(list_instr->element))->Exp_Type_3, convert_value, etiq_definition, list_instr);
            return list_instr;
        }
    }
    return list_instr;
}


LIST look_for_undefined_etiq_in_instr(LIST l, LIST symb_table){ // met à 1 etiq def si définie dans le code, sinon ajoute l'etiq à symb_table mais laisse etiq_def à 0

    INSTR I;
    while (l!= NULL){
        I = l->element;

        if ( ((ARG_INST)(I->arg1))->etiq_def == 0 ) {
                int a = 0;
                int b = 1;
                a = look_for_etiq(symb_table,(((ARG_INST)(I->arg1))->val.char_chain));
                if(a==1){ //si etiq dans symbtable
                    ETIQ Et = look_for_etiq_and_return(symb_table,(((ARG_INST)(I->arg1))->val.char_chain));
                    b = Et->def_in_file;
                }
                (I->arg1)->etiq_def = 1;
                if ((a == 0)||(b ==0)) {                        // etiq non trouvée donc non déf --> il faut alors l'ajouter à la table des symboles
                    if (a ==0){                                 // si etiq pas encore dans symb_tab
                        char* name_etiq = (I->arg1)->val.char_chain;  //pas de strdup car add_to_symb_table le fait déjà
                        int dec = 0;
                        int line = (I->lex)->nline ;
                        int sect = TEXT; // car list instr
                        symb_table = add_to_symb_table(name_etiq, dec, line, sect, FALSE, symb_table);
                    }
                    (I->arg1)->etiq_def = 0;
                }

        }
        if ( ((ARG_INST)(I->arg2))->etiq_def == 0 ) {
                int a = 0;
                int b = 1;
                a = look_for_etiq(symb_table, (((ARG_INST)(I->arg2))->val.char_chain));
                if(a==1){
                    ETIQ Et = look_for_etiq_and_return(symb_table,(((ARG_INST)(I->arg2))->val.char_chain));
                    b = Et->def_in_file;
                }
                (I->arg2)->etiq_def = 1;
                if ((a == 0)||(b ==0)) {                          // etiq non trouvée donc non déf
                    if (a ==0){                                 // si etiq pas encore dans symb_tab
                        char* name_etiq = (I->arg2)->val.char_chain;   //pas de strdup car add_to_symb_table le fait déjà
                        int dec = 0;
                        int line = (I->lex)->nline ;
                        int sect = TEXT; // car list instr
                        symb_table = add_to_symb_table(name_etiq, dec, line, sect, FALSE, symb_table);
                    }
                    (I->arg2)->etiq_def = 0;
                }

        }
        if ( ((ARG_INST)(I->arg3))->etiq_def == 0 ) {
                int a = 0;
                int b = 1;
                a = look_for_etiq(symb_table, (((ARG_INST)(I->arg3))->val.char_chain));
                if(a==1){
                    ETIQ Et = look_for_etiq_and_return(symb_table,(((ARG_INST)(I->arg3))->val.char_chain));
                    b = Et->def_in_file;
                }
                (I->arg3)->etiq_def = 1;
                if ((a == 0)||(b ==0)) {                         // etiq non trouvée donc non déf
                    if (a ==0){                                 // si etiq pas encore dans symb_tab
                        char* name_etiq = (I->arg3)->val.char_chain;   //pas de strdup car add_to_symb_table le fait déjà
                        int dec = 0;
                        int line = (I->lex)->nline ;
                        int sect = TEXT; // car list instr
                        symb_table = add_to_symb_table(name_etiq, dec, line, sect, FALSE, symb_table);
                    }
                    (I->arg3)->etiq_def = 0;
                }
        }
        l = l->next;
    }
    return symb_table;
}


LIST look_for_undefined_etiq_in_data(LIST l, LIST symb_table){ // met à 1 etiq def si définie dans le code, sinon ajoute l'etiq à symb_table mais laisse etiq_def à 0
    while (l!= NULL){

        int a = 0;
        int b = 1;
        if ( ((data_op)(((DATA)(l->element))->D))->type == LABEL) { // on ne considère que les etiq, d'où LABEL
            if ( ((DATA)(l->element))-> etiq_def == 0 ){

                a = look_for_etiq(symb_table, ((data_op)(((DATA)(l->element))->D))->val.LABEL);
                if(a==1){
                    ETIQ Et = look_for_etiq_and_return(symb_table,((data_op)(((DATA)(l->element))->D))->val.LABEL);
                    b = Et->def_in_file;
                }
                ((DATA)(l->element))-> etiq_def = 1;

                if ((a == 0)||(b ==0)) {                                         // etiq non trouvée donc non déf
                    char* name_etiq = ((data_op)(((DATA)(l->element))->D))->val.LABEL;  //pas de strdup car add_to_symb_table le fait déjà
                    int dec = 0;
                    int line = ((DATA)(l->element))->line;
                    int sect = PDATA; // car list .data
                    symb_table = add_to_symb_table(name_etiq, dec, line, sect, FALSE, symb_table);
                    ((DATA)(l->element))-> etiq_def = 0;
                }

            }
        }
        l = l->next;
    }
    return symb_table;
}

LIST look_for_undefined_etiq_in_bss(LIST l, LIST symb_table){ // met à 1 etiq def si définie dans le code, sinon ajoute l'etiq à symb_table mais laisse etiq_def à 0
    while (l!= NULL){

        int a = 0;
        if ( ((data_op)(((DATA)(l->element))->D))->type == LABEL) { // on ne considère que les etiq, d'où LABEL
            if ( ((DATA)(l->element))-> etiq_def == 0 ){

                a = look_for_etiq(symb_table, ((data_op)(((DATA)(l->element))->D))->val.LABEL);
                ((DATA)(l->element))-> etiq_def = 1;

                if (a == 0){   // Impossible dans BSS ??                            // etiq non trouvée donc non déf
                    char* name_etiq = ((data_op)(((DATA)(l->element))->D))->val.LABEL;   //pas de strdup car add_to_symb_table le fait déjà
                    int dec = 0;
                    int line = ((DATA)(l->element))->line;
                    int sect = BSS; // car list .bss
                    symb_table = add_to_symb_table(name_etiq, dec, line, sect, FALSE, symb_table);
                    ((DATA)(l->element))-> etiq_def = 0;
                }
            }
        }
        l = l->next;
    }
    return symb_table;
}



char *mystrcat( char *start, char *addend )
{
	size_t slen = strlen( start );
	size_t alen = strlen( addend );
	char   *str = calloc( 1, slen+alen+1 );
	memcpy( str, start, slen );
	memcpy( str+slen, addend, alen ); // ajout du \0 automatique ! car calloc met a 0
    return str;
}


inst_op_type convert_inst_op_type(char* type)
{
    if (strcmp(type, "Reg")==0) return Reg;
    if (strcmp(type, "Imm")==0) return Imm;
    if (strcmp(type, "Sa")==0) return Sa;
    if (strcmp(type, "Bas")==0) return Bas;
    if (strcmp(type, "Rel")==0) return Rel;
    if (strcmp(type, "Abs")==0) return Abs;
    if (strcmp(type, "None")==0) return None;
    ERROR_MSG("Type invalide dans dictionnaire !\n");
}
