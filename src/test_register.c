#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <ctype.h>

#include <global.h>
#include <notify.h>
#include <lex.h>
#include <queue_list.h>
#include <test.h>
#include <dictionnaire.h>
#include <etiq.h>
#include <fct_analyse_1.h>
#include <analyse_synth.h>
#include <print_functions.h>

#include <error.h>
#include <assert.h>


void test_register()
{
    int reg;
    printf("\n------ test des registres --------\n");

    reg = check_and_convert_register("$t0");
    printf("Pour le symbole t0 le prog renvoit : %d \n", reg);
    reg = check_and_convert_register("$t1");
    printf("Pour le symbole t1 le prog renvoit : %d \n", reg);
    reg = check_and_convert_register("$t2");
    printf("Pour le symbole t2 le prog renvoit : %d \n", reg);
    reg = check_and_convert_register("$t3");
    printf("Pour le symbole t3 le prog renvoit : %d \n", reg);
    reg = check_and_convert_register("$t4");
    printf("Pour le symbole t4 le prog renvoit : %d \n", reg);
    reg = check_and_convert_register("$t5");
    printf("Pour le symbole t5 le prog renvoit : %d \n", reg);
    reg = check_and_convert_register("$t6");
    printf("Pour le symbole t6 le prog renvoit : %d \n", reg);
    reg = check_and_convert_register("$t7");
    printf("Pour le symbole t7 le prog renvoit : %d \n", reg);
    reg = check_and_convert_register("$a0");
    printf("Pour le symbole a0 le prog renvoit : %d \n", reg);
    reg = check_and_convert_register("$a1");
    printf("Pour le symbole a1 le prog renvoit : %d \n", reg);
    reg = check_and_convert_register("$k1");
    printf("Pour le symbole k1 le prog renvoit : %d \n", reg);
    reg = check_and_convert_register("$31");
    printf("Pour le symbole 31 le prog renvoit : %d \n", reg);
    reg = check_and_convert_register("$15");
    printf("Pour le symbole 15 le prog renvoit : %d \n", reg);
    reg = check_and_convert_register("$ra");
    printf("Pour le symbole ra le prog renvoit : %d \n", reg);
    reg = check_and_convert_register("$gp");
    printf("Pour le symbole gp le prog renvoit : %d \n", reg);
    reg = check_and_convert_register("$fp");
    printf("Pour le symbole fp le prog renvoit : %d \n", reg);
    reg = check_and_convert_register("$zero");
    printf("Pour le symbole zero le prog renvoit : %d \n", reg);


    return;

}
