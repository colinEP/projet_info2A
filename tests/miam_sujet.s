# TEST_RETURN_CODE = PASS
# allons au ru
.set noreorder  .  $t5
.text
    Lw $t0 ,      lunchtime
    LW $6, -    200($7)
    ADDI $t1,$zero,8
boucle       :
    BEQ $t0 , $t1 , byebye
    NOP
    addi $t1 , $t1 , 1
    J boucle
    NOP                                $
byebye:
    JAL viteviteauru
    0
    0x45af
    0b1234
    0x12sd
    04545
    04582
    0($ded)
    200($dee
    200(7)
.data
lunchtime:
    .word 12
    .word menu
    .asciiz "ils disent : \"au,: ru!\"\\\\\\\\"
.bss
menu:
    .space 24   "
