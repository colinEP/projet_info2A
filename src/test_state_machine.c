#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <strings.h>

#include <global.h>
#include <notify.h>
#include <lex.h>

#include <test.h>



void type_lex_test()
{
    printf("\n------ test analyse lexeme --------\n");
    if (lex_analyse("01234567",0 , 0, NULL) != OCTAL)
    {
        printf("Erreur analyse pour les %d\n", OCTAL);
    }
    else printf("analyse ok pour les %d\n", OCTAL);

    if (lex_analyse("ProjetInfo1",0, 0, NULL) != SYMBOLE)
    {
        printf("Erreur analyse pour les %d\n", SYMBOLE);
    }
    else printf("analyse ok pour les %d\n", SYMBOLE);


    if (lex_analyse("0",0, 0, NULL) != DECIMAL) //type 0 impossible !
    {
        printf("Erreur analyse pour les %d\n", DECIMAL);
    }
    else printf("analyse ok pour les %d\n", DECIMAL);

    if (lex_analyse(":",0, 0, NULL) != DEUX_PTS)
    {
        printf("Erreur analyse pour les %d\n", DEUX_PTS);
    }
    else printf("analyse ok pour les %d\n", DEUX_PTS);

    if (lex_analyse("-",0, 0, NULL) != MOINS)
    {
        printf("Erreur analyse pour les %d\n", MOINS);
    }
    else printf("analyse ok pour les %d\n", MOINS);

    if (lex_analyse(",",0, 0, NULL) != VIRGULE)
    {
        printf("Erreur analyse pour les %d\n", VIRGULE);
    }
    else printf("analyse ok pour les %d\n", VIRGULE);

    if (lex_analyse("# Oui, je suis un : commentaire !",0, 0, NULL) != COMMENT)
    {
        printf("Erreur analyse pour les %d\n", COMMENT);
    }
    else printf("analyse ok pour les %d\n", COMMENT);

    if (lex_analyse("$ae87",0, 0, NULL) != REGISTRE)
    {
        printf("Erreur analyse pour les %d\n", REGISTRE);
    }
    else printf("analyse ok pour les %d\n", REGISTRE);

    if (lex_analyse(".directive45!",0, 0, NULL) != DIRECTIVE)
    {
        printf("Erreur analyse pour les %d\n", DIRECTIVE);
    }
    else printf("analyse ok pour les %d\n", DIRECTIVE);

    if (lex_analyse("\" Oui, je suis un : string !\"",0, 0, NULL) != STRING)
    {
        printf("Erreur analyse pour les %d\n", STRING);
    }
    else printf("analyse ok pour les %d\n", STRING);

    if (lex_analyse("0x8975",0, 0, NULL) != HEXA)
    {
        printf("Erreur analyse pour les %d\n", HEXA);
    }
    else printf("analyse ok pour les %d\n", HEXA);

    if (lex_analyse("975846",0, 0, NULL) != DECIMAL)
    {
        printf("Erreur analyse pour les %d\n", DECIMAL);
    }
    else printf("analyse ok pour les %d\n", DECIMAL);

    if (lex_analyse("200($789)",0, 0, NULL) != AIBD)
    {
        printf("Erreur analyse pour les %d\n", AIBD);
    }
    else printf("analyse ok pour les %d\n", AIBD);


    // if (lex_analyse("200($789debut",0) != DEBUT_ADRESSAGE)
    // {
    //     printf("Erreur analyse pour les %d\n", DEBUT_ADRESSAGE);
    // }
    return;
}
