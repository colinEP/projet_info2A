#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <strings.h>

#include <global.h>
#include <notify.h>
#include <lex.h>
#include <queue.h>

void type_lex_test();

void type_lex_test()
{
    if (lex_analyse("01234567",0) != OCTAL)
    {
        printf("Erreur analyse pour les %d\n", OCTAL);
    }

    if (lex_analyse(":",0) != DEUX_PTS)
    {
        printf("Erreur analyse pour les %d\n", DEUX_PTS);
    }

    if (lex_analyse("-",0) != MOINS)
    {
        printf("Erreur analyse pour les %d\n", MOINS);
    }

    if (lex_analyse(",",0) != VIRGULE)
    {
        printf("Erreur analyse pour les %d\n", VIRGULE);
    }

    if (lex_analyse("# Oui, je suis un : commentaire !",0) != COMMENT)
    {
        printf("Erreur analyse pour les %d\n", COMMENT);
    }

    if (lex_analyse("$ae87",0) != REGISTRE)
    {
        printf("Erreur analyse pour les %d\n", REGISTRE);
    }

    if (lex_analyse(".directive45!",0) != DIRECTIVE)
    {
        printf("Erreur analyse pour les %d\n", DIRECTIVE);
    }

    if (lex_analyse("\" Oui, je suis un : string !\"",0) != STRING)
    {
        printf("Erreur analyse pour les %d\n", STRING);
    }

    if (lex_analyse("0x8975",0) != HEXA)
    {
        printf("Erreur analyse pour les %d\n", HEXA);
    }

    if (lex_analyse("975846",0) != DECIMAL)
    {
        printf("Erreur analyse pour les %d\n", DECIMAL);
    }

    if (lex_analyse("200($789)",0) != AIBD)
    {
        printf("Erreur analyse pour les %d\n", AIBD);
    }

    // if (lex_analyse("200($789debut",0) != DEBUT_ADRESSAGE)
    // {
    //     printf("Erreur analyse pour les %d\n", DEBUT_ADRESSAGE);
    // }
    return;
}
