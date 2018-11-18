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
boucle:
    BEQ $t0 , $t1 , 6
 .data
 lunchtime:
    .word 12
    .word boucle    # mais pas là!
    .asciiz "ils disent : \"au ru!\""
    .asciiz "abc"
    .asciiz "12345"
    .byte 4
    .word 2147483647
    #.word 21474836494
    .word -2147483648
    #.word -2147483649
    .byte 127
    #.byte 128
    .byte -128
    #.byte -129
.bss
menu:
    .space 24
