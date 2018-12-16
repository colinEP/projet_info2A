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
#include <dictionnaire.h>
#include <in_binar.h>

#include <error.h>



int* instr_in_binar(LIST list_instr, int size_list, LIST dictionnaire)
{
    int* tab_instr_binar = calloc (size_list, sizeof(int));
    int i=0;

    // parcourir la liste
    while (list_instr != NULL){
        unsigned int binar_value = 0;
        INSTR I = list_instr->element;
        char* instruction =  strdup( ((LEXEM)(I->lex))->value );
        instruction = put_in_uppercase (instruction);
        int opc;
        char * from_25_21;
        char * from_20_16;
        char * from_15_11;
        char * from_10_6;
        char * from_5_0;
        int val_1, val_2,val_3,val_4,val_5;
        opc = look_for_instr_and_return_binar_info(dictionnaire,instruction, &from_25_21, &from_20_16, &from_15_11, &from_10_6, &from_5_0); // fonctionne !
        // ajouter chaque instruction dans le tableau : tab_instr_bin[i]
        // méthode ajout :
            // opcode : easy, le laisser tel quel
            // if un entier laisser tel quel
            // if _ ne rien faire
            // if a1, a2 ou a3 ajouter la valeur stockée dans la liste:
            // l'ajout se fera sur une taille qui dépendra du type de l'argument (5 bits si Reg, 16 bits si c'est un Rel ou 26 si c'est un Abs, etc. )

        /* MISE EN COMM POUR LE DEBUG */
//        printf("\n%s \nopcode : %d \nfrom_25_21 : %s  \nfrom_20_16 : %s     \nfrom_15_11: %s\nfrom_10_6 : %s \nfrom_5_0 : %s\n",instruction,opc, from_25_21,from_20_16,from_15_11,from_10_6,from_5_0 );
        //////////////////////////////////////

        // ------------------------------------
        // opcode:
        binar_value = binar_value | ( (opc << 26) & 0xFC000000 ); // OK
        // printf("Je suis ici ! \n");
        // printf("ici binar_value vaut : %d \n",binar_value);
        // printf("Je suis ici ! \n");

        // -------------------------------------
        // from_25_21
        if (strcmp(from_25_21, "_")==0){
            // ne rien faire : utilité symbolique
        }

        if (from_25_21[0] == 'a'){
            // aller chercher l'argument correspondant
            // check le TYPE : si != Reg et != Sa-> error car seuls 5 bits sont dispos --> logiquement cela a déjà été vérifié précédemment !

            if (from_25_21[1] == '1'){
                if (((I->arg1)->type != Reg)&&((I->arg1)->type != Sa)) ERROR_MSG("Erreur, type de longeur 5 bits attendu!\n");
                val_1 = (I->arg1)->val.entier;
            }
            if (from_25_21[1] == '2'){
                if (((I->arg2)->type != Reg)&&((I->arg2)->type != Sa)) ERROR_MSG("Erreur, type de longeur 5 bits attendu !\n");
                val_1 = (I->arg2)->val.entier;
            }
            if (from_25_21[1] == '3'){
                if (((I->arg3)->type != Reg)&&((I->arg3)->type != Sa)) ERROR_MSG("Erreur, type de longeur 5 bits attendu !\n");
                val_1 = (I->arg3)->val.entier;
            }
            // ajouter la valeur
            binar_value = binar_value | ( (val_1 << 21) & 0x03E00000 );
        }

        if (isdigit(from_25_21[0])){
            // si ne commence pas par 'a' ni '_' alors c'est un nombre car c'est ainsi que nous avons fait le dico
            // on suppose que de par l'écriture du dico sa valeur < 5 bits
            val_1 = strtol(from_25_21, NULL, 0);
            binar_value = binar_value | ( ((int) val_1 << 21) & 0x03E00000 );
        }
        //OK
        //printf("ici binar_value vaut : %d",binar_value);

        // -------------------------------------
        // from_20_16
        if (from_20_16[0] == 'a'){
            // aller chercher l'argument correspondant
            // check le TYPE : si != Reg et != Sa-> error car seuls 5 bits sont dispos --> logiquement cela a déjà été vérifié précédemment !

            // dans ce cas, IMPOSSIBLE d'avoir un truc plus grand que 5 bits aussi

            if (from_20_16[1] == '1'){
                if (((I->arg1)->type != Reg)&&((I->arg1)->type != Sa)) ERROR_MSG("Erreur, type de longeur 5 bits attendu!\n");
                val_2 = (I->arg1)->val.entier;
            }
            if (from_20_16[1] == '2'){
                if (((I->arg2)->type != Reg)&&((I->arg2)->type != Sa)) ERROR_MSG("Erreur, type de longeur 5 bits attendu !\n");
                val_2 = (I->arg2)->val.entier;

            }
            if (from_20_16[1] == '3'){
                if (((I->arg3)->type != Reg)&&((I->arg3)->type != Sa)) ERROR_MSG("Erreur, type de longeur 5 bits attendu !\n");
                val_2 = (I->arg3)->val.entier;

            }
            // ajouter la valeur
            binar_value = binar_value | ( (val_2 << 16) & 0x001F0000 );
        }

        if (isdigit(from_20_16[0])){
            // si ne commence pas par 'a' ni '_' alors c'est un nombre car c'est ainsi que nous avons fait le dico
            // on sait que de par l'écriture du dico sa valeur < 5 < 16 bits
            val_2 = strtol(from_20_16, NULL, 0);
            binar_value = binar_value | ( ((int) val_2 << 16) & 0x001F0000 );

        }

        // -------------------------------------------
        // from_15_11
        if (from_15_11[0] == 'a'){
            // aller chercher l'argument correspondant
            // check le TYPE : si != Reg et != Sa-> error car seuls 5 bits sont dispos --> logiquement cela a déjà été vérifié précédemment !

            // dans ce cas, IMPOSSIBLE d'avoir un truc plus grand que 5 bits aussi

            if (from_15_11[1] == '1'){
                if (((I->arg1)->type != Reg)&&((I->arg1)->type != Sa)) ERROR_MSG("Erreur, type de longeur 5 bits attendu!\n");
                val_3 = (I->arg1)->val.entier;
            }
            if (from_15_11[1] == '2'){
                if (((I->arg2)->type != Reg)&&((I->arg2)->type != Sa)) ERROR_MSG("Erreur, type de longeur 5 bits attendu !\n");
                val_3 = (I->arg2)->val.entier;
            }
            if (from_15_11[1] == '3'){
                if (((I->arg3)->type != Reg)&&((I->arg3)->type != Sa)) ERROR_MSG("Erreur, type de longeur 5 bits attendu !\n");
                val_3 = (I->arg3)->val.entier;
            }
            // ajouter la valeur
            binar_value = binar_value | ( (val_3 << 11) & 0x0000F800 );
        }

        if (isdigit(from_15_11[0])){
            // si ne commence pas par 'a' ni '_' alors c'est un nombre car c'est ainsi que nous avons fait le dico
            // on sait que de par l'écriture du dico sa valeur < 5 < 16 bits
            val_3 = strtol(from_15_11, NULL, 0);
            binar_value = binar_value | ( ((int) val_3 << 11) & 0x0000F800 );
        }

        // -------------------------------------------
        // from_10_6
        if (from_10_6[0] == 'a'){
            // aller chercher l'argument correspondant
            // check le TYPE : si != Reg et != Sa-> error car seuls 5 bits sont dispos --> logiquement cela a déjà été vérifié précédemment !

            // dans ce cas, IMPOSSIBLE d'avoir un truc plus grand que 5 bits aussi

            if (from_10_6[1] == '1'){
                if (((I->arg1)->type != Reg)&&((I->arg1)->type != Sa)) ERROR_MSG("Erreur, type de longeur 5 bits attendu!\n");
                val_4 = (I->arg1)->val.entier;
            }
            if (from_10_6[1] == '2'){
                if (((I->arg2)->type != Reg)&&((I->arg2)->type != Sa)) ERROR_MSG("Erreur, type de longeur 5 bits attendu !\n");
                val_4 = (I->arg2)->val.entier;
            }
            if (from_10_6[1] == '3'){
                if (((I->arg3)->type != Reg)&&((I->arg3)->type != Sa)) ERROR_MSG("Erreur, type de longeur 5 bits attendu !\n");
                val_4 = (I->arg3)->val.entier;
            }
            // ajouter la valeur
            binar_value = binar_value | ( (val_4 << 6) & 0x000007C0 );
        }

        if (isdigit(from_10_6[0])){
            // si ne commence pas par 'a' ni '_' alors c'est un nombre car c'est ainsi que nous avons fait le dico
            // on sait que de par l'écriture du dico sa valeur < 5 < 16 bits
            val_4 = strtol(from_10_6, NULL, 0);
            binar_value = binar_value | ( ((int) val_4 << 6) & 0x000007C0 );
        }
        // -------------------------------------------
        // from_5_0
        if (from_5_0[0] == 'a'){

            // aller chercher l'argument correspondant
            if (from_5_0[1] == '1'){
                // check le TYPE :
                if (((I->arg1)->type == Reg)||((I->arg1)->type == Sa)) {
                    val_5 = (I->arg1)->val.entier;
                    binar_value = binar_value | ( val_5 & 0x0000003F );
                }
                if (((I->arg1)->type == Imm)||((I->arg1)->type == Bas)||((I->arg1)->type == Rel)) {
                    if( strcmp(from_10_6, "_") || strcmp(from_15_11, "_")){ // càd que juste avant on n'a pas laissé de "place" pour mettre 16 bits
                        ERROR_MSG("Erreur, 16 bits libres sont nécessaires pour stocker argument !\n");
                    }
                    val_5 = (I->arg1)->val.entier;
                    val_5 =  val_5 >> 2; // DECALAGE pour avoir 18 bits sur 16
                    binar_value = binar_value | ( val_5 & 0x0000FFFF );
                }
                if ((I->arg1)->type == Abs) {
                    if( strcmp(from_10_6, "_") || strcmp(from_15_11, "_")|| strcmp(from_20_16, "_")|| strcmp(from_25_21, "_")){ // càd que juste avant on n'a pas laissé de "place" pour mettre 26 bits
                        ERROR_MSG("Erreur, 26 bits libres sont nécessaires pour stocker argument !\n");
                    }
                    val_5 = (I->arg1)->val.entier;
                    val_5 =  val_5 >> 2; // DECALAGE pour avoir 28 bits sur 26
                    binar_value = binar_value | ( val_5 & 0x03FFFFFF );
                }
            }
            if (from_5_0[1] == '2'){
                // check le TYPE :
                if (((I->arg2)->type == Reg)||((I->arg2)->type == Sa)) {
                    val_5 = (I->arg2)->val.entier;
                    binar_value = binar_value | ( val_5 & 0x0000003F );
                }
                if (((I->arg2)->type == Imm)||((I->arg2)->type == Bas)||((I->arg2)->type == Rel)) {
                    if( strcmp(from_10_6, "_") || strcmp(from_15_11, "_")){ // càd que juste avant on n'a pas laissé de "place" pour mettre 16 bits
                        ERROR_MSG("Erreur, 16 bits libres sont nécessaires pour stocker argument !\n");
                    }
                    val_5 = (I->arg2)->val.entier;
                    val_5 =  val_5 >> 2; // DECALAGE pour avoir 18 bits sur 16
                    binar_value = binar_value | ( val_5 & 0x0000FFFF );
                }
                if ((I->arg2)->type == Abs) {
                    if( strcmp(from_10_6, "_") || strcmp(from_15_11, "_")|| strcmp(from_20_16, "_")|| strcmp(from_25_21, "_")){ // càd que juste avant on n'a pas laissé de "place" pour mettre 26 bits
                        ERROR_MSG("Erreur, 26 bits libres sont nécessaires pour stocker argument !\n");
                    }
                    val_5 = (I->arg2)->val.entier;
                    val_5 =  val_5 >> 2; // DECALAGE pour avoir 28 bits sur 26
                    binar_value = binar_value | ( val_5 & 0x03FFFFFF );
                }
            }
            if (from_5_0[1] == '3'){
                // check le TYPE :
                if (((I->arg3)->type == Reg)||((I->arg3)->type == Sa)) {
                    val_5 = (I->arg3)->val.entier;
                    // ajouter la valeur
                    binar_value = binar_value | ( val_5 & 0x0000003F );
                }
                if (((I->arg3)->type == Imm)||((I->arg3)->type == Bas)||((I->arg3)->type == Rel)) {
                    if( strcmp(from_10_6, "_") || strcmp(from_15_11, "_")){ // càd que juste avant on n'a pas laissé de "place" pour mettre 16 bits
                        ERROR_MSG("Erreur, 16 bits libres sont nécessaires pour stocker argument !\n");
                    }
                    val_5 = (I->arg3)->val.entier;
                    //printf("from 5_0 a3 vaut avant décalage : %d\n",val_5 );
                    if ((I->arg3)->type == Rel) val_5 =  val_5 >> 2; // DECALAGE pour avoir 18 bits sur 16
                    //printf("from 5_0 a3 vaut après décalage : %d\n",val_5 );
                    // ajouter la valeur
                    binar_value = binar_value | ( val_5 & 0x0000FFFF );
                }
                if ((I->arg3)->type == Abs) {
                    if( strcmp(from_10_6, "_") || strcmp(from_15_11, "_")|| strcmp(from_20_16, "_")|| strcmp(from_25_21, "_")){ // càd que juste avant on n'a pas laissé de "place" pour mettre 26 bits
                        ERROR_MSG("Erreur, 26 bits libres sont nécessaires pour stocker argument !\n");
                    }
                    val_5 = (I->arg3)->val.entier;
                    //
                    val_5 =  val_5 >> 2; // DECALAGE pour avoir 28 bits sur 26
                    // ajouter la valeur
                    binar_value = binar_value | ( val_5 & 0x03FFFFFF );
                }
            }


        }

        if (isdigit(from_5_0[0])){
            // si ne commence pas par 'a' ni '_' alors c'est un nombre car c'est ainsi que nous avons fait le dico
            // on sait que de par l'écriture du dico sa valeur < 5 < 16 bits
            val_5 = strtol(from_5_0, NULL, 0);
            binar_value = binar_value | ( val_5 & 0x0000003F );
        }

        printf("Pour cette instruction, binar_value vaut : %08x\n",binar_value);
        binar_value = swap(binar_value);
        printf("Pour cette instruction, binar_value SWAPEE vaut : %08x\n\n",binar_value);
        tab_instr_binar[i] = binar_value;
        i = i+1;
        list_instr = list_instr->next;

        free(instruction);
        free(from_25_21);
        free(from_20_16);
        free(from_15_11);
        free(from_10_6);
        free(from_5_0);
    }


    return tab_instr_binar;
}

int* data_in_binar(LIST list_data, int size_list){ // FONCTIONNE !
    // si c'est un .word mettre au début des 32 bits
    // si c'est un .byte un .asciiz, .space ou .byte : ajout indifférencié !
    // si c'est un DEC_LABEL = .word nécéssairement !

    // NOTE : un octet en binaire = 2 symboles en hexa !

    int* tab_data_binar = calloc (size_list, sizeof(int));
    int i=0;
    int j=0;
    unsigned int binar_value = 0;


    // parcourir la liste
    while (list_data != NULL){
        DATA Dat = list_data->element;
        if ((((Dat->D)->type) == PWORD ) || (((Dat->D)->type) == DEC_LABEL )){
            if ((j != 0)){
                // full with zeros
                binar_value = binar_value << (32-j);
                binar_value = swap(binar_value);
                tab_data_binar[i] = binar_value;
                i = i+1;
                printf("Pour cette directive, binar_value vaut : %08x\n\n",binar_value);
                binar_value = 0; // remise à 0 pour la suite
            }
            j=0;
            binar_value = (Dat->D)->val.PWORD;
            binar_value = swap(binar_value);
            tab_data_binar[i] = binar_value;
            i = i+1;
            printf("Pour cette directive, binar_value vaut : %08x\n\n",binar_value);
            binar_value = 0;
        }



        else { // coucou le .byte, .asciiz ou .space !

            if (j==32){
                binar_value = swap(binar_value);
                tab_data_binar[i] = binar_value;
                i = i+1;
                printf("Pour cette directive, binar_value vaut : %08x\n\n",binar_value);
                j = 0;
                binar_value = 0;
            }


            if (((Dat->D)->type) == PBYTE) {
                // alors ce sont des valeurs numériques qui tiennent sur 8 bits
                binar_value = (binar_value << 8) | ( (Dat->D)->val.PBYTE & 255  );
                j = j + 8;
            }
            if ( (Dat->D)->type == PSPACE ){
                // alors on réserve x octets à 0
                int M = (Dat->D)->val.PSPACE;
                int k = 0;
                while (k<M){
                    binar_value = (binar_value << 8); // rajout d'un octet de 0
                    j = j + 8;
                    if (j == 32){
                        binar_value = swap(binar_value);
                        tab_data_binar[i] = binar_value;
                        i = i+1;
                        printf("Pour cette directive, binar_value vaut : %08x\n\n",binar_value);
                        j = 0;
                        binar_value = 0;
                    }
                    k = k+1;
                }
            }

            if ((Dat->D)->type == PASCIIZ ){
                // ajouter une à une les lettres en table ascii qui tiennent sur un octet chacune
                int k = 0;
                while ((Dat->D)->val.PASCIIZ[k] != '\0') {
                    binar_value = (binar_value << 8) | ( ((int)((Dat->D)->val.PASCIIZ[k])) & 255  );
                    j = j + 8;
                    if (j == 32){
                        binar_value = swap(binar_value);
                        tab_data_binar[i] = binar_value;
                        i = i+1;
                        printf("Pour cette directive, binar_value vaut : %08x\n\n",binar_value);
                        j = 0;
                        binar_value = 0;
                    }
                    k = k+1;
                }
            }
        } // HOP On sort du else car la cellule DATA a été traitée, on passe à la suivante !

        list_data = list_data->next;

        }

    return tab_data_binar;
}




int look_for_instr_and_return_binar_info( LIST dictionnaire, char* instruction, char ** from_25_21, char ** from_20_16, char ** from_15_11, char ** from_10_6, char ** from_5_0)
// renvoit l'opcode et les autres en arg
 {
     int a;
     DICO_LINE D;
     while (dictionnaire != NULL)
     {
         D = dictionnaire->element;
         a = strcmp(D->instruction, instruction);

         if (a == 0) {
             *from_25_21 = strdup(D->code_bin_1);
             *from_20_16 = strdup(D->code_bin_2);
             *from_15_11 = strdup(D->code_bin_3);
             *from_10_6  = strdup(D->code_bin_4);
             *from_5_0   = strdup(D->code_bin_5);
             return D->opcode;
         }
         dictionnaire = dictionnaire->next;
     }
     ERROR_MSG("Probleme d'instruction non trouvee dans dictionnaire !\n");
 }

 int lengh_of_tab_data_in_binar(LIST l ){ // OK
     if (l==NULL) return 0;   // cas liste vide

     // NOTE en termes de décalage :
     // PWORD = 4 octets
     // PBYTE = 1 octet
     // PSPACE x = x octets (réservés à 0)
     // PASCIIZ = 1 octet par terme
     // DONC EN FAIT seul nous intéresse le décalage du dernier élement de la liste, qu'il faudra éventuellement augmenter pour avoir un mutilple de 4 !

     int dec = 0;
     while( (l->next)!=NULL ) {
         l = l->next;
     }
     DATA Dat = l->element;
     //printf("Voici la valeur de la directive : %d\n",Dat->D->type );
     dec = Dat->decalage; // dernier élément de la liste
     // il faut ajouter la valeur de décalage correspondant au dernier terme !
     if ((Dat->D->type == PWORD)||(Dat->D->type == DEC_LABEL)){ // car label seulement après .word
         dec = dec + 4;
     }
     if (Dat->D->type == PBYTE){
         dec = dec + 1;
     }
     if (Dat->D->type == PSPACE){
         dec = dec + Dat->D->val.PSPACE;
     }
     if (Dat->D->type == PASCIIZ){
        // printf("dec ici vaut : %d\n\n\n", dec);
         dec = dec + (strlen(Dat->D->val.PASCIIZ) +1 ); // +1 pour le \0
         //printf("strlen ici vaut : %ld\n\n\n", strlen(Dat->D->val.PASCIIZ));
     }
     while (dec%4){ // on doit être alignés sur un multiple de 4
         dec = dec +1;
     }
     // la valeur dec est en octets, nous on la veut en int = 4 octets
     int taille = dec/4;
     return taille ;

 }

 int lengh_of_space_in_bss(LIST l){

     if (l==NULL) return 0;   // cas liste vide

     int i =0;
     while( l!=NULL ) {
         i = i + ((DATA)(l->element))->D->val.PSPACE;
         l = l->next;
     }
     return i;
 }


int swap(int value){ // OK

    int swaped_value = ((value >> 24) & 0x000000FF) | ((value >> 8) & 0x0000FF00) | ((value << 8) & 0x00FF0000)
                    | ((value << 24) & 0xFF000000);

    return swaped_value;
}
