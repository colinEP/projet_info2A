/**
 * @file lex.h
 * @author François Portet <francois.portet@imag.fr>
 * @brief Lexem-related stuff.
 *
 * Contains lexem types definitions, some low-level syntax error codes,
 * the lexem structure definition and the associated prototypes.
 */

#ifndef _LEX_H_
#define _LEX_H_

#include <stdio.h>
#include <queue.h>

enum {INIT , DEUX_PTS , VIRGULE , MOINS , COMMENT , REGISTRE ,
//     0         1         2        3        4         5
      DIRECTIVE , STRING , SYMBOLE , ZERO , HEXA , OCTAL ,
//  ,     6         7         8       9      10     11
      DECIMAL , AIBD };   // AIBD = Adressage Indirect avec Base et Déplacement
//      12       13



void	lex_read_line( char *, int, QUEUE);
void	lex_load_file( char *, unsigned int * );
char* 	getNextToken( char** , char* );

int   lex_analyse(char* token, unsigned int nline);
char* lex_type_string(int type, int normLen);

#endif /* _LEX_H_ */
