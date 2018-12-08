#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <strings.h>

#include <fct_analyse_1.h>
#include <global.h>
#include <notify.h>
#include <lex.h>
#include <queue_list.h>
#include <test.h>
#include <etiq.h>
#include <reloc.h>

#include <error.h>




LIST reloc_and_replace_etiq_by_dec_in_instr (LIST l, LIST symb_table)
{
        // Ajout à la table des relocation + mise à jour de l'argument de la list_instr

        LIST reloc_table_text = new_list();
        INSTR I;
        while (l!= NULL){
            I = l->element;
            RELOC Re = calloc (1, sizeof(*Re));
            ETIQ Et;


            if ( ((ARG_INST)(I->arg1))->type == Label ) {


                Re->section = strdup(".text"); // NOTE on doit avoir une char* d'après le sujet  ??
                Re->adress = I->decalage; // valeur du décalage de l'APPEL de l'étiq (!= de sa définition)

                if ( (I->arg1)->etiq_def == 0 ) {               //etiq globale
                    if ( (I->Exp_Type_1) == Rel) {
                        printf("ERREUR LIGNE : %d\n", (I->lex)->nline);
                        ERROR_MSG("type relatif inadapté pour etiquette non definie !\n");
                    }
                    Re->addend = strdup((I->arg1)->val.char_chain);// stocke le NOM de l'etiquette car non def
                    (I->arg1)->val.entier = 0; // pour remplacer le nom de l'étiq par int = 0 car non def donc décalage nul
                    Re->type_r = find_R_type(I->Exp_Type_1);// a définir selon l'instruction !
                    reloc_table_text = add_to_end_list(reloc_table_text, Re);
                    (I->arg1)->type = Abs; //nécessairement car sinon erreur
                }

                else {
                    if ( (I->arg1)->etiq_def == 1 ) {               //etiq bien definie
                    Et = look_for_etiq_and_return(symb_table, (I->arg1)->val.char_chain);

                    //le cas Et->section == TEXT ne nécessite pas de relocation car nous sommes déjà dans la section Text

                    if (Et->section == PDATA) {                 // etiq def dans .data
                        if ( (I->Exp_Type_1) == Rel) ERROR_MSG("type relatif nécessite usage d'une étiquette définie dans la même section !\n");
                        Re->addend = strdup(".data");           //NOTE conversion en char*: est-ce vraiment utile pour la suite ?
                        Re->type_r = find_R_type(I->Exp_Type_1);// a définir selon l'instruction !
                        reloc_table_text = add_to_end_list(reloc_table_text, Re);
                        (I->arg1)->val.entier = Et->decalage;// remplacer char* nom etiq par valeur décalage de la DEFINITION de l'étiquette
                        (I->arg1)->type = Abs;
                    }

                    if (Et->section == BSS){                    // etiq def dans .bss
                        if ( (I->Exp_Type_1) == Rel) ERROR_MSG("type relatif nécessite usage d'une étiquette définie dans la même section !\n");
                        Re->addend = strdup(".bss");             //NOTE conversion en char*: est-ce vraiment utile pour la suite ?
                        Re->type_r = find_R_type(I->Exp_Type_1);// a définir selon l'instruction !
                        reloc_table_text = add_to_end_list(reloc_table_text, Re);
                        (I->arg1)->val.entier = Et->decalage;// remplacer char* nom etiq par valeur décalage de la DEFINITION de l'étiquette
                        (I->arg1)->type = Abs;
                    }
                    if (Et->section == TEXT){                    // etiq def dans .text = "locale"
                        if ( (I->Exp_Type_1) == Rel){
                                (I->arg1)->val.entier = (Et->decalage-(I->decalage)) -4 ;// NOTE correct ?
                                (I->arg1)->type = Rel;
                            }
                        else {
                            Re->addend = strdup(".text");             //NOTE conversion en char*: est-ce vraiment utile pour la suite ?
                            Re->type_r = find_R_type(I->Exp_Type_1);// a définir selon l'instruction !
                            reloc_table_text = add_to_end_list(reloc_table_text, Re);
                            (I->arg1)->val.entier = Et->decalage;// remplacer char* nom etiq par valeur décalage de la DEFINITION de l'étiquette
                            (I->arg1)->type = Abs;
                        }
                    }
                    }
                    else {
                        printf("ERREUR LIGNE : %d\n", (I->lex)->nline);
                        ERROR_MSG("Valeur etiq_def invalide apres etiquette !\n");
                    }
                }

            }

            // En arg2 uniquement, cas pseudo instr !
            if ( (((ARG_INST)(I->arg2))->type == Label )||( ((ARG_INST)(I->arg2))->type == Target) || ( ((ARG_INST)(I->arg2))->type == Bas_Target) ) {

                Re->section = strdup(".text"); // NOTE on doit avoir une char* d'après le sujet  ??
                Re->adress = I->decalage; // valeur du décalage de l'APPEL de l'étiq (!= de sa définition)


                if ( (I->arg2)->etiq_def == 0 ) {               //etiq globale

                    if ( (I->Exp_Type_2) == Rel){
                        printf("ERREUR LIGNE : %d\n", (I->lex)->nline);
                        ERROR_MSG("type relatif inadapté pour etiquette non definie !\n");
                    }
                    Re->addend = strdup((I->arg2)->val.char_chain);// stocke le NOM de l'etiquette car non def
                    if ( ((I->arg2)->type == Label) || ((I->arg2)->type == Target)) {
                         (I->arg2)->val.entier = 0; // pour remplacer le nom de l'étiq par int = 0 car non def donc décalage nul

                     }
                    if ((I->arg2)->type == Bas_Target){
                        (I->arg2)->val.entier = (I->arg1)->val.entier; // car alors on somme $rt + 0(=adresse d'une etiq non def)
                    }
                    if (((I->arg2)->type == Bas_Target)||((I->arg2)->type == Target) ){
                        Re->type_r = find_R_type((I->arg2)->type);// a définir selon l'instruction !
                    }
                    if (((I->arg2)->type == Label) ){
                        Re->type_r = find_R_type(I->Exp_Type_2); // a définir selon l'instruction !
                    }
                    reloc_table_text = add_to_end_list(reloc_table_text, Re);
                    (I->arg2)->type = Abs;
                }


                else {
                    if ( (I->arg2)->etiq_def == 1 ) {               //etiq bien definie
                        printf("Ici on arg2 : %s et etiqdef : %d \n", (I->arg2)->val.char_chain, (I->arg2)->etiq_def);
                        Et = look_for_etiq_and_return(symb_table, (I->arg2)->val.char_chain);

                        //le cas Et->section == TEXT ne nécessite pas de relocation car nous sommes déjà dans la section Text

                        if (Et->section == PDATA) {                 // etiq def dans .data
                            if ( (I->Exp_Type_2) == Rel) ERROR_MSG("type relatif nécessite usage d'une étiquette définie dans la même section !\n");
                            Re->addend = strdup(".data");           //NOTE conversion en char*: est-ce vraiment utile pour la suite ?
                            if (((I->arg2)->type == Bas_Target)||((I->arg2)->type == Target) ){
                                Re->type_r = find_R_type((I->arg2)->type);// a définir selon l'instruction !
                            }
                            if (((I->arg2)->type == Label) ){
                                Re->type_r = find_R_type(I->Exp_Type_2);// a définir selon l'instruction !
                            }
                            reloc_table_text = add_to_end_list(reloc_table_text, Re);
                            (I->arg2)->type = Abs;
                        }

                        if (Et->section == BSS){                    // etiq def dans .bss
                            if ( (I->Exp_Type_2) == Rel) ERROR_MSG("type relatif nécessite usage d'une étiquette définie dans la même section !\n");
                            Re->addend = strdup(".bss") ;            //NOTE conversion en char*: est-ce vraiment utile pour la suite ?
                            if (((I->arg2)->type == Bas_Target)||((I->arg2)->type == Target) ){
                                Re->type_r = find_R_type((I->arg2)->type);// a définir selon l'instruction !
                            }
                            if (((I->arg2)->type == Label) ){
                                Re->type_r = find_R_type(I->Exp_Type_2);// a définir selon l'instruction !
                            }
                            reloc_table_text = add_to_end_list(reloc_table_text, Re);
                            (I->arg2)->type = Abs;
                        }

                        if (Et->section == TEXT){                    // etiq def dans .text = "locale"
                            if ( (I->Exp_Type_2) == Rel){
                                    (I->arg2)->val.entier = (Et->decalage-(I->decalage)) -4 ;// NOTE correct ?
                                }
                            else {
                                Re->addend = strdup(".text");             //NOTE conversion en char*: est-ce vraiment utile pour la suite ?
                                if (((I->arg2)->type == Bas_Target)||((I->arg2)->type == Target) ){
                                    Re->type_r = find_R_type((I->arg2)->type);// a définir selon l'instruction !
                                }
                                if (((I->arg2)->type == Label) ){
                                    Re->type_r = find_R_type(I->Exp_Type_2);// a définir selon l'instruction !
                                }
                                reloc_table_text = add_to_end_list(reloc_table_text, Re);
                            }
                        }

                        if ((I->arg2)->type == Label){
                            (I->arg2)->val.entier = Et->decalage;// remplacer char* nom etiq par valeur décalage de la DEFINITION de l'étiquette
                        }
                        if ((I->arg2)->type == Target){
                            (I->arg2)->val.entier = upper_16(Et->decalage);
                        }
                        if ((I->arg2)->type == Bas_Target){
                            (I->arg2)->val.entier = lower_16(Et->decalage + (I->arg1)->val.entier );
                        }
                        if ( (I->Exp_Type_2) == Rel) (I->arg2)->type = Rel;
                        else (I->arg2)->type = Abs;

                    }
                    else {
                        printf("ERREUR LIGNE : %d\n", (I->lex)->nline);
                        ERROR_MSG("Valeur etiq_def invalide apres etiquette !\n");
                    }
                }
            }


            if ( ((ARG_INST)(I->arg3))->type == Label ) {

                Re->section = strdup(".text"); // NOTE on doit avoir une char* d'après le sujet  ??
                Re->adress = I->decalage; // valeur du décalage de l'APPEL de l'étiq (!= de sa définition)

                if ( (I->arg3)->etiq_def == 0 ) {               //etiq globale
                    if ( (I->Exp_Type_3) == Rel) {
                        printf("ERREUR LIGNE : %d\n", (I->lex)->nline);
                        ERROR_MSG("type relatif inadapté pour etiquette non definie !\n");
                    }
                    Re->addend = strdup((I->arg3)->val.char_chain);// stocke le NOM de l'etiquette car non def
                    (I->arg3)->val.entier = 0; // pour remplacer le nom de l'étiq par int = 0 car non def donc décalage nul
                    Re->type_r = find_R_type((I->arg3)->type);// a définir selon l'instruction !
                    reloc_table_text = add_to_end_list(reloc_table_text, Re);
                    (I->arg3)->type = Abs;
                }

                else {
                    if ( (I->arg3)->etiq_def == 1 ) {               //etiq bien definie
                    Et = look_for_etiq_and_return(symb_table, (I->arg3)->val.char_chain);

                    //le cas Et->section == TEXT ne nécessite pas de relocation car nous sommes déjà dans la section Text

                    if (Et->section == PDATA) {                 // etiq def dans .data
                        if ( (I->Exp_Type_3) == Rel) ERROR_MSG("type relatif nécessite usage d'une étiquette définie dans la même sectin !\n");
                        Re->addend = strdup(".data");           //NOTE conversion en char*: est-ce vraiment utile pour la suite ?
                        Re->type_r = find_R_type((I->arg3)->type);// a définir selon l'instruction !
                        reloc_table_text = add_to_end_list(reloc_table_text, Re);
                        (I->arg3)->val.entier = Et->decalage;
                        (I->arg3)->type = Abs;
                    }

                    if (Et->section == BSS){                    // etiq def dans .bss
                        if ( (I->Exp_Type_3) == Rel) ERROR_MSG("type relatif nécessite usage d'une étiquette définie dans la même sectin !\n");
                        Re->addend = strdup(".bss")  ;           //NOTE conversion en char*: est-ce vraiment utile pour la suite ?
                        Re->type_r = find_R_type((I->arg3)->type);// a définir selon l'instruction !
                        reloc_table_text = add_to_end_list(reloc_table_text, Re);
                        (I->arg3)->val.entier = Et->decalage;
                        (I->arg3)->type = Abs;
                    }
                    if (Et->section == TEXT){                    // etiq def dans .text = "locale"
                        if ( (I->Exp_Type_3) == Rel){
                                (I->arg3)->val.entier = (Et->decalage-(I->decalage)) -4 ;// NOTE correct ?
                                (I->arg3)->type = Rel;
                            }
                        else {
                            Re->addend = strdup(".text");             //NOTE conversion en char*: est-ce vraiment utile pour la suite ?
                            Re->type_r = find_R_type(I->Exp_Type_3);// a définir selon l'instruction !
                            reloc_table_text = add_to_end_list(reloc_table_text, Re);
                            (I->arg3)->val.entier = Et->decalage;// remplacer char* nom etiq par valeur décalage de la DEFINITION de l'étiquette
                            (I->arg3)->type = Abs;
                        }
                    }
                    }

                    else {
                    printf("ERREUR LIGNE : %d\n", (I->lex)->nline);
                    ERROR_MSG("Valeur etiq_def invalide apres etiquette !\n");
                    }
                }
            }
            l = l->next;
        }
        return reloc_table_text;
}


LIST reloc_and_replace_etiq_by_dec_in_data (LIST l, LIST symb_table)
{
    LIST reloc_table_data = new_list();
    DATA Da;
    while (l!= NULL){
        RELOC Re = calloc (1, sizeof(*Re));
        Da = l->element;
        if ( (Da->D)->type == LABEL) { // on ne considère que les etiq, d'où LABEL

            Re->section = strdup(".data");
            Re->adress = Da->decalage; // valeur du décalage de l'APPEL de l'étiq (!= de sa définition)
            Re->type_r = R_MIPS_32; // car on est dans .data
            (Da->D)->type = DEC_LABEL;
            if ( Da-> etiq_def == 0 ){
                (Da->D)->val.DEC_LABEL = 0;
                reloc_table_data = add_to_end_list(reloc_table_data, Re);
            }
            if ( Da-> etiq_def == 1 ){
                ETIQ Et = look_for_etiq_and_return(symb_table, (Da->D)->val.LABEL);

                if (Et->section == TEXT) {                 // etiq def dans .data
                    Re->addend = strdup(".text");           //NOTE conversion en char*: est-ce vraiment utile pour la suite ?
                    reloc_table_data = add_to_end_list(reloc_table_data, Re);
                }
                if (Et->section == BSS){                    // etiq def dans .bss
                    Re->addend = strdup(".bss")  ;           //NOTE conversion en char*: est-ce vraiment utile pour la suite ?
                    reloc_table_data = add_to_end_list(reloc_table_data, Re);
                }
                (Da->D)->val.DEC_LABEL = Et->decalage;
            }

        }
        l = l->next;
    }
    return reloc_table_data ;

}

// PAS D'ETIQUETTE DANS .BSS TODO ERROR MESSAGE



reloc_type find_R_type(inst_op_type type) // utilisée UNIQUEMENT pour appel dans .text
{
    if ( type == Abs ) {
        return R_MIPS_26;
    }

    if ( type == Target)  {
        return R_MIPS_HI16;
    }
    if ( (type == Bas_Target) || ( type == Imm ))  {
        return R_MIPS_LO16;
    }
    else{
        printf("Ici type vaut %d\n", type);
         ERROR_MSG("Relocation impossible après une telle instruction !\n"); //car logiquement on ne peut pas être dans .data ni .bss
    }
}
