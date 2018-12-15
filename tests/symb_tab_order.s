# TEST_RETURN_CODE = PASS
# allons au ru
.set noreorder
.text
    Lw $t0 , lunchtime
    LW $6, -200($7)
    ADDI $t1,$zero, BEQ
boucle:
    ADDI $t1,$zero, LW
    BEQ $t0 , $t1 , byebye
    NOP
boucle2:
    addi $t1 , $t1 , 1
    J boucle
    NOP
byebye: BEQ: LW:
    JAL viteviteauru
    JAL COUROUCOUCOU

.data
lunchtime:
    .word 12
    .word boucle
    .asciiz "ils disent : \"au ru!\""
    .word COUROUCOUCOU

.text
    BEQ $t0 , $t1 , byebye
boucle3:
    J lunchtime


.bss
menu:
    .space 24

.text
    BEQ $t0 , $t1 , BEQ
