.text
    ADd 1, 2, 3 # OK
    #luchtime
    Lw 0 , 800
    LW 6 , 55
    ADDI 1,8, 3
    bEQ $t0 , $t1 , byebye
    NOP
    addi $t1 , $t1 , 1
    J boucle
    NOP
.data
