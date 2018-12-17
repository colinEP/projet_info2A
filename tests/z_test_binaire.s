# TEST_RETURN_CODE = PASS
# allons au ru
.set noreorder
.text
	BNE $4,$5, boucle #test decalage
	BLT $4,$5, boucle
    ADDI $t1,$zero,8
boucle:
    BEQ $t0 , $t1 , byebye
    NOP
    J etiqnondef
	J boucle
	.set noreorder #test
    NOP
byebye:
    addi $t1 , $t1 , 8

.data
lunchtime:
    .word 12
    .word boucle
	.space 5
	.byte 3
	.space 1
    .asciiz "123ils disent : \"au rrrrrru!\""
dec_de_fin:
	.byte 4
dec_de_fin2:
	.word 4
