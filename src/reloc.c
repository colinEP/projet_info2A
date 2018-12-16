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

#include <analyse_synth.h>

#include <error.h>




LIST reloc_and_replace_etiq_by_dec_in_instr (LIST l_instr, LIST symb_table)
{
        // Ajout à la table des relocation + mise à jour de l'argument de la list_instr
        // /!\ /!\ WARNING ce qu'on appelle addend dans la foncrtion est en réalité le symbole sur quoi on fait la relocation

        LIST reloc_table_text = new_list();
        INSTR I;
        INSTR previous_inst = NULL;

        while (l_instr!= NULL){
            I = l_instr->element;
            //RELOC Re = calloc (1, sizeof(*Re));
            ETIQ Et;

            if ( ((ARG_INST)(I->arg1))->type == Label ) {
                RELOC Re = calloc (1, sizeof(*Re));

                Re->section = strdup(".text"); // NOTE on doit avoir une char* d'après le sujet  ??
                Re->adress = I->decalage; // valeur du décalage de l'APPEL de l'étiq (!= de sa définition)

                if ( (I->arg1)->etiq_def == 0 ) {               //etiq globale
                    if ( (I->Exp_Type_1) == Rel) {
                        printf("ERREUR LIGNE : %d\n", (I->lex)->nline);
                        ERROR_MSG("type relatif inadapté pour etiquette non definie !\n");
                    }
                    Re->addend = strdup((I->arg1)->val.char_chain);// stocke le NOM de l'etiquette car non def
                    free( (I->arg1)->val.char_chain ); // free de la str avant de remplacer par le decalage
                    (I->arg1)->val.entier = 0; // pour remplacer le nom de l'étiq par int = 0 car non def donc décalage nul
                    Re->type_r = find_R_type(I->Exp_Type_1);// a définir selon l'instruction !
                    reloc_table_text = add_to_end_list(reloc_table_text, Re);
                    (I->arg1)->type = Abs; //nécessairement car sinon erreur
                }

                else {
                    if ( (I->arg1)->etiq_def == 1 ) {               //etiq bien definie
                        Et = look_for_etiq_and_return(symb_table, (I->arg1)->val.char_chain);
                        free( (I->arg1)->val.char_chain ); // free de la str avant de remplacer par le decalage

                        //le cas Et->section == TEXT ne nécessite pas de relocation car nous sommes déjà dans la section Text
                        if (Et->section == PDATA) {                 // etiq def dans .data
                            if ( (I->Exp_Type_1) == Rel) ERROR_MSG("type relatif nécessite usage d'une étiquette définie dans la même section !\n");
                            Re->addend = strdup(".data");           //NOTE conversion en char*: est-ce vraiment utile pour la suite ?
                            Re->type_r = find_R_type(I->Exp_Type_1);// a définir selon l'instruction !
                            reloc_table_text = add_to_end_list(reloc_table_text, Re);
                            (I->arg1)->val.entier = Et->decalage;// remplacer char* nom etiq par valeur décalage de la DEFINITION de l'étiquette
                            (I->arg1)->type = Abs;
                            (I->arg1)->type = (I->Exp_Type_1);
                        }

                        if (Et->section == BSS){                    // etiq def dans .bss
                            if ( (I->Exp_Type_1) == Rel) ERROR_MSG("type relatif nécessite usage d'une étiquette définie dans la même section !\n");
                            Re->addend = strdup(".bss");             //NOTE conversion en char*: est-ce vraiment utile pour la suite ?
                            Re->type_r = find_R_type(I->Exp_Type_1);// a définir selon l'instruction !
                            reloc_table_text = add_to_end_list(reloc_table_text, Re);
                            (I->arg1)->val.entier = Et->decalage;// remplacer char* nom etiq par valeur décalage de la DEFINITION de l'étiquette
                            //(I->arg1)->type = Abs;
                            (I->arg1)->type = (I->Exp_Type_1);
                        }
                        if (Et->section == TEXT){                    // etiq def dans .text = "locale"
                            if ( (I->Exp_Type_1) == Rel){
                                (I->arg1)->val.entier = (Et->decalage-(I->decalage)) -4 ;// NOTE correct ?
                                (I->arg1)->type = Rel;

                                //free de Re qui n'est pas utilisé
                                free(Re->section);
                                free(Re->addend); // normalement deja vide
                                free(Re);
                            }
                            else {
                                Re->addend = strdup(".text");             //NOTE conversion en char*: est-ce vraiment utile pour la suite ?
                                Re->type_r = find_R_type(I->Exp_Type_1);// a définir selon l'instruction !
                                reloc_table_text = add_to_end_list(reloc_table_text, Re);
                                (I->arg1)->val.entier = Et->decalage;// remplacer char* nom etiq par valeur décalage de la DEFINITION de l'étiquette
                                (I->arg1)->type = (I->Exp_Type_1);
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
                RELOC Re = calloc (1, sizeof(*Re));


                Re->section = strdup(".text"); // NOTE on doit avoir une char* d'après le sujet  ??
                Re->adress = I->decalage; // valeur du décalage de l'APPEL de l'étiq (!= de sa définition)


                if ( (I->arg2)->etiq_def == 0 ) {               //etiq globale

                    if ( (I->Exp_Type_2) == Rel){
                        printf("ERREUR LIGNE : %d\n", (I->lex)->nline);
                        ERROR_MSG("type relatif inadapté pour etiquette non definie !\n");
                    }
                    Re->addend = strdup((I->arg2)->val.char_chain);// stocke le NOM de l'etiquette car non def
                    if ( ((I->arg2)->type == Label) || ((I->arg2)->type == Target)) {
                        free( (I->arg2)->val.char_chain ); // free de la str avant de remplacer par le decalage
                        (I->arg2)->val.entier = 0; // pour remplacer le nom de l'étiq par int = 0 car non def donc décalage nul

                    }
                    if (((I->arg2)->type == Bas_Target)||((I->arg2)->type == Target) ){

                        Re->type_r = find_R_type((I->arg2)->type);// a définir selon l'instruction !
                    }
                    if ((I->arg2)->type == Bas_Target) {
                        Et = look_for_etiq_and_return(symb_table, (I->arg2)->val.char_chain);
                        free( (I->arg3)->val.char_chain ); // free de la str avant de remplacer par le decalage
                        // WARNING free : est ce arg3 existe
                        I->arg3->val.entier = lower_16( Et->decalage );
                        I->arg3->type       = Imm;
                        I->Exp_Type_3       = Imm;

                        free( (I->arg2)->val.char_chain ); // free de la str avant de remplacer par le decalage
                        I->arg2->val.entier = 1; // $at
                        I->arg2->type       = Reg;
                        I->Exp_Type_2       = Reg;

                        I->nb_arg = 3; // INUTILE ??

                        // Vérification si offset est <0 (sur une réprésentation signée de 16 bits !!)
                        // PAS BESOIN DE VERIF car si etiq non déf => decalage=0 => offset=0 >= 0
                    }

                    if (((I->arg2)->type == Label) ){
                        Re->type_r = find_R_type(I->Exp_Type_2); // a définir selon l'instruction !
                    }
                    reloc_table_text = add_to_end_list(reloc_table_text, Re);
                    (I->arg2)->type = I->Exp_Type_2; //nécessairement !
                }


                else {
                    if ( (I->arg2)->etiq_def == 1 ) {               //etiq bien definie
                        printf("Ici on arg2 : %s et etiqdef : %d \n", (I->arg2)->val.char_chain, (I->arg2)->etiq_def);
                        Et = look_for_etiq_and_return(symb_table, (I->arg2)->val.char_chain);

                        //le cas Et->section == TEXT ne nécessite pas de relocation car nous sommes déjà dans la section Text

                        if ((I->arg2)->type == Label){
                            free( (I->arg2)->val.char_chain ); // free de la str avant de remplacer par le decalage
                            (I->arg2)->val.entier = Et->decalage;// remplacer char* nom etiq par valeur décalage de la DEFINITION de l'étiquette
                        }
                        if ((I->arg2)->type == Target){
                            free( (I->arg2)->val.char_chain ); // free de la str avant de remplacer par le decalage
                            (I->arg2)->val.entier = upper_16(Et->decalage);
                        }

                        if (Et->section == PDATA) {                 // etiq def dans .data
                            if ( (I->Exp_Type_2) == Rel) ERROR_MSG("type relatif nécessite usage d'une étiquette définie dans la même section !\n");
                            Re->addend = strdup(".data");           //NOTE conversion en char*: est-ce vraiment utile pour la suite ?
                            if (((I->arg2)->type == Bas_Target)||((I->arg2)->type == Target) ){
                                Re->type_r = find_R_type((I->arg2)->type);// a définir selon l'instruction !
                            }
                            if (((I->arg2)->type == Label) ){
                                Re->type_r = find_R_type(I->Exp_Type_2);// a définir selon l'instruction !
                            }
                            if ((I->arg2)->type == Bas_Target) {
                                Et = look_for_etiq_and_return(symb_table, (I->arg2)->val.char_chain);

                                free( (I->arg3)->val.char_chain ); // free de la str avant de remplacer par le decalage
                                // WARNING free : est ce arg3 existe
                                I->arg3->val.entier = lower_16( Et->decalage );
                                I->arg3->type       = Imm;
                                I->Exp_Type_3       = Imm;

                                free( (I->arg2)->val.char_chain ); // free de la str avant de remplacer par le decalage
                                I->arg2->val.entier = 1; // $at
                                I->arg2->type       = Reg;
                                I->Exp_Type_2       = Reg;

                                I->nb_arg = 3; // INUTILE ??

                                // Vérification si offset est <0 (sur une réprésentation signée de 16 bits !!)
                                if (I->arg3->val.entier & 0x00008000) {  //check du bit de poids fort sur 16 bits)
                                    if (previous_inst->arg2->type==Imm) {  //normalement inutile
                                        (previous_inst->arg2->val.entier)++;
                                    }
                                    else ERROR_MSG("ERR LINE %d : problème de gestion des pseudo-instructions LW et SW", (I->lex)->nline);
                                }
                            }
                            reloc_table_text = add_to_end_list(reloc_table_text, Re);
                            //(I->arg2)->type = Abs;
                            (I->arg2)->type = (I->Exp_Type_2);
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
                            if ((I->arg2)->type == Bas_Target) {
                                Et = look_for_etiq_and_return(symb_table, (I->arg2)->val.char_chain);

                                free( (I->arg3)->val.char_chain ); // free de la str avant de remplacer par le decalage
                                // WARNING free : est ce arg3 existe
                                I->arg3->val.entier = lower_16( Et->decalage );
                                I->arg3->type       = Imm;
                                I->Exp_Type_3       = Imm;

                                free( (I->arg2)->val.char_chain ); // free de la str avant de remplacer par le decalage
                                I->arg2->val.entier = 1; // $at
                                I->arg2->type       = Reg;
                                I->Exp_Type_2       = Reg;

                                I->nb_arg = 3; // INUTILE ??

                                // Vérification si offset est <0 (sur une réprésentation signée de 16 bits !!)
                                if (I->arg3->val.entier & 0x00008000) {  //check du bit de poids fort sur 16 bits)
                                    if (previous_inst->arg2->type==Imm) {  //normalement inutile
                                        (previous_inst->arg2->val.entier)++;
                                    }
                                    else ERROR_MSG("ERR LINE %d : problème de gestion des pseudo-instructions LW et SW", (I->lex)->nline);
                                }
                            }
                            reloc_table_text = add_to_end_list(reloc_table_text, Re);
                            //(I->arg2)->type = Abs;
                            (I->arg2)->type = (I->Exp_Type_2);
                        }

                        if (Et->section == TEXT){                    // etiq def dans .text = "locale"
                            if ( (I->Exp_Type_2) == Rel){
                                    //free( (I->arg2)->val.char_chain ); // free de la str avant de remplacer par le decalage
                                    (I->arg2)->val.entier = (Et->decalage-(I->decalage)) -4 ;// NOTE correct ?

                                    //free de Re qui n'est pas utilisé
                                    free(Re->section);
                                    free(Re->addend); // normalement deja vide
                                    free(Re);
                                }
                            else {
                                Re->addend = strdup(".text");             //NOTE conversion en char*: est-ce vraiment utile pour la suite ?
                                if (((I->arg2)->type == Bas_Target)||((I->arg2)->type == Target) ){
                                    Re->type_r = find_R_type((I->arg2)->type);// a définir selon l'instruction !

                                }
                                if (((I->arg2)->type == Label) ){
                                    Re->type_r = find_R_type(I->Exp_Type_2);// a définir selon l'instruction !
                                }
                                if ((I->arg2)->type == Bas_Target) {
                                    Et = look_for_etiq_and_return(symb_table, (I->arg2)->val.char_chain);

                                    free( (I->arg3)->val.char_chain ); // free de la str avant de remplacer par le decalage
                                    // WARNING free : est ce arg3 existe
                                    I->arg3->val.entier = lower_16( Et->decalage );
                                    I->arg3->type       = Imm;
                                    I->Exp_Type_3       = Imm;

                                    free( (I->arg2)->val.char_chain ); // free de la str avant de remplacer par le decalage
                                    I->arg2->val.entier = 1; // $at
                                    I->arg2->type       = Reg;
                                    I->Exp_Type_2       = Reg;

                                    I->nb_arg = 3; // INUTILE ??

                                    // Vérification si offset est <0 (sur une réprésentation signée de 16 bits !!)
                                    if (I->arg3->val.entier & 0x00008000) {  //check du bit de poids fort sur 16 bits)
                                        if (previous_inst->arg2->type==Imm) {  //normalement inutile
                                            (previous_inst->arg2->val.entier)++;
                                        }
                                        else ERROR_MSG("ERR LINE %d : problème de gestion des pseudo-instructions LW et SW", (I->lex)->nline);
                                    }
                                    printf(" BBBB %08lx\n", I->arg3->val.entier);
                                }
                                reloc_table_text = add_to_end_list(reloc_table_text, Re);
                                (I->arg2)->type = (I->Exp_Type_2);
                            }
                        }

                        (I->arg2)->type = (I->Exp_Type_2);

                    }
                    else {
                        printf("ERREUR LIGNE : %d\n", (I->lex)->nline);
                        ERROR_MSG("Valeur etiq_def invalide apres etiquette !\n");
                    }
                }
            }


            if ( ((ARG_INST)(I->arg3))->type == Label ) {
                RELOC Re = calloc (1, sizeof(*Re));


                Re->section = strdup(".text"); // NOTE on doit avoir une char* d'après le sujet  ??
                Re->adress = I->decalage; // valeur du décalage de l'APPEL de l'étiq (!= de sa définition)

                if ( (I->arg3)->etiq_def == 0 ) {               //etiq globale
                    if ( (I->Exp_Type_3) == Rel) {
                        printf("ERREUR LIGNE : %d\n", (I->lex)->nline);
                        ERROR_MSG("type relatif inadapté pour etiquette non definie !\n");
                    }
                    Re->addend = strdup((I->arg3)->val.char_chain);// stocke le NOM de l'etiquette car non def
                    free( (I->arg3)->val.char_chain ); // free de la str avant de remplacer par le decalage
                    (I->arg3)->val.entier = 0; // pour remplacer le nom de l'étiq par int = 0 car non def donc décalage nul
                    (I->arg3)->type = I->Exp_Type_3;
                    Re->type_r = find_R_type((I->arg3)->type);// a définir selon l'instruction !
                    reloc_table_text = add_to_end_list(reloc_table_text, Re);


                }

                else {
                    if ( (I->arg3)->etiq_def == 1 ) {               //etiq bien definie
                        Et = look_for_etiq_and_return(symb_table, (I->arg3)->val.char_chain);

                        //le cas Et->section == TEXT ne nécessite pas de relocation car nous sommes déjà dans la section Text

                        if (Et->section == PDATA) {                 // etiq def dans .data
                            if ( (I->Exp_Type_3) == Rel) ERROR_MSG("type relatif nécessite usage d'une étiquette définie dans la même sectin !\n");
                            Re->addend = strdup(".data");           //NOTE conversion en char*: est-ce vraiment utile pour la suite ?
                            (I->arg3)->type=I->Exp_Type_3;
                            Re->type_r = find_R_type((I->arg3)->type);// a définir selon l'instruction !
                            reloc_table_text = add_to_end_list(reloc_table_text, Re);
                            free( (I->arg3)->val.char_chain ); // free de la str avant de remplacer par le decalage
                            (I->arg3)->val.entier = Et->decalage;
                            //(I->arg3)->type = Abs;

                        }

                        if (Et->section == BSS){                    // etiq def dans .bss
                            if ( (I->Exp_Type_3) == Rel) ERROR_MSG("type relatif nécessite usage d'une étiquette définie dans la même sectin !\n");
                            Re->addend = strdup(".bss")  ;           //NOTE conversion en char*: est-ce vraiment utile pour la suite ?
                            (I->arg3)->type=I->Exp_Type_3;
                            Re->type_r = find_R_type((I->arg3)->type);// a définir selon l'instruction !
                            reloc_table_text = add_to_end_list(reloc_table_text, Re);
                            free( (I->arg3)->val.char_chain ); // free de la str avant de remplacer par le decalage
                            (I->arg3)->val.entier = Et->decalage;

                            /* S'il faut tester taille dec des symoble */

                            // printf("%d   %s    %d    %d\n", I->lex->lex_type, Et->name ,I->Exp_Type_3, Et->decalage);
                            // int tmp;
                            // scanf("%d", &tmp);
                            // (I->arg3)->val.entier = check_size_dec(Et->decalage, I->Exp_Type_3);
                            //
                            // long int check_size_dec(int decalage, inst_op_type exp_type) {
                            //     if      (exp_type==Imm) {
                            //         if ((convert_value <-32768)||(convert_value >32767)){
                            //             ERROR_MSG("Erreur, decalage etiquette trop grand pour  immediat over 16 bits long !\n");
                            //         }
                            //     }
                            //     else if (exp_type==rel) {
                            //
                            //     }
                            //     else if (exp_type==Abs) {
                            //
                            //     }
                            // }

                        }
                        if (Et->section == TEXT){                    // etiq def dans .text = "locale"
                            if ( (I->Exp_Type_3) == Rel){
                                    free( (I->arg3)->val.char_chain ); // free de la str avant de remplacer par le decalage
                                    (I->arg3)->val.entier = (Et->decalage-(I->decalage)) -4 ;
                                    //printf("Dans reloc_and_replace_etiq_by_dec_in_instr arg3 valeur du decalage relatif : %ld\n", (I->arg3)->val.entier);
                                    (I->arg3)->type = Rel;

                                    //free de Re qui n'est pas utilisé
                                    free(Re->section);
                                    free(Re->addend); // normalement deja vide
                                    free(Re);
                                }
                            else {
                                Re->addend = strdup(".text");             //NOTE conversion en char*: est-ce vraiment utile pour la suite ?
                                Re->type_r = find_R_type(I->Exp_Type_3);// a définir selon l'instruction !
                                reloc_table_text = add_to_end_list(reloc_table_text, Re);
                                free( (I->arg3)->val.char_chain ); // free de la str avant de remplacer par le decalage
                                (I->arg3)->val.entier = Et->decalage;// remplacer char* nom etiq par valeur décalage de la DEFINITION de l'étiquette
                                //printf("Dans reloc_and_replace_etiq_by_dec_in_instr arg3 on a dec = %d",(I->arg3)->val.entier );
                                (I->arg3)->type=I->Exp_Type_3;
                            }
                        }
                    }

                    else {
                    printf("ERREUR LIGNE : %d\n", (I->lex)->nline);
                    ERROR_MSG("Valeur etiq_def invalide apres etiquette !\n");
                    }
                }
            }
            previous_inst = I; //sert pour faire le +1 à $at si offset négatif
            l_instr = l_instr->next;
        }
        return reloc_table_text;
}


LIST reloc_and_replace_etiq_by_dec_in_data (LIST l_data, LIST symb_table)
{
    LIST reloc_table_data = new_list();
    DATA Da;
    while (l_data!= NULL){
        Da = l_data->element;
        if ( (Da->D)->type == LABEL) { // on ne considère que les etiq, d'où LABEL
            RELOC Re = calloc (1, sizeof(*Re));

            Re->section = strdup(".data");
            Re->adress = Da->decalage; // valeur du décalage de l'APPEL de l'étiq (!= de sa définition)
            Re->type_r = R_MIPS_32; // car on est dans .data
            (Da->D)->type = DEC_LABEL;
            if ( Da-> etiq_def == 0 ){
                Re->addend = strdup((Da->D)->val.LABEL);
                free( (char*) ((Da->D)->val.LABEL ) ); // free de la str avant de remplacer par le decalage
                (Da->D)->val.DEC_LABEL = 0;
                reloc_table_data = add_to_end_list(reloc_table_data, Re);
            }
            if ( Da-> etiq_def == 1 ){
                ETIQ Et = look_for_etiq_and_return(symb_table, (Da->D)->val.LABEL);

                if (Et->section == TEXT) {                 // etiq def dans .text
                    Re->addend = strdup(".text");           //NOTE conversion en char*: est-ce vraiment utile pour la suite ?
                    reloc_table_data = add_to_end_list(reloc_table_data, Re);
                }
                if (Et->section == BSS){                    // etiq def dans .bss
                    Re->addend = strdup(".bss");           //NOTE conversion en char*: est-ce vraiment utile pour la suite ?
                    reloc_table_data = add_to_end_list(reloc_table_data, Re);
                }
                if (Et->section == PDATA) {
                    Re->addend = strdup(".data");           //NOTE conversion en char*: est-ce vraiment utile pour la suite ?
                    reloc_table_data = add_to_end_list(reloc_table_data, Re);
                }
                free( (char*) ((Da->D)->val.LABEL ) ); // free de la str avant de remplacer par le decalage
                (Da->D)->val.DEC_LABEL = Et->decalage;
            }

        }
        l_data = l_data->next;
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
