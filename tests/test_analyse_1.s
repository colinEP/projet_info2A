# TEST_RETURN_CODE = PASS
# allons au ru
.set noreorder
.text #comm
    ADDI $t1,$zero,8
    NOP
    MOVE $s5, $t1
    NEG $k0, $gp
    LI $28, 51
boucle: #comm2
    BEQ $t0 , $t1 , byebye
    addi $t1 , $t1 , 1
    J boucle
    NOP
byebye:
    JAL 754
