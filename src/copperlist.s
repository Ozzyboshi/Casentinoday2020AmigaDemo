;	include exec/types.i
	include exec/libraries.i
;	include exec/exec_lib.i

	SECTION Ciribiri,CODE

_LVODisable	EQU	-120
_LVOEnable	EQU	-126
_LVOCloseLibrary	EQU	-414
_LVOOldOpenLibrary	EQU	-408
	
DISABLE	MACRO
	LINKLIB _LVODisable,_AbsExecBase
	ENDM
	
ENABLE	MACRO
	LINKLIB _LVOEnable,_AbsExecBase
	ENDM
	
OPENGRAPHICS MACRO
	lea GfxName,a1
	LINKLIB _LVOOldOpenLibrary,_AbsExecBase
	move.l d0,GfxBase
	ENDM
	
CLOSEGRAPHICS MACRO
	move.l GfxBase,a1
	LINKLIB _LVOCloseLibrary,_AbsExecBase
	ENDM

WAITVEND MACRO
	cmpi.b #$ff,$dff006
	bne \1
	ENDM
	
WAITVOUT MACRO
	cmpi.b #$ff,$dff006
	beq \1
	ENDM
	
CHECKMOUSEDX MACRO
	btst #2,$dff016
	bne.s \1
	bsr.s \2
	ENDM

_AbsExecBase EQU 4
 
START
	DISABLE	; Disable interrupts 
	OPENGRAPHICS	; Got on GfxBase variable the base of graphics lib
	move d0,a6
	move.l $26(a6),OldCop ; Save oldcopperlist addr
	
	; Bitplane pointing
	move.l	#PIC,d0  
	lea	BPLPOINTERS,a1
	move.w	d0,6(a1)
	swap	d0
	move.w	d0,2(a1)
	swap 	d0

	;Sprite 0 init
	MOVE.L	#MYSPRITE,d0		
	LEA	SpritePointers,a1	; Puntatori in copperlist
	move.w	d0,6(a1)
	swap	d0
	move.w	d0,2(a1)

	;Sprite 1 init (attached)	
	MOVE.L	#MYSPRITE1,d0		
	addq.w	#8,a1
	;LEA	SpritePointers,a1	
	move.w	d0,6(a1)
	swap	d0
	move.w	d0,2(a1)
	
	; let's start our custom copperlist
	move.l #COPPERLIST,$dff080
	move.l d0,$dff088
	move.l #0,$dff1fc
	move.l $c00,$dff106
	
	; enable dma sprite
	move.w #$83a0,$dff096

;	bsr.w   mt_init ; init music
	

		
mouse	
	WAITVEND mouse; Wait for 255th row
	;bsr.s draw
	;CHECKMOUSEDX lclick,rightclick
;	bsr.w mt_music
	bsr.s MoveSprite	
	
vwait	WAITVEND vwait

;lclick	btst #6,$bfe001
;	bne mouse

;	bsr.w   mt_end ; end music
	
	;move.l OldCop,$dff080
	;move.w d0,$dff088
	CLOSEGRAPHICS
	ENABLE
	rts

MoveSprite
		ADDQ.L	#1,TABXPOINT
		MOVE.L	TABXPOINT(PC),A0
		CMP.L	#FINETABX-1,A0
		BNE.S	NOBSTART
		MOVE.L	#TABX-1,TABXPOINT

NOBSTART:
	MOVE.b	(A0),HSTART
	MOVE.b	(A0),HSTART1
	rts


TABXPOINT
		dc.l	TABX-1

TABX
	dc.b	$39,$39,$39,$39,$39,$39,$39,$39,$39,$39,$39,$39 ; 200 valori
	dc.b	$5C,$5E,$61,$63,$65,$67,$69,$6B,$6E,$70,$72,$74
	dc.b	$76,$78,$7A,$7C,$7E,$80,$82,$84,$86,$88,$8A,$8C
	dc.b	$8E,$90,$92,$94,$96,$97,$99,$9B,$9D,$9E,$A0,$A2
	dc.b	$A3,$A5,$A7,$A8,$AA,$AB,$AD,$AE,$B0,$B1,$B2,$B4
	dc.b	$B5,$B6,$B8,$B9,$BA,$BB,$BD,$BE,$BF,$C0,$C1,$C2
	dc.b	$C3,$C4,$C5,$C5,$C6,$C7,$C8,$C9,$C9,$CA,$CB,$CB
	dc.b	$CC,$CC,$CD,$CD,$CE,$CE,$CE,$CF,$CF,$CF,$CF,$D0
	dc.b	$D0,$D0,$D0,$D0,$D0,$D0,$D0,$D0,$D0,$CF,$CF,$CF
	dc.b	$CF,$CE,$CE,$CE,$CD,$CD,$CC,$CC,$CB,$CB,$CA,$C9
	dc.b	$C9,$C8,$C7,$C6,$C5,$C5,$C4,$C3,$C2,$C1,$C0,$BF
	dc.b	$BE,$BD,$BB,$BA,$B9,$B8,$B6,$B5,$B4,$B2,$B1,$B0
	dc.b	$AE,$AD,$AB,$AA,$A8,$A7,$A5,$A3,$A2,$A0,$9E,$9D
	dc.b	$9B,$99,$97,$96,$94,$92,$90,$8E,$8C,$8A,$88,$86
	dc.b	$84,$82,$80,$7E,$7C,$7A,$78,$76,$74,$72,$70,$6E
	dc.b	$6B,$69,$67,$65,$63,$61,$5E,$5C,$5A,$58,$55,$53
	dc.b	$51,$4F,$4C,$4A,$48,$46,$43,$41
FINETABX
	
draw		lea PIC,a1
		add.l #1,a1
		lea image,a2
		move.b (a2)+,0(a1)
		move.b (a2)+,40(a1)
		move.b (a2)+,80(a1)

		rts	
	
rightclick
		
		lea PIC,a1
		addi.l #1,Counter
		move.l Counter,d2
loop		subq  #1,d2
		add.l #1,a1
		bne loop

		lea image,a2
		move.b (a2)+,0(a1)
		move.b (a2)+,40(a1)
		move.b (a2)+,80(a1)
		
		addq.b #1,HSTART
		addq.b #3,VSTOP
		addq.b #3,VSTART
		addq.b #1,HSTART1
		addq.b #3,VSTOP1
		addq.b #3,VSTART1
		rts	
	

Counter dc.l 1
image	dc.b %00011000
	dc.b %00111100
	dc.b %00000000
	dc.b %00000000
	
GfxBase	dc.l 0
OldCop	dc.l 0
GfxName	dc.b 'graphics.library',0,0

;	include "music.s"

	SECTION GRAPHIC,DATA_C

	
COPPERLIST	
SpritePointers	dc.w $120,$0000,$122,$0000,$124,$0000,$126,$0000,$128,$0000
		dc.w $12a,$0000,$12c,$0000,$12e,$0000,$130,$0000,$132,$0000
		dc.w $134,$0000,$136,$0000,$138,$0000,$13a,$0000,$13c,$0000
		dc.w $13e,$0000
		
		dc.w	$8e,$2c81
		dc.w	$90,$2cc1
		dc.w	$92,$0030
		dc.w	$94,$00d0
		dc.w	$102,0
		dc.w	$104,$0009
		dc.w	$108,0
		dc.w	$10a,0
		
		dc.w $100,%0001001000000000 ; BPLCON0 copperlist 

BPLPOINTERS	dc.w $00e0,$0000 ; BPL1PTH (WORD) hi ptr
		dc.w $00e2,$0000 ; BPL1PTL low ptr
		
COLORS		dc.w $0180,$fff,$0182,$f0f
		dc.w    $1a2,$111    ; color1
		dc.w    $1a4,$eee    ; color2
		dc.w    $1a6,$aaa    ; color3
		dc.w    $1a8,$070    ; color4
		dc.w    $1aa,$000    ; color5
		dc.w    $1ac,$0b0    ; color6
		dc.w    $1ae,$222    ; color7
		dc.w    $1b0,$444    ; color8
		dc.w    $1b2,$f0f    ; color9
		dc.w    $1b4,$f0f    ; color10
		dc.w    $1b6,$f0f    ; color11
		dc.w    $1b8,$f0f    ; color12
		dc.w    $1ba,$f0f    ; color13
		dc.w    $1bc,$f0f    ; color14
		dc.w    $1be,$f0f    ; color15


		;dc.w $C901,$FFFE	; WAIT - aspetto la linea $79
		;dc.w $180,$F00
		
		dc.w $FFFF,$FFFE

MYSPRITE			
VSTART		dc.b $30	
HSTART		dc.b $90	
VSTOP		dc.b $40	
		dc.b $00
		dc.w $0180,$0000
		dc.w $0040,$0000
		dc.w $0000,$0000
		dc.w $0F80,$0000
		dc.w $14C0,$0B80
		dc.w $1878,$07C0
		dc.w $1F2C,$01C0
		dc.w $3F26,$00C0
		dc.w $2F63,$10F0
		dc.w $4781,$31B8
		dc.w $43A1,$385C
		dc.w $4031,$1FCC
		dc.w $2043,$0F1C
		dc.w $1F82,$0030
		dc.w $100C,$0060
		dc.w $0FFC,$0000
		dc.w 0,0


MYSPRITE1			
VSTART1		dc.b $30	
HSTART1		dc.b $90	
VSTOP1		dc.b $40
		dc.b $80	
			dc.w $0000,$0000
			dc.w $0000,$0000
			dc.w $0000,$0000
			dc.w $0000,$0000
			dc.w $0000,$0000
			dc.w $0000,$0000
			dc.w $0018,$0000
			dc.w $001C,$0000
			dc.w $103E,$0000
			dc.w $383E,$0040
			dc.w $3C5E,$0000
			dc.w $3FCE,$0000
			dc.w $1FBE,$0000
			dc.w $007C,$0000
			dc.w $0FF8,$0000
			dc.w $0000,$0000
			dc.w 0,0



;	SECTION PLANEVUOTO,DATA_C
PIC	dcb.b 40*256,$ff		
;mt_data	incbin  "mod.purple-shades"

	end
