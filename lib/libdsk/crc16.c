
#include <stdlib.h>
#include "crc16.h"


/* CRCSUBS - CRC calculations translated from the Z80 library CRCSUBS.Z80
 * by John Elliott (for portability) 
 *
 * Original heading from CRCSUBS.Z80:
 * -----------------------------------------------------------------------
 *
 * CRCSUBS (Cyclic Redundancy Code Subroutines) for Z80 Mnemonics
 *
 * These subroutines compute and check a true 16-bit Cyclic Redundancy
 * Code for a message of arbitrary length.
 *
 *	This algorithm produces the same results as obtained in COMM700,
 *	DISK7, MDM700, and MODEM700 programs [and the LBR file format - JE].
 *
 *-----------------------------------------------------------------------
 *
 * On the Z80, we might as well use the original Z80 code for speed 
 *
 * This relies on the following calling convention:
 *
 * On entry: SP-> DW return address  DW param 1
 * 
 * On exit: Results returned in HL; AF BC DE can be altered.
 */

#ifdef z80

#asm		/* This gives a warning with GNU C in "pedantic" mode. Tough. */

	psect	text
	global _CRC_Init, _CRC_Clear, _CRC_Update, _CRC_Done, _CRC_Table

_CRC_Init:
;
; Builds the 512-byte look-up table, enter with HL containing
; buffer starting address.
;
; [JE: The original documentation stated that HL should be
;      on a page boundary (ie, L=0) but there seems to be no 
;      reason for this as all address calculations use 16-bit
;      arithmetic].
;
	pop	de	;Return address
	pop	hl	;Pointer to table
	push	hl
	push	de
	LD	(CRCTBL),HL	; ADDRESS OF THE 512-BYTE LOOK-UP TABLE
	LD	C,0		; TABLE INDEX
;
GLOOP:	EX	DE,HL
	LD	HL,0		; INITIALIZE 'CRC' REGISTER PAIR
	LD	A,C
	PUSH	BC		; SAVE THE INDEX IN 'C' REG.
	LD	B,8
	XOR	H
	LD	H,A
;
LLOOP:	ADD	HL,HL
	JR	NC,LSKIP
	LD	A,10H		; GENERATOR IS x^16 + x^12 + x^5 + x^0 AS
	XOR	H		; RECOMMENDED BY 'CCIT' FOR ASYNCHRONOUS
	LD	H,A		; COMMUNICATIONS
	LD	A,21H
	XOR	L
	LD	L,A
;
LSKIP:	DJNZ	LLOOP
	POP	BC
	EX	DE,HL		; 'DE' PAIR NOW HAS 'CRC', 'HL' POINTING
	LD	(HL),D		; INTO TABLE.  STORE HIGH BYTE OF 'CRC'
	INC	H
	LD	(HL),E		; STORE LOW BYTE
	DEC	H
	INC	HL		; MOVE TO NEXT TABLE ENTRY
	INC	C		; NEXT INDEX
	JR	NZ,GLOOP
	RET

	psect	bss
CRCVAL:	defs	2		; Running total
CRCTBL:	defs	2		; Table address
	psect	text
;
;Clear the stored CRC
;
_CRC_Clear:
	LD	HL,0
	LD	(CRCVAL),HL
	RET


; Updates the CRC value.  Finds its value by indexing into the look-up
; table.  Character arrives in the 'A' register.
; [JE] Character now arrives in L and is copied to A.
;
_CRC_Update:
	POP	DE	;Return address
	POP	HL	;Parameter
	LD	A,L
	PUSH	HL
	PUSH	DE
	LD	DE,(CRCVAL)	; PICK UP PARTIAL REMAINDER
	LD	B,0
	XOR	D
	LD	C,A
	LD	HL,(CRCTBL)	; INDEX INTO THE 512-BYTE 'CRC' TABLE
	ADD	HL,BC		; [JE] See? 16-bit pointer arithmetic
	LD	A,(HL)
	XOR	E
	LD	D,A
	INC	H
	LD	E,(HL)
	LD	(CRCVAL),DE	; STORE THE CURRENT 'CRC' VALUE
	RET
;
;Returns the calculated value.
;
_CRC_Done:
	LD	HL,(CRCVAL)
	RET
;
;Returns the table address
;
_CRC_Table:
	LD	HL,(CRCTBL)
	RET

#endasm

#else	/* def z80 */

static byte *crc_tbl;        /* Table address */
static word16 crc_val;       /* Value */

void CRC_Init(byte *table)	/* C version for non-Z80 CPUs */
{
	int idx;
	byte b;
	word16 hl;

	crc_tbl = table;
	for (idx = 0; idx < 256; idx++)
	{
		hl = (idx << 8);			
		for (b = 0; b < 8; b++)
		{
			if (hl & 0x8000) 
			{
				hl = hl << 1;
				hl ^= 0x1021;
			}
			else	
			{
				hl = hl << 1;
			}
		}				
		table[idx]       = (hl >> 8);
		table[idx + 256] = (hl & 0xFF);
	}
}

void CRC_Clear(void) 
{
	crc_val = 0;
}


void CRC_Update(byte a)
{
	int index;
	byte h,l;

	index = (a ^ (crc_val >> 8));
	
	l = crc_tbl[index];
	h = crc_tbl[index + 256];

	crc_val = (((crc_val & 0xFF) ^ l) << 8) | h;
}

word16 CRC_Done(void)
{
	return crc_val;
}

byte *CRC_Table(void)
{
	return crc_tbl;
}

#endif /* def z80 */
