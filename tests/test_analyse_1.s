.text
    MOVE $1, boucle
boucle:
    BEQ $t0 , $t1 , 2
    NOP
    NEG boucle, $6
    LI $t3, boucle
    BLT $1, $2, boucle
