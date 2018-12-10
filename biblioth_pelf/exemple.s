#TEST_RETURN_CODE = PASS
.set noreorder
.text
ADD $2, $3, $4
boucle : ADDI $2, $3, boucle
BEQ $2, $3, boucle
.data
.word tab
.bss
.space 0x10
tab: .space 8
