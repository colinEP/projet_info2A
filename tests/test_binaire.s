# TEST_RETURN_CODE = PASS
# allons au ru
.set noreorder #test
.text
	BNE $4,$5, boucle #test decalage
	BLT $4,$5, boucle
    ADDI $t1,$zero,8
boucle:
    BEQ $t0 , $t1 , byebye
    NOP
    J etiqnondef
	J boucle
    NOP
byebye:
    addi $t1 , $t1 , 8
