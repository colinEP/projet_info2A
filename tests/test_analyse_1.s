.text
    Lw lunchtime   #ici se trouve une erreur de segmentation

    ADD $t1,$zero,$4
    NOP
    addi $t1 , $t1 , 1
boucle:
    BEQ $t0 , $t1 , 6
 .data
 lunchtime:
    .word 12
    .word boucle    # mais pas là!
    .asciiz "ils disent : \"au ru!\""
.bss
menu:
    .space 24
