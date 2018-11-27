# TEST_RETURN_CODE = PASS
# allons au ru
.set noreorder #test
.text
    Lw $t0, lunchtime
    LW $6, -200($7)
    ADDI $t1,$zero,8
boucle:
    BEQ $t0 , $t1 , byebye
    NOP

    J boucle
    NOP
byebye:
    addi $t1 , $t1 , etiqnondef

.data
lunchtime:
    .word 12
    .word boucle
    .asciiz "ils disent : \"au ru!\""
.bss
menu:
    .space 24
