#include <stdlib.h>
#include <stdio.h>
#include <string.h>
/*#include "fonctions.h"*/
#include <norm.h>
#include <global.h>
/* celui que j'ai modifié, il marche*/

/* note : a deplacer */
char *mystrcat( char *start, char *addend )
{
	size_t slen = strlen( start );
	size_t alen = strlen( addend );
	char   *str = calloc( 1, slen+alen+1 );
	memcpy( str, start, slen );
	memcpy( str+slen, addend, alen ); /* ajout du \0 automatique !*/
    return str;
}





char* norm_line(char* str)  {
  	int taille = strlen(str);
	int i;
	int normActive = TRUE;  // par défaut on normalise

    for (i=0; i<taille ; i++) {
		if ( normActive && str[i] == '#' ) return str;   // le comment va jusqu'a la fin de la ligne
		if ( str[i] == '"' ) normActive = !normActive;   // impossible d'etre dans un commentaire return avant
		if ( !normActive && str[i] == '\\' ) i++;   // si '\\' est le dernier element de la str => pas de risque d'index out car il y a encore '\0'

	    if (normActive) {
			if (str[i] == ':' || str[i] == ',') {  // l'égalité fonctionne entre des nombres, donc on met '..' pour considérer les chiffres ascii
				str = add_space_left(str, i); //if STRLEN OK
				i++;
				str = add_space_right(str, i);
				i++;
				taille +=2;
			}
			if (str[i] == '-') {
				str = del_blank_right(str,i);
				i++;
				taille++;

			}
		}
    }

  return str;

}

/* cette fonction fonctionne !*/


char* add_space_right(char* chaine, int i)
{
	char* espace = strdup(" ");
	char* new_chaine= calloc(1, i);

	strncpy(new_chaine, chaine, i+1);
	new_chaine[i+1] = '\0'; /* utile ?*/
	new_chaine = mystrcat (new_chaine, espace); /* ajout espace après les deux points*/
	new_chaine = mystrcat (new_chaine, chaine + i + 1); /* copie fin du texte dans la new_chaine*/
	//free(chaine)   ????????????????
	return new_chaine;
}

char* add_space_left(char* chaine, int i)
{
	char* espace = strdup(" "); /* ou char espace[] = " ";*/
	char* new_chaine = calloc(1, i);

	strncpy(new_chaine, chaine, i+1);
	new_chaine[i] = '\0'; /*utile ?*/
	new_chaine = mystrcat (new_chaine, espace); /* ajout espace avant les deux points*/
	new_chaine = mystrcat (new_chaine, chaine + i); /* copie fin du texte dans la new_chaine*/
	//free(chaine)   ????????????????
	return new_chaine;
}

char* del_blank_right(char* str,int pos) {
	int len = strlen(str);

	int i;
	for (i=pos+1 ; str[i] == ' ' || str[i] == '\t' ; i++);
	memcpy( str+pos+1 , str+i , len -1 -(pos+1) );
	return str;
}



// int norm_on_off(int N,char chaine[], int i, int Back_Slash)
// /*renvoit 1 si on doit normaliser, 0 sinon*/
// {
//     if (chaine[i] == '"' && Back_Slash == 0) /* on rentre dans la boucle seulement s'il n'y a pas un bakslash avant*/
//       {
//           if (N == 0) return 1;
//           else return 0;
//       }
//
//       return N;
// }
