# TEST_RETURN_CODE = PASS
# allons au ru
.set noreorder #test
.text
	BLT $4,$5,boucle #test decalage
	# ERROR pseudo-instr : arg2 = None alors que arg3 = 4
    Lw $t0, lunchtime
    Lw $t0, etiqnondef
    LW $6, -200($7)
    ADDI $t1,$zero,8
boucle:
    BEQ $t0 , $t1 , byebye
    NOP
    J etiqnondef
    NOP
byebye:
    addi $t1 , $t1 , 8

.data
lunchtime:
    .word 12
etiq1:    #OK
    .byte 4
etiq2:    #err => if .word après aligné sur 4
    .word boucle
etiq3:    #OK
    .asciiz "ils disent : \"au ru!\""
etiq4:    # pas tester car faut corriger gestion str avant
.bss
menu:
    .space 24
etiq5:    #OK
