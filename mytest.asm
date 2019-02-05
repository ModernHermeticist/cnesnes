LDA #$44
LDX #$42
LDY #$40
TAX
LDA #$10
TXA
STY $10
LDA $10
TAY

times 510-($-$$) db 0
