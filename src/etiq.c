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

 LIST add_to_symb_table(char* name_etiq, int dec, int line, int sect, LIST symb_table)
 {
     ETIQ etiq = calloc(1, sizeof(*etiq));
     etiq-> decalage = dec;
     etiq-> nline = line;
     etiq-> section = sect;
     etiq->name = strdup(name_etiq);
     symb_table = add_to_list(symb_table, etiq);
     return symb_table;
 }
