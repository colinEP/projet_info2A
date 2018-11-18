.text
    LW $6, -200($7)
    ADDI $t1,$zero,8
boucle:
    BEQ $t0 , $t1 , byebye
    NOP
    addi $t1 , $t1 , 1
    J boucle
    NOP
byebye:
    JAL 754

.data
lunchtime:
    .word 12
    .word boucle
    .asciiz "ils disent : \"au ru!\""
.bss
menu:
    .space 24
