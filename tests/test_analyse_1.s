# TEST_RETURN_CODE = PASS
# allons au ru
.set noreorder #test
.text
	BNE $4,$5, boucle #test decalage
	BLT $4,$5, boucle
	# ERROR pseudo-instr : arg2 = None alors que arg3 = 4
    Lw $t0, etiq1
    Lw $t1, etiqnondef
    LW $6, -200($7)
    ADDI $t1,$zero,8
boucle:
    BEQ $t0 , $t1 , byebye
    NOP
    J etiqnondef
	J boucle
    NOP
byebye:
    addi $t1 , $t1 , 8

.data
lunchtime:
    .word boucle
	.word 2147483647
	#.word 2147483648
	#.word 9999999999
	.word -2147483648
	#.word -2147483649
    .byte 4 ,5 , 6
    .word boucle
    .asciiz "ils disent : \"au ru!\""
    .byte 4

.bss
menu:
    .space 24
etiq5:    #OK
