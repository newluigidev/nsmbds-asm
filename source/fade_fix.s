@ Fading fix for New Super Mario Bros. DS
@ Nintendo managed to make the bottom screen not complete the fade, ending with a darker color.

nsub_02008058:
	LDRB    R0, [R7,#0x5BC]
	TST     R0, #2
	BEQ     .return

	LDR     R0, =0x4001050
	MOV     R1, #63
	MOV     R2, #0
	BL      G2x_SetBlendBrightness_

.return:
	LDRB    R0, [R7,#0x5BC]
	AND     R0, R0, #1
	B       0x0200805C
