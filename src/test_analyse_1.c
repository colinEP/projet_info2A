#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>

#include <global.h>
#include <notify.h>
#include <lex.h>
#include <queue_list.h>
#include <test.h>
#include <dictionnaire.h>
#include <etiq.h>
#include <analyse_synth.h>

#include <error.h>
#include <assert.h>





void test_analyse(LIST list_lex){
    LIST list_instr = NULL;
    LIST list_data = NULL;
    LIST list_bss = NULL;
    LIST symb_table = NULL;
    analyse_synth(list_instr, list_data, list_bss, symb_table, list_lex );
    return;
}
