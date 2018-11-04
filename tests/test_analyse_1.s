
.text
boucle:
    BEQ $t0 , $t1 , byebye
    J boucle
    NOP
byebye:
    JAL viteviteauru

.data
lunchtime:
    .word 12
    .word menu
    .asciiz "ils disent : \"au ru!\""
.bss
menu:
    .space 24
