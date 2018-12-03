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

#include <etiq.h>

#include <error.h>



int look_for_etiq( LIST symb_table, char* lexem) // renvoit 1 si trouvée, 0 sinon
 {
     int a;
     while (symb_table != NULL)
     {
         a = strcmp(((ETIQ) symb_table->element)->name, lexem); // conversion en ETIQ de (symb_table->element)
         if (a == 0)

         {
             return 1; //alors l'étiquette a été trouvée
         }
         symb_table = symb_table->next;
     }

     return 0; //étiquette non trouvée
 }

LIST add_to_symb_table(char* name_etiq, int dec, int line, int sect, int def, LIST symb_table)
{
    ETIQ etiq = calloc(1, sizeof(*etiq));
    etiq-> decalage = dec;
    etiq-> nline = line;
    etiq-> section = sect;
    etiq-> name = strdup(name_etiq);
    etiq-> def_in_file = def;
    symb_table = add_to_end_list(symb_table, etiq);
    return symb_table;
}


LIST reloc_and_replace_etiq_by_dec_in_instr (LIST l, LIST symb_table)
{
        // Ajout à la table des relocation + mise à jour de l'argument de la list_instr

        LIST reloc_table_text = new_list();
        INSTR I;
        while (l!= NULL){
            I = l->element;
            RELOC Re = calloc (1, sizeof(*Re));
            ETIQ Et;

            // WARNING et le cas == Target !?!?

            if ( ((ARG_INST)(I->arg1))->type == Label ) {

                Re->section = strdup(".text"); // NOTE on doit avoir une char* d'après le sujet  ??
                Re->adress = I->decalage; // valeur du décalage de l'APPEL de l'étiq (!= de sa définition)

                if ( (I->arg1)->etiq_def == 0 ) {               //etiq globale
                    Re->addend = strdup((I->arg1)->val.char_chain);// stocke le NOM de l'etiquette car non def
                    (I->arg1)->val.entier = 0; // pour remplacer le nom de l'étiq par int = 0 car non def donc décalage nul
                    Re->type_r = find_R_type((I->lex)->value);// a définir selon l'instruction !
                }

                else {
                    if ( (I->arg1)->etiq_def == 1 ) {               //etiq bien definie
                    Et = look_for_etiq_and_return(symb_table, (I->arg1)->val.char_chain);

                    //le cas Et->section == TEXT ne nécessite pas de relocation car nous sommes déjà dans la section Text

                    if (Et->section == PDATA) {                 // etiq def dans .data
                        Re->addend = strdup(".data");           //NOTE conversion en char*: est-ce vraiment utile pour la suite ?
                        Re->type_r = find_R_type((I->lex)->value);// a définir selon l'instruction !
                        reloc_table_text = add_to_end_list(reloc_table_text, Re);
                    }

                    if (Et->section == BSS){                    // etiq def dans .bss
                        Re->addend = strdup(".bss");             //NOTE conversion en char*: est-ce vraiment utile pour la suite ?
                        Re->type_r = find_R_type((I->lex)->value);// a définir selon l'instruction !
                        reloc_table_text = add_to_end_list(reloc_table_text, Re);
                    }

                    (I->arg1)->val.entier = Et->decalage;// remplacer char* nom etiq par valeur décalage de la DEFINITION de l'étiquette

                    }
                    else {
                    printf("ERREUR LIGNE : %d\n", (I->lex)->nline);
                    ERROR_MSG("Valeur etiq_def invalide apres etiquette !\n");
                    }
                }

            }
            if ( ((ARG_INST)(I->arg2))->type == Label ) {
                Re->section = strdup(".text"); // NOTE on doit avoir une char* d'après le sujet  ??
                Re->adress = I->decalage; // valeur du décalage de l'APPEL de l'étiq (!= de sa définition)

                if ( (I->arg2)->etiq_def == 0 ) {               //etiq globale
                    Re->addend = strdup((I->arg2)->val.char_chain);// stocke le NOM de l'etiquette car non def
                    (I->arg2)->val.entier = 0; // pour remplacer le nom de l'étiq par int = 0 car non def donc décalage nul
                    Re->type_r = find_R_type((I->lex)->value);// a définir selon l'instruction !
                }

                else {
                    if ( (I->arg2)->etiq_def == 1 ) {               //etiq bien definie
                    Et = look_for_etiq_and_return(symb_table, (I->arg2)->val.char_chain);

                    //le cas Et->section == TEXT ne nécessite pas de relocation car nous sommes déjà dans la section Text

                    if (Et->section == PDATA) {                 // etiq def dans .data
                        Re->addend = strdup(".data");           //NOTE conversion en char*: est-ce vraiment utile pour la suite ?
                        Re->type_r = find_R_type((I->lex)->value);// a définir selon l'instruction !
                        reloc_table_text = add_to_end_list(reloc_table_text, Re);
                    }

                    if (Et->section == BSS){                    // etiq def dans .bss
                        Re->addend = strdup(".bss") ;            //NOTE conversion en char*: est-ce vraiment utile pour la suite ?
                        Re->type_r = find_R_type((I->lex)->value);// a définir selon l'instruction !
                        reloc_table_text = add_to_end_list(reloc_table_text, Re);
                    }

                    (I->arg2)->val.entier = Et->decalage;// remplacer char* nom etiq par valeur décalage de la DEFINITION de l'étiquette

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
                    Re->addend = strdup((I->arg3)->val.char_chain);// stocke le NOM de l'etiquette car non def
                    (I->arg3)->val.entier = 0; // pour remplacer le nom de l'étiq par int = 0 car non def donc décalage nul
                    Re->type_r = find_R_type((I->lex)->value);// a définir selon l'instruction !
                }

                else {
                    if ( (I->arg3)->etiq_def == 1 ) {               //etiq bien definie
                    Et = look_for_etiq_and_return(symb_table, (I->arg3)->val.char_chain);

                    //le cas Et->section == TEXT ne nécessite pas de relocation car nous sommes déjà dans la section Text

                    if (Et->section == PDATA) {                 // etiq def dans .data
                        Re->addend = strdup(".data");           //NOTE conversion en char*: est-ce vraiment utile pour la suite ?
                        Re->type_r = find_R_type((I->lex)->value);// a définir selon l'instruction !
                        reloc_table_text = add_to_end_list(reloc_table_text, Re);
                    }

                    if (Et->section == BSS){                    // etiq def dans .bss
                        Re->addend = strdup(".bss")  ;           //NOTE conversion en char*: est-ce vraiment utile pour la suite ?
                        Re->type_r = find_R_type((I->lex)->value);// a définir selon l'instruction !
                        reloc_table_text = add_to_end_list(reloc_table_text, Re);
                    }

                    (I->arg3)->val.entier = Et->decalage;// remplacer char* nom etiq par valeur décalage de la DEFINITION de l'étiquette
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


ETIQ look_for_etiq_and_return( LIST symb_table, char* lexem)
 {
     int a;
     while (symb_table != NULL)
     {
         a = strcmp(((ETIQ) (symb_table->element))->name, lexem);
         if (a == 0) {
             return symb_table->element;
         }
         symb_table = symb_table->next;
     }
     ERROR_MSG("Probleme d'etiquette non trouvee dans table des symboles !\n");
 }



reloc_type find_R_type(char* instruction) // utilisée UNIQUEMENT pour appel dans .text
{
    if ( (strcmp(instruction, "J")==0) || (strcmp(instruction, "JAL")==0) || (strcmp(instruction, "JR")==0) ) {
        return R_MIPS_26;
    }
    // HOHO ! Y a t il d'autres cas possibles ? WARNING
    if ( strcmp(instruction, "LUI") == 0)  {
        return R_MIPS_HI16;
    }
    if ( (strcmp(instruction, "LW") == 0)|| (strcmp(instruction, "SW")==0))  {
        return R_MIPS_LO16;
    }
    else{
         ERROR_MSG("Relocation impossible après une telle instruction !\n"); //car logiquement on ne peut pas être dans .data ni .bss
    }
}
