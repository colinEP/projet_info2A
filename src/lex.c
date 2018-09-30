
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
#include <queue.h>
#include <test_queue.h>

#include <error.h>


/* ATTENTION: getNextToken est juste un exemple -> il est à recoder completement !!! */
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
                    print_error("String non fermée", nline, end-1-line, line);
                    //break;
                }
            }
            end++;
        }
    }
    /* si on est encore dans une string => la string n'est pas fermée => ERROR */
    if (inString) print_error("String non fermée", nline, end-1-line, line);

    /*compute size : if zero there is no more token to extract*/
    token_size=end-start;
    if (token_size>0){
        *p_token = calloc(token_size+1,sizeof(*start));         /* WARNING WHAT ??? calloc sur deja un calloc ? ou est le free  => ok c'est un pointeur du token (qui est deja un pointer)*/
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
QUEUE lex_read_line( char *line, int nline, QUEUE Q) {
    char* token = NULL;
    char* current_address=line;
    int pos = 0;


    /* TODO : faire l'analyse lexical de chaque token ici et les ajouter dans une collection*/
    while( (current_address= getNextToken(&token, current_address, nline, line)) != NULL){

        /* TODO ANALYSE LEX ICI     (token)    => gros switch sa mere */
        /* TODO ajouter dans liste (ou fifo) */
        pos = (current_address-line)-strlen(token);
        int type = lex_analyse(token, nline, pos, line);
        if (type != -1) printf("%s  ",lex_type_string(type,TRUE) );
        else printf("           ");
        printf("%ld\t", strlen(token)); //DEBUG
        puts(token); //DEBUG
        // afficher en INFO_MSG


        // TODO erreur si les fonctions renvoie rien

        Q = add_to_queue(token, type, nline, Q);


        // TODO free(token) ???   non car mis dans une liste ??
    }

    // TODO add NL lexeme a la fin

    return Q;
}

/**
 * @param file Assembly source code file name.
 * @param nlines Pointer to the number of lines in the file.
 * @return should return the collection of lexemes
 * @brief This function loads an assembly code from a file into memory.
 *
 */
void lex_load_file( char *file, unsigned int *nlines ) {

    FILE        *fp   = NULL;
    char         line[STRLEN]; /* original source line */

    fp = fopen( file, "r" );
    if ( NULL == fp ) {
        /*macro ERROR_MSG : message d'erreur puis fin de programme ! */
        ERROR_MSG("Error while trying to open %s file --- Aborts",file);
    }

    *nlines = 0;
    QUEUE liste_queue = new_queue();
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
                liste_queue = lex_read_line(line,*nlines, liste_queue);
                // TODO free(line); ????
            }
        }

    }
    read_queue_lex(liste_queue);
    fclose(fp);
    return;
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
                else if (c=='-') return MOINS;//TODO erreur
                else if (c=='#') return COMMENT;

                else if (c=='$') {
                    /* check si le registre est pas "vide" */
                    if (token[i+1]!='\0') return REGISTRE;
                    // else printf("  ERR_INIT_reg\n");           //TODO erreur
                    else print_error("test de fct erreur", nline, pos+i, line);
                }
                else if (c=='.') {
                    /* check si la directive est pas "vide" */
                    if (token[i+1]!='\0') return DIRECTIVE;
                    //else printf("  ERR_INIT_direct\n");        //TODO erreur
                    else print_error("test de fct erreur", nline, pos+i, line);
                }
                else if (c=='"') {
                    /* check si c'est pas un '"' seul */
                    if (token[i+1]!='\0') return STRING;  //on sait qu la string est conforme ("fermée") car check dans getNextToken
                    // else printf("  ERR_INIT_str\n");           //TODO erreur
                    else print_error("test de fct erreurBBB", nline, pos+i, line);
                }

                else if ( isalpha(c) || c=='_' ) STATE=SYMBOLE;  // un symb ne peut pzs commencer par un chiffre

                else if (c=='0') {
                    /* check si c'est un 0 tout seul" */
                    if (token[i+1]=='\0') return DECIMAL;   //ou return OCTAL;
                    else STATE=ZERO;
                }
                else if (isdigit(c)) STATE=DECIMAL;   // c!='0' forcement vrai avec le if d'avant

                // else printf("  ERR_INIT\n");                   //TODO erreur
                else print_error("AAAAAAAA", nline, pos+i, line);

                break;

            case SYMBOLE:
                if ( !isalnum(c) && c!='_' ) print_error("test de fct erreur", nline, pos+i, line);//printf("  ERR_SYMB\n");    //TODO erreur
                break;

            case ZERO:
                if ( c=='x' || c=='X' ) STATE=HEXA;
                else if ( isdigit(c) && c<'8' ) STATE=OCTAL;   // un octal commence par 0 puis des chiffres < 8
                else if (c=='(') {
                    if (token[i+1]=='$') STATE=DEBUT_AIBD;
                    // else printf("  ERR_ZERO_aibd\n");          //TODO erreur
                    else print_error("test de fct erreur", nline, pos+i+1, line);
                }
                // else printf("  ERR_ZERO\n");                   //TODO erreur
                else print_error("test de fct erreur", nline, pos+i, line);
                break;

            case HEXA:
                if (c=='(') {
                    if (token[i+1]=='$') STATE=DEBUT_AIBD;
                    // else printf("  ERR_HEXA_aibd\n");          //TODO erreur
                    else print_error("test de fct erreur", nline, pos+i+1, line);
                }
                else if (!isxdigit(c)) print_error("test de fct erreur", nline, pos+i, line);//printf("  ERR_HEXA\n");          //TODO erreur
                break;

            case OCTAL :
                // WARNING est ce que ça peut etre un octal devant un AIBD ??
                if ( c<'0' || c>'7' ) print_error("test de fct erreur", nline, pos+i, line);//printf("  ERR_OCTAL\n"); //TODO erreur

            case DECIMAL:
                if (c=='(') {
                    if (token[i+1]=='$') STATE=DEBUT_AIBD;
                    // else printf("  ERR_DECIMAL_aibd\n");       //TODO erreur
                    else print_error("test de fct erreur", nline, pos+i+1, line);
                }
                else if (!isdigit(c)) print_error("test de fct erreur", nline, pos+i, line);//printf("  ERR_DECIMAL\n");        //TODO erreur
                break;

            case DEBUT_AIBD:  // Adressage Indirect avec Base et Déplacement
                if (c==')') {
                    if (token[i+1]=='\0') return AIBD;
                    else print_error("test de fct erreur", nline, pos+i+1, line);
                }
                break;
        }
    }
    if ( STATE==ZERO || STATE==INIT || STATE==DEBUT_AIBD) return -1;               //TODO erreur et suppr return -1
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
        else if (type==9)  return "ZERO       ";  //normalement impossible
        else if (type==10) return "HEXA       ";
        else if (type==11) return "OCTAL      ";
        else if (type==12) return "DECIMAL    ";
        else if (type==13) return "DEBUT_AIBD ";
        else if (type==14) return "AIBD       ";
        else ;                                 //TODO erreur
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
        else if (type==9)  return "ZERO";       //normalement impossible
        else if (type==10) return "HEXA";
        else if (type==11) return "OCTAL";
        else if (type==12) return "DECIMAL";
        else if (type==13) return "DEBUT_AIBD";
        else if (type==14) return "AIBD";
        else ;                                 //TODO erreur
    }
    return NULL; //quand gestion erreur ok, plus besoin ?
}
