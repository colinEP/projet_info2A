# TEST_RETURN_CODE = PASS
# allons au ru
.set noreorder
.text #comm
    Lw $t0 , lunchtime
    LW $6, -200($7)
    ADDI $t1,$zero,8
boucle: #comm2
    BEQ $t0 , $t1 , byebye
    NOP
    addi $t1 , $t1 , 1
    J boucle
    NOP
byebye:
    JAL boucle

.data
lunchtime:
    .word 12
    .word boucle
    .asciiz "ils disent : \"au ru!\""
.bss
menu:
    .space 24
