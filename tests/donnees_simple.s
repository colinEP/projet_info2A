# TEST_RETURN_CODE=PASS


.data 
text1: .asciiz "j'adore les textes avec des accents ê é ǜ, des symboles $ µ % \\, des MAJUSCULES et du code assembleur .asciiz text1:", "\n"
.asciiz "$µ%"
.word 0x10000000
#.asciiz "est-ce que le .word est bien \"aligné\" ?" 
#.word 0x00000000
