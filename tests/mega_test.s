# TEST_RETURN_CODE = PASS
.set noreorder

.text
predef:
    ##### test erreur pseudo instruction #####
    #NEG $t7, predef
    #MOVE $t7, predef
    #LI predef , 64
    #BLT $4, predef, 4
    #NoP $4
    #LI predef, 1
    #LW predef, 8($4)
    #LW predef, predef
    # SW predef, predef
    ######################  => OK !

    #       3 = 00011
    # t7 = 15 = 01111
    # a2 = 6  = 00110

    ADD $3, $t7, $a2
    adD $3, $t7, $a2    #test uppercase
    # ADDI $3, $t7, - -500  # doit planter  => OK
    ADDI $3, $t7, -500
    ADDI $3, $t7, 0254
    ADDI $3, $t7, 0xFFFF     # gestion hexa RESOLU
    #ADDI $3, $t7, 0xFFFF    # doit planter  => OK
    ADDI $3, $t7, -1
    ADDI $3, $t7, predef
    ADDI $3, $t7, postdef
    ADDI $3, $t7, nodef
    ADDI $3, $t7, etiqdata
    # ADDI $3, $t7, 0x1FFFF     # doit planter  => OK
    ADDI $3, $t7, etiqbss     # PAS DE VERIF SUR LA TAILLE DU DECALAGE !!!
                              # NORMALEMENT DEVRAIT FAIRE UNE ERREUR CAR etiqbss=0x1FFFF
                              # MAIS BON PAS GRAVE !!!!

    SUB $3, $t7, $a2
    MULT $3, $t7
    DIV $3, $t7
    AND $3, $t7, $a2
    OR $3, $t7, $a2
    XOR $3, $t7, $a2
    ROTR $3, $t7, 31
    ROTR $3, $t7, 0xF
    # ROTR $3, $t7, predef    # Plante !!!!!  normal ? etiq interdit pour Sa ?
    # ROTR $3, $t7, -0xF  # doit planter  => OK
    # ROTR $3, $t7, 0x20  # doit planter  => OK
    # ROTR $3, $t7, 32 # doit planter  => OK
    # ROTR $3, $t7, -1 # doit planter  => OK
    SLL $3, $t7, 2
    SRL $3, $t7, 2
    SLT $3, $t7, $a2

    LW $3, 200($4)
    # LW $3, ($4)      #doit planter    => OK
    # LW $3, 200( )    #doit planter    => OK
    # LW $3, 200 ($4)   # error => a corrigé ??????? TODO TODO
    # LW $3, 200( $4)   # error => a corrigé ??????? TODO TODO
    LW $3, -8($4)
    # LW $3, 0x1FFFF($4)  #doit planter (pas %4)    => OK
    # LW $3, 0x10000($4)  #doit planter   => OK
    LW $3, 0xFFFF($4)
    LW $3, 0xFFFF($4)
    LW $3, 0xFFFF($4)
    SW $3, 8($4)
    SW $3, -8($4)

    LUI $3 , 64
    LUI $3 , -2
    LUI $3 , 32767
    LUI $3 , -32768
    LUI $3 , 0xFFFF   #  gestion hexa corrigé
    LUI $3 , predef
    LUI $3 , postdef
    LUI $3 , nodef
    LUI $3 , startdata
    LUI $3 , etiqbss

    MFHI $t5
    MFLO $t7

    #BEQ $3, $t7, 131070
    # BEQ $3, $t7, 0xFF     #doit planter (pas %4)    => OK
    BEQ $3, $t7, 0xFC
    BEQ $3, $t7, 0x3FFFC    # max car doit aussi etre divisible par 4
    # BEQ $3, $t7, 0x40000    #doit planter (pas %4)    => OK
    BEQ $3, $t7, 131068     # max car doit aussi etre divisible par 4
    BEQ $3, $t7, -131072
    # BEQ $3, $t7, 131072       #doit planter (pas %4)    => OK

    #BEQ $3, $t7, -10         #doit planter (pas %4)    => OK
    BEQ $3, $t7, predef
    BEQ $3, $t7, postdef
    # BEQ $3, $t7, nodef       #doit planter    => OK
    # BEQ $3, $t7, startdata   #doit planter    => OK
    BNE $3, $t7, 4
    # BNE $3, $t7, -10     #doit planter (pas %4)    => OK
    BNE $3, $t7, predef
    BNE $3, $t7, postdef
    # BNE $3, $t7, nodef   #doit planter    => OK
    BGTZ $3, 4
    # BGTZ $3, -10          #doit planter    => OK
    BGTZ $3, predef
    BGTZ $3, postdef
    # BGTZ $3, nodef       #doit planter    => OK
    BLEZ $3, 4
    # BLEZ $3, -10         #doit planter (pas %4)    => OK
    BLEZ $3, predef
    BLEZ $3, postdef
    # BLEZ $3, nodef       #doit planter    => OK

    J 8
    # J -8 #erreur normalement ???
    J 0x3FC
    # J -0x3FC     #doit planter    => OK
    # J 0x3FD      #doit planter    => OK
    # J -4         #doit planter    => OK
    J 0x0FFFFFFC   # max car doit aussi etre divisible par 4
    # J 0x1FFFFFFC   #doit planter    => OK
    # J 0x10000000  #doit planter    => OK
    J predef
    J postdef
    J nodef
    J etiqdata      # erreur si jump dans autre section ???????
    J etiqbss       # erreur si jump dans autre section ???????
    JAL 8
    # JAL -8    #doit planter    => OK
    JAL 0x3FC
    JAL predef
    JAL postdef
    JAL nodef
    JAL etiqdata       # erreur si jump dans autre section ???????
    JAL etiqbss        # erreur si jump dans autre section ???????
    JR $3

    #### pseudo instruction ####
    NOP
    MOVE $3, $t7
    NEG $3, $t7

    LI $3 , 64
    LI $3 , -2
    LI $3 , 32767
    LI $3 , -32768
    LI $3 , 0xFFFF    # gestion hexa corrigé
    LI $3 , predef    #  => OK
    LI $3 , postdef
    LI $3 , nodef
    LI $3 , etiqdata
    LI $3 , etiqbss

    BLT $3, $t7, 4
    BLT $3, $t7, 0xFC
    # BLT $3, $t7, -10  #doit planter (pas %4)    => OK
    BLT $3, $t7, predef
    BLT $3, $t7, postdef
    # BLT $3, $t7, nodef      #doit planter    => OK
    # BLT $3, $t7, etiqdata   #doit planter    => OK

    LW $3, predef
    LW $3, postdef
    LW $3, nodef
    LW $3, etiqdata
    LW $3, etiqbss
    SW $3, predef
    SW $3, postdef
    SW $3, nodef
    SW $3, etiqdata
    SW $3, etiqbss
postdef:

.data
startdata:
    .byte 1
    .byte -1
    .byte 0xF
    # .byte -0xF         # => doit planter => OK
    # .byte startdata    # => doit planter => OK

    .word 1
    .word -1
    .word -32
    .word 0xFFFFFFFF   # -1
    .space 1
    .byte -1
    .word 0xF
    # .word -0xF         # => doit planter => OK
    .word 0xF , 1
    .word startdata , 1
    .word etiqdata
    .word predef
    .word etiqbss
    .word nodef

    .byte -1
    .asciiz "abcdef"
    .asciiz "1234","\n\t\\\""
    # .asciiz 10         # => doit planter => OK
    # .asciiz startdata  # => doit planter => OK
    .word -1
    .space 2
    #.byte -1

    .word -1
    # .space 5
    .byte -1
    .space 2
    #.space startdata    # => doit planter => OK


etiqdata:

.bss
    .space 0x1FFFD
    .space 2
etiqbss:
