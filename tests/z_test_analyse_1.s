# TEST_RETURN_CODE = PASS
# allons au ru
.set noreorder #test
.text

	#BNE $4,$5, boucle #test decalage
	#BLT $4,$5, boucle
    #Lw $t0, etiq1

	Lw $t2, etiqnondef
etiq:	Lw $t2, etiq
	Lw $t2, boucle
	LW $10, testoffsetneg
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
	.word -1   # ATTENTION TEST TRES IMPORTANT : A REVOIR
    .word boucle
	.word 2147483647
	#.word 2147483648
	#.word 9999999999
	.word -2147483648
	#.word -2147483649
    .byte 4 ,5 , 6
	#.space 1000000  #=> ca marche
    .word boucle
    .asciiz "ils disent : \"au ru!\""
    .byte 4


.bss
menu:
    .space 20
	.space 4
etiq5:    #OK
	#.space 65500   #pour test offset negatif
	#.space 0x1ffff   #pour test offset negatif   =0x186a0
testoffsetneg:
