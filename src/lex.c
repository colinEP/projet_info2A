
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



/* ATTENTION: getNextToken est juste un exemple -> il est à recoder completement !!! */
/**
 * @param token The pointeur to the lexeme to be extracted.
 * @param current_line The address from which the analysis must be performed.
 * @return the address at which the analysis stopped or NULL if the analysis is terminated.
 * @brief This function get an input line, extracts a token from it and return the pointeur to the next place where the analysis should continue.
 */

/* note that MIPS assembly supports distinctions between lower and upper case*/
char* getNextToken(char** p_token, char* current_line) {
    char* start = current_line;
    char* end=NULL;
    int token_size=0;
    int inComment = FALSE;
    int inString = FALSE;

    /* check input parameter*/
    if (current_line ==NULL) return NULL;                           /* utilité ? */

    /* eats any blanks before the token*/
    while (*start!='\0' && isblank(*start)){
        start++;
    }
    /* go till next blank or end*/
    end=start;

    if (*end==':') end++;
    else if (*end==',') end++;
    else if (*end=='-') end++;
    else {
        while ( *end!='\0' && ( (!isblank(*end) && *end!=':' && *end!=',' && *end!='-') || inString || inComment) ) {

            if ( *end == '#' ) inComment = TRUE;   // le comment va jusqu'a la fin de la ligne
    		if ( !inComment && *end == '"' ) inString = !inString;
            if ( inString && *end == '\\' ) {
                end++;     // on saute le prochain caractere
                if (*end=='\0') break;  // sinon risque d'index out si '\\' est le dernier caractère de la ligne
            }
            end++;
        }
    }

    /*compute size : if zero there is no more token to extract*/
    token_size=end-start;
    if (token_size>0){
        *p_token = calloc(token_size+1,sizeof(*start));         /* WHAT ??? calloc sur deja un calloc ? ou est le free  => ok c'est un pointeur du token (qui est deja un pointer)*/
        strncpy(*p_token,start,token_size);
        (*p_token)[token_size]='\0';
        //printf("  %d  ", token_size);
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
void lex_read_line( char *line, int nline) {
    char* token = NULL;
    char* current_address=line;


    /* TODO : faire l'analyse lexical de chaque token ici et les ajouter dans une collection*/
    /* ATTENTION: getNextToken est à recoder completement*/
    while( (current_address= getNextToken(&token, current_address)) != NULL){

        /* TODO ANALYSE LEX ICI     (token)    => gros switch sa mere */
        /* ajouter dans liste (ou fifo) */

        // test comment probleme C90

        puts(token);

        /* free(token) ???   non car mis dans une liste ??*/
    }

    return;
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

    while(!feof(fp)) {     /*feof => test si EOF (end of file)*/

        /*read source code line-by-line */
        if ( NULL != fgets( line, STRLEN-1, fp ) ) {                     /*-1 car ajout de '\0' à la fin   //inclue => donc pas besion de -1 ??????*/
            line[strlen(line)-1] = '\0';  /* eat final '\n' */
            (*nlines)++;

            if ( 0 != strlen(line) ) {
                /* TODO NORMALISATION DE LA LIGNE A METTRE ICI */
                //////char* line_norm = norm_line(line);
                //printf("     %s\n", line_norm);   // DEBUG
                // free(line); ????
                lex_read_line(line,*nlines);
            }
        }
    }

    fclose(fp);
    return;
}
