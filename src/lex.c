
/**
 * @file lex.c
 * @author François Portet <francois.portet@imag.fr>
 * @brief Lexical analysis routines for MIPS assembly syntax.
 *
 * These routines perform the analysis of the lexeme of an assembly source code file.
 */

#define _POSIX_C_SOURCE 200112L
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
#include <print_functions.h>
#include <fct_analyse_1.h>

#include <error.h>



/**
 * @param token The pointeur to the lexeme to be extracted.
 * @param start The address from which the analysis must be performed.
 * @return the address at which the analysis stopped or NULL if the analysis is terminated.
 * @brief This function get an input line, extracts a token from it and return the pointeur to the next place where the analysis should continue.
 */

/* note that MIPS assembly supports distinctions between lower and upper case*/
char* getNextToken(char** p_token, char* start, unsigned int nline, char* line) {
    char* end=NULL;
    int token_size=0;
    int inComment = FALSE;
    int inString = FALSE;

    /* check input parameter*/
    if (start ==NULL) return NULL;                           /* utilité ? si ligne vide ? y'aura toujours le '\0'*/

    /* eats any blanks before the token*/
    while (*start!='\0' && isblank(*start)){
        start++;
    }
    /* go till the end of the token*/
    end=start;

    if      (*end==':') end++;    // ici on ne peut pas etre dans un commentaire ou une string
    else if (*end==',') end++;    // permet de récupérer juste ':' ou ',' ou '-' qui est en dehors d'un commentaire ou une string
    else if (*end=='-') end++;
    else {
        while ( *end!='\0' && ( ( !isblank(*end) && *end!=':' && *end!=',' && *end!='-' ) || inString || inComment ) ) {

            if ( *end == '#' ) inComment = TRUE;   // le comment va jusqu'a la fin de la ligne
    		if ( !inComment && *end == '"' ) inString = !inString;
            if ( inString && *end == '\\' ) {
                end++;                  // on saute le prochain caractere
                if (*end=='\0') {       // Sinon risque d'index out si '\\' est le dernier caractère de la ligne (on a suppr '\n' en fin de ligne)
                                        // exemple : ADD $t0, "example\              .
                                        // syntaxe fausse mais il faut prévoir les potentiels erreurs du developpeur
                    print_error_lex("string non fermée", nline, end-1-line, line);
                    //break;
                }
            }
            end++;
        }
    }
    /* si on est encore dans une string => la string n'est pas fermée => ERROR */
    if (inString) print_error_lex("string non fermée", nline, end-1-line, line);

    /*compute size : if zero there is no more token to extract*/
    token_size=end-start;
    if (token_size>0){
        *p_token = calloc(token_size+1,sizeof(*start));
        strncpy(*p_token,start,token_size);
        (*p_token)[token_size]='\0';
        return end;
    }
    return NULL;
}



/**
 * @param line String of the line of source code to be analysed.
 * @param nline the line number in the source code.
 * @return should return the collection of lexemes that represent the input line of source code.
 * @brief This function performs lexical analysis of one standardized line.
 *
 */
QUEUE lex_read_line( char *line, int nline, QUEUE list_lex) {
    char* token = NULL;
    char* current_address=line;
    int pos = 0;

    while( (current_address= getNextToken(&token, current_address, nline, line)) != NULL){

        pos = (current_address-line)-strlen(token);
        int type = lex_analyse(token, nline, pos, line);

        if (type==STRING) {
            char str[STRLEN];  // on sait que le token ne sera pas plus que STRLEN-2 au max
                               // pas besoin de faire de calloc car strdup dans add_to_queue_lex
            int len = strlen(token);
            int i;
            int j=0;
            // a ce moment on sait que les string sont bien fermées
            for (i=1 ; i<len-1 ; i++) {  // on vire les " extérieurs
                if(token[i] != '\\') {
                    str[j] = token[i];
                    j++;
                }
                else {
                    i++;
                    if      (token[i]=='t')  str[j]='\t';
                    else if (token[i]=='n')  str[j]='\n';
                    //else if (token[i]=='\'') str[j]='\'';
                    else if (token[i]=='\\') str[j]='\\';
                    else if (token[i]=='\"') str[j]='\"';
                    else if (token[i]=='0')  str[j]='\0';
                    else print_error_lex("Caractère d'échappement inconnu (échappements traités : \\t \\n \\\\ \\\" \\0)", nline, pos+i, line);
                    j++;
                }
            }
            str[j]='\0';
            list_lex = add_to_queue_lex(list_lex, str, type, nline);
        }
        else list_lex = add_to_queue_lex(list_lex, token, type, nline);

        /* Une fois le token utilisé, il faut le free */
        free(token);
    }

    // WARNING a voir si c'est utile pour la suite ... sinon on peut l'enlever facilement
    list_lex = add_to_queue_lex(list_lex, "\\n", 15, nline);

    return list_lex;
}

/**
 * @param file Assembly source code file name.
 * @param nlines Pointer to the number of lines in the file.
 * @return should return the collection of lexemes
 * @brief This function loads an assembly code from a file into memory.
 *
 */
QUEUE lex_load_file( char *file, unsigned int *nlines , QUEUE list_lex) {
    /* Pour povoir ajouter en queue plus facilemant,
       on traite list_lex comme si c'était une QUEUE.
       On le mettra sous forme de LIST avec de renvoyer list_lex */

    FILE        *fp   = NULL;
    char         line[STRLEN]; /* original source line */

    INFO_MSG("Ouverture du fichier assembleur à compiler");
    fp = fopen( file, "r" );
    if ( NULL == fp ) {
        /*macro ERROR_MSG : message d'erreur puis fin de programme ! */
        ERROR_MSG("Error while trying to open %s file --- Aborts",file);
    }

    *nlines = 0;

    while(!feof(fp)) {     // feof => test si EOF (end of file)

        /*read source code line-by-line */
        if ( NULL != fgets( line, STRLEN, fp ) ) {      // pas besoin de mettre STRLEN-1 ("It stops when either (n-1) characters are read")
                                                        // https://www.tutorialspoint.com/c_standard_library/c_function_fgets.htm
            (*nlines)++;
            /* test si la ligne n'etait pas trop longue */
            if (line[strlen(line)-1] != '\n' ) {
                ERROR_MSG("Line #%d too long ! Max lenght: %d characters  (NL excluded)", *nlines, STRLEN-2);
            }

            line[strlen(line)-1] = '\0';  /* eat final '\n' */

            if ( 0 != strlen(line) ) {
                list_lex = lex_read_line(line,*nlines, list_lex);
            }
        }
    }
    fclose(fp);

    /* rétablissement de la QUEUE sous forme de LIST */
    list_lex = queue_to_list(list_lex);

    return list_lex;
}


int lex_analyse(char* token, unsigned int nline, int pos, char* line) {
    int STATE=INIT;
    char c;

    int i;
    for (i=0 ; token[i]!='\0' ; i++) {
        c = token[i];

        switch(STATE) {
            case INIT:
                if      (c==':') return DEUX_PTS;
                else if (c==',') return VIRGULE;
                else if (c=='-') return MOINS;
                else if (c=='#') return COMMENT;
                else if (c=='"') return STRING;  // on sait que la string est conforme ("fermée") car check dans getNextToken
                else if (c=='$') {
                    /* check si le registre est pas "vide" */
                    if (token[i+1]!='\0') return REGISTRE;
                    else print_error_lex("[ERR_INIT_REG] register name expected after '$'", nline, pos+i, line);
                }
                else if (c=='.') {
                    /* check si la directive est pas "vide" */
                    if (token[i+1]!='\0') return DIRECTIVE;
                    else print_error_lex("[ERR_INIT_DIR] directive name excepted after '.'", nline, pos+i, line);
                }
                else if ( isalpha(c) || c=='_' ) STATE=SYMBOLE;  // un symb ne peut pas commencer par un chiffre
                else if (c=='0') {
                    /* check si c'est un 0 tout seul" */
                    if (token[i+1]=='\0') return DECIMAL;   //ou return OCTAL;
                    else STATE=ZERO;
                }
                else if (isdigit(c)) STATE=DECIMAL;   // c!='0' forcement vrai avec le if d'avant
                else print_error_lex("[ERR_INIT] character unexpected", nline, pos+i, line);
                break;

            case SYMBOLE:
                if ( !isalnum(c) && c!='_' ) print_error_lex("[ERR_SYMBOLE] character unexpected in symbole", nline, pos+i, line);
                break;

            case ZERO:
                if ( c=='x' || c=='X' ) STATE=HEXA;
                else if ( c>='0' && c<'8' ) STATE=OCTAL;   // un octal commence par 0 puis des chiffres < 8
                else if (c=='(') {
                    if (token[i+1]=='$') STATE=DEBUT_AIBD;
                    else print_error_lex("[ERR_ZERO] register expected in '()' for \"Adressage indirect avec base et déplacement\"", nline, pos+i+1, line);
                }
                else print_error_lex("[ERR_ZERO] character unexpected after '0'", nline, pos+i, line);
                break;

            case HEXA:
                if (c=='(') {
                    if (token[i+1]=='$') STATE=DEBUT_AIBD;
                    else print_error_lex("[ERR_HEXA] register expected in '()' for \"Adressage indirect avec base et déplacement\"", nline, pos+i+1, line);
                }
                else if (!isxdigit(c)) print_error_lex("[ERR_HEXA] character unexpected for hexa number", nline, pos+i, line);
                break;

            case OCTAL :
                // WARNING est ce que ça peut etre un octal devant un AIBD ??
                if ( c<'0' || c>'7' ) print_error_lex("[ERR_OCTAL] character unexpected for octal number", nline, pos+i, line);
                break;

            case DECIMAL:
                if (c=='(') {
                    if (token[i+1]=='$') STATE=DEBUT_AIBD;
                    else print_error_lex("[ERR_DECIMAL] register expected in '()' for \"Adressage indirect avec base et déplacement\"", nline, pos+i+1, line);
                }
                else if (!isdigit(c)) print_error_lex("[ERR_DECIMAL] character unexpected for decimal number", nline, pos+i, line);
                break;

            case DEBUT_AIBD:  // Adressage Indirect avec Base et Déplacement
                if (c==')') {
                    if (token[i+1]=='\0') return AIBD;
                    else print_error_lex("[ERR_AIBD] expected nothing after ')' for \"Adressage indirect avec base et déplacement\"", nline, pos+i+1, line);
                }
                break;
        }
    }
    if ( STATE==ZERO || STATE==INIT || STATE==DEBUT_AIBD) ERROR_MSG("Problème dans la machine à état : renvoie un état impossible (num_etat = %d)", STATE);
    return STATE;
}



char* lex_type_string(int type, int norm_len) {        // strdup   ????????
    if (norm_len) {               // len == 11
        if      (type==0)  return "INIT       ";
        else if (type==1)  return "DEUX_PTS   ";
        else if (type==2)  return "VIRGULE    ";
        else if (type==3)  return "MOINS      ";
        else if (type==4)  return "COMMENT    ";
        else if (type==5)  return "REGISTRE   ";
        else if (type==6)  return "DIRECTIVE  ";
        else if (type==7)  return "STRING     ";
        else if (type==8)  return "SYMBOLE    ";
        else if (type==9)  return "ZERO       ";
        else if (type==10) return "HEXA       ";
        else if (type==11) return "OCTAL      ";
        else if (type==12) return "DECIMAL    ";
        else if (type==13) return "DEBUT_AIBD ";
        else if (type==14) return "AIBD       ";
        else if (type==15) return "NL         ";
        else ERROR_MSG("type=%d : num de type non valide, doit etre compris entre 0 et 15", type);
    }
    else {
        if      (type==0)  return "INIT";
        else if (type==1)  return "DEUX_PTS";
        else if (type==2)  return "VIRGULE";
        else if (type==3)  return "MOINS";
        else if (type==4)  return "COMMENT";
        else if (type==5)  return "REGISTRE";
        else if (type==6)  return "DIRECTIVE";
        else if (type==7)  return "STRING";
        else if (type==8)  return "SYMBOLE";
        else if (type==9)  return "ZERO";
        else if (type==10) return "HEXA";
        else if (type==11) return "OCTAL";
        else if (type==12) return "DECIMAL";
        else if (type==13) return "DEBUT_AIBD";
        else if (type==14) return "AIBD";
        else if (type==15) return "NL";
        else ERROR_MSG("type=%d : num de type non valide, doit etre compris entre 0 et 15", type);
    }
    return NULL;  // normalement impossible d'arriver ici
}
