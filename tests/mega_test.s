# TEST_RETURN_CODE = PASS
.set noreorder

.text
predef:
    ADD $5, $t2, $a2
    adD $5, $t2, $a2
    ADDI $5, $t2, 500
    ADDI $5, $t2, 0254
    ADDI $5, $t2, 0x452    #gestion moins hexa pas bonne
    ADDI $5, $t2, -1
    ADDI $5, $t2, predef
    ADDI $5, $t2, postdef
    ADDI $5, $t2, nodef     # TODO corrigé erreur    etiq reste type 7 (label)
    ADDI $5, $t2, etiqdata  # TODO coorigé erreur    etis reeste type 7 (label)
    ADDI $5, $t2, etiqbss     # TODO coorigé erreur    etis reeste type 7 (label)
                                # Imm sur arg 3 mal géré ?
    SUB $5, $t2, $a2
    MULT $5, $t2
    DIV $5, $t2
    AND $5, $t2, $a2
    OR $5, $t2, $a2
    XOR $5, $t2, $a2
    ROTR $5, $t2, 31
    # ROTR $5, $t2, 32 # doit planter  => OK
    # ROTR $5, $t2, -1 # doit planter  => OK
    SLL $5, $t2, 2
    SRL $5, $t2, 2
    SLT $5, $t2, $a2

    LW $5, 8($4)
    LW $5, -8($4)
    # LW $5, 0xFFFFFFFF($4) #gestion moins hexa pas bonne
    SW $5, 8($4)
    SW $5, -8($4)

    LUI $5 , 64
    LUI $5 , -2
    LUI $5 , 32767
    LUI $5 , -32768
    # LUI $5 , 0xFFFFFFFF  #gestion moins hexa pas bonne
    LUI $5 , predef
    LUI $5 , postdef
    LUI $5 , nodef
    LUI $5 , startdata
    LUI $5 , etiqbss

    MFHI $t5
    MFLO $t7

    #BEQ $5, $t2, 131070
    # BEQ $5, $t2, 0xFF     #doit planter (pas %4)    => OK
    BEQ $5, $t2, 0xFC
    #BEQ $5, $t2, -10         #doit planter (pas %4)    => OK
    BEQ $5, $t2, predef
    BEQ $5, $t2, postdef
    # BEQ $5, $t2, nodef       #doit planter    => OK
    # BEQ $5, $t2, startdata   #doit planter    => OK
    BNE $5, $t2, 4
    # BNE $5, $t2, -10     #doit planter (pas %4)    => OK
    BNE $5, $t2, predef
    BNE $5, $t2, postdef
    # BNE $5, $t2, nodef   #doit planter    => OK
    BGTZ $5, 4
    # BGTZ $5, -10          #doit planter    => OK
    BGTZ $5, predef
    BGTZ $5, postdef
    # BGTZ $5, nodef       #doit planter    => OK
    BLEZ $5, 4
    # BLEZ $5, -10         #doit planter (pas %4)    => OK
    BLEZ $5, predef
    BLEZ $5, postdef
    # BLEZ $5, nodef       #doit planter    => OK

    J 8
    # J -8 #erreur normalement ???
    J 0x3FC
    #J 0x3FD    #doit planter    => OK
    J predef
    J postdef
    J nodef
    J etiqdata
    J etiqbss
    JAL 8
    ## JAL -8 #erreur normalement ???
    JAL 0x3FC
    JAL predef
    JAL postdef
    JAL nodef
    JAL etiqdata
    JAL etiqbss
    JR $5

    #### pseudo instruction ####
    NOP
    MOVE $5, $t2
    NEG $5, $t2

    LI $5 , 64
    LI $5 , -2
    LI $5 , 32767
    LI $5 , -32768
    ## LI $5 , 0xFFFFFFFF  #gestion moins hexa pas bonne
    LI $5 , predef    #  => OK
    LI $5 , postdef
    #LI $5 , nodef     # TODO corrigé erreur    etiq reste type 7 (label)
    #LI $5 , etiqdata  # TODO corrigé erreur    etiq reste type 7 (label)
    #LI $5 , etiqbss   # TODO corrigé erreur    etiq reste type 7 (label)

    BLT $5, $t2, 4
    BLT $5, $t2, 0xFC
    # BLT $5, $t2, -10  #doit planter (pas %4)    => OK
    BLT $5, $t2, predef
    BLT $5, $t2, postdef
    # BLT $5, $t2, nodef      #doit planter    => OK
    # BLT $5, $t2, etiqdata   #doit planter    => OK

    LW $5, predef
    LW $5, postdef
    LW $5, nodef
    LW $5, etiqdata
    LW $5, etiqbss
    SW $5, predef
    SW $5, postdef
    SW $5, nodef
    SW $5, etiqdata
    SW $5, etiqbss
postdef:

.data
startdata:
    .byte 1
    .byte -1
    .byte 0xF
    # .byte startdata    # => doit planter => OK

    .word 1
    .word -1
    .word 0xF
    .word 0xF , 1
    .word startdata , 1
    .word etiqdata
    .word predef
    .word etiqbss
    .word nodef

    .asciiz "abcd"
    .asciiz "1234","\n\t\\\""
    # .asciiz 10         # => doit planter => OK
    # .asciiz startdata  # => doit planter => OK

    .space 2
    #.space startdata    # => doit planter => OK

etiqdata:

.bss
    .space 2
etiqbss:
