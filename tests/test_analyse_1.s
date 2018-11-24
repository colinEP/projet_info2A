.text
    Lw $t0 , boucle
    sw $t0 , boucle
    move $2, $4
boucle:
    BEQ $t0 , $t1 , 2
