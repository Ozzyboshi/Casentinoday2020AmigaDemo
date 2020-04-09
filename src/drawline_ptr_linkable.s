        XDEF _Drawline
        XDEF _InitLine
        XDEF _DrawlineOr

        Section HYPERX,CODE_C


;******************************************************************************
; Questa routine effettua il disegno della linea. prende come parametri gli
; estremi della linea P1 e P2, e l'indirizzo del bitplane su cui disegnarla.
; D0 - X1 (coord. X di P1)
; D1 - Y1 (coord. Y di P1)
; D2 - X2 (coord. X di P2)
; D3 - Y2 (coord. Y di P2)
; A0 - indirizzo bitplane
;******************************************************************************

;           ("`-/")_.-'"``-._
;            . . `; -._    )-;-,_`)
;       FL  (v_,)'  _  )`-.\  ``-'
;          _.- _..-_/ / ((.'
;        ((,.-'   ((,/

_InitLine
        btst    #6,$dff002 ; dmaconr
WBlit_Init:
        btst    #6,$dff002 ; dmaconr - attendi che il blitter abbia finito
        bne.s   WBlit_Init

        moveq.l #-1,d5
        move.l  d5,$dff044              ; BLTAFWM/BLTALWM = $FFFF
        move.w  #$8000,$dff074          ; BLTADAT = $8000
        move.w  #320,$dff060             ; BLTCMOD = 320
        move.w  #320,$dff066             ; BLTDMOD = 320
        rts


_Drawline:
        move.l  mt_data,A0

        move.l 4(sp),a0
        move.l 8(sp),d0
        move.l 12(sp),d1
        
        move.l 16(sp),d2
        move.l 20(sp),d3

* scelta ottante

        sub.w   d0,d2           ; D2=X2-X1
        bmi.s   DRAW4           ; se negativo salta, altrimenti D2=DiffX
        sub.w   d1,d3           ; D3=Y2-Y1
        bmi.s   DRAW2           ; se negativo salta, altrimenti D3=DiffY
        cmp.w   d3,d2           ; confronta DiffX e DiffY
        bmi.s   DRAW1           ; se D2<D3 salta..
                                ; .. altrimenti D3=DY e D2=DX
        moveq   #$10,d5         ; codice ottante
        bra.s   DRAWL
DRAW1:
        exg.l   d2,d3           ; scambia D2 e D3, in modo che D3=DY e D2=DX
        moveq   #0,d5           ; codice ottante
        bra.s   DRAWL
DRAW2:
        neg.w   d3              ; rende D3 positivo
        cmp.w   d3,d2           ; confronta DiffX e DiffY
        bmi.s   DRAW3           ; se D2<D3 salta..
                                ; .. altrimenti D3=DY e D2=DX
        moveq   #$18,d5         ; codice ottante
        bra.s   DRAWL
DRAW3:
        exg.l   d2,d3           ; scambia D2 e D3, in modo che D3=DY e D2=DX
        moveq   #$04,d5         ; codice ottante
        bra.s   DRAWL
DRAW4:
        neg.w   d2              ; rende D2 positivo
        sub.w   d1,d3           ; D3=Y2-Y1
        bmi.s   DRAW6           ; se negativo salta, altrimenti D3=DiffY
        cmp.w   d3,d2           ; confronta DiffX e DiffY
        bmi.s   DRAW5           ; se D2<D3 salta..
                                ; .. altrimenti D3=DY e D2=DX
        moveq   #$14,d5         ; codice ottante
        bra.s   DRAWL
DRAW5:
        exg.l   d2,d3           ; scambia D2 e D3, in modo che D3=DY e D2=DX
        moveq   #$08,d5         ; codice ottante
        bra.s   DRAWL
DRAW6:
        neg.w   d3              ; rende D3 positivo
        cmp.w   d3,d2           ; confronta DiffX e DiffY
        bmi.s   DRAW7           ; se D2<D3 salta..
                                ; .. altrimenti D3=DY e D2=DX
        moveq   #$1c,d5         ; codice ottante
        bra.s   DRAWL
DRAW7:
        exg.l   d2,d3           ; scambia D2 e D3, in modo che D3=DY e D2=DX
        moveq   #$0c,d5         ; codice ottante

; Quando l'esecuzione raggiunge questo punto, abbiamo:
; D2 = DX
; D3 = DY
; D5 = codice ottante

DRAWL:
        mulu.w  #40,d1          ; offset Y
        add.l   d1,a0           ; aggiunge l'offset Y all'indirizzo

        move.w  d0,d1           ; copia la coordinata X
        and.w   #$000F,d0       ; seleziona i 4 bit piu` bassi della X..
        ror.w   #4,d0           ; .. e li sposta nei bit da 12 a 15
        or.w    #$0B4A,d0       ; con un OR ottengo il valore da scrivere
                                ; in BLTCON0. Con questo valore di LF ($4A)
                                ; si disegnano linee in EOR con lo sfondo. 

        lsr.w   #4,d1           ; cancella i 4 bit bassi della X
        add.w   d1,d1           ; ottiene l'offset X in bytes   
        add.w   d1,a0           ; aggiunge l'offset X all'indirizzo

        move.w  d2,d1           ; copia DX in D1
        addq.w  #1,d1           ; D1=DX+1
        lsl.w   #$06,d1         ; calcola in D1 il valore da mettere in BLTSIZE
        addq.w  #$0002,d1       ; aggiunge la larghezza, pari a 2 words

        lsl.w   #$02,d3         ; D3=4*DY
        add.w   d2,d2           ; D2=2*DX

        btst    #6,$dff002
WaitLine:
        btst    #6,$dff002        ; aspetta blitter fermo
        bne     WaitLine

        move.w  d3,$dff062      ; BLTBMOD=4*DY
        sub.w   d2,d3           ; D3=4*DY-2*DX
        move.w  d3,$dff052      ; BLTAPTL=4*DY-2*DX

                                ; prepara valore da scrivere in BLTCON1
        or.w    #$0001,d5       ; setta bit 0 (attiva line-mode)
        tst.w   d3
        bpl.s   OK1             ; se 4*DY-2*DX>0 salta..
        or.w    #$0040,d5       ; altrimenti setta il bit SIGN
OK1:
        move.w  d0,$dff040      ; BLTCON0
        move.w  d5,$dff042      ; BLTCON1
        sub.w   d2,d3           ; D3=4*DY-4*DX
        move.w  d3,$dff064      ; BLTAMOD=4*DY-4*DX
        move.l  a0,$dff048      ; BLTCPT - indirizzo schermo
        move.l  a0,$dff054      ; BLTDPT - indirizzo schermo
        ;move.w  #$f0f0,$dff072
        move.w  d1,$dff058      ; BLTSIZE
        rts


_DrawlineOr:
        move.l  mt_data,A0

        move.l 4(sp),a0
        move.l 8(sp),d0
        move.l 12(sp),d1
        
        move.l 16(sp),d2
        move.l 20(sp),d3

* scelta ottante

        sub.w   d0,d2           ; D2=X2-X1
        bmi.s   DRAW4OR           ; se negativo salta, altrimenti D2=DiffX
        sub.w   d1,d3           ; D3=Y2-Y1
        bmi.s   DRAW2OR           ; se negativo salta, altrimenti D3=DiffY
        cmp.w   d3,d2           ; confronta DiffX e DiffY
        bmi.s   DRAW1OR           ; se D2<D3 salta..
                                ; .. altrimenti D3=DY e D2=DX
        moveq   #$10,d5         ; codice ottante
        bra.s   DRAWLOR
DRAW1OR:
        exg.l   d2,d3           ; scambia D2 e D3, in modo che D3=DY e D2=DX
        moveq   #0,d5           ; codice ottante
        bra.s   DRAWLOR
DRAW2OR:
        neg.w   d3              ; rende D3 positivo
        cmp.w   d3,d2           ; confronta DiffX e DiffY
        bmi.s   DRAW3OR           ; se D2<D3 salta..
                                ; .. altrimenti D3=DY e D2=DX
        moveq   #$18,d5         ; codice ottante
        bra.s   DRAWLOR
DRAW3OR:
        exg.l   d2,d3           ; scambia D2 e D3, in modo che D3=DY e D2=DX
        moveq   #$04,d5         ; codice ottante
        bra.s   DRAWLOR
DRAW4OR:
        neg.w   d2              ; rende D2 positivo
        sub.w   d1,d3           ; D3=Y2-Y1
        bmi.s   DRAW6OR           ; se negativo salta, altrimenti D3=DiffY
        cmp.w   d3,d2           ; confronta DiffX e DiffY
        bmi.s   DRAW5OR           ; se D2<D3 salta..
                                ; .. altrimenti D3=DY e D2=DX
        moveq   #$14,d5         ; codice ottante
        bra.s   DRAWLOR
DRAW5OR:
        exg.l   d2,d3           ; scambia D2 e D3, in modo che D3=DY e D2=DX
        moveq   #$08,d5         ; codice ottante
        bra.s   DRAWLOR
DRAW6OR:
        neg.w   d3              ; rende D3 positivo
        cmp.w   d3,d2           ; confronta DiffX e DiffY
        bmi.s   DRAW7OR           ; se D2<D3 salta..
                                ; .. altrimenti D3=DY e D2=DX
        moveq   #$1c,d5         ; codice ottante
        bra.s   DRAWLOR
DRAW7OR:
        exg.l   d2,d3           ; scambia D2 e D3, in modo che D3=DY e D2=DX
        moveq   #$0c,d5         ; codice ottante

; Quando l'esecuzione raggiunge questo punto, abbiamo:
; D2 = DX
; D3 = DY
; D5 = codice ottante

DRAWLOR:
        mulu.w  #40,d1          ; offset Y
        add.l   d1,a0           ; aggiunge l'offset Y all'indirizzo

        move.w  d0,d1           ; copia la coordinata X
        and.w   #$000F,d0       ; seleziona i 4 bit piu` bassi della X..
        ror.w   #4,d0           ; .. e li sposta nei bit da 12 a 15
        or.w    #$0BCA,d0       ; con un OR ottengo il valore da scrivere
                                ; in BLTCON0. Con questo valore di LF ($CA)
                                ; si disegnano linee in EOR con lo sfondo. 

        lsr.w   #4,d1           ; cancella i 4 bit bassi della X
        add.w   d1,d1           ; ottiene l'offset X in bytes   
        add.w   d1,a0           ; aggiunge l'offset X all'indirizzo

        move.w  d2,d1           ; copia DX in D1
        addq.w  #1,d1           ; D1=DX+1
        lsl.w   #$06,d1         ; calcola in D1 il valore da mettere in BLTSIZE
        addq.w  #$0002,d1       ; aggiunge la larghezza, pari a 2 words

        lsl.w   #$02,d3         ; D3=4*DY
        add.w   d2,d2           ; D2=2*DX

        btst    #6,$dff002
WaitLineOR:
        btst    #6,$dff002        ; aspetta blitter fermo
        bne     WaitLineOR

        move.w  d3,$dff062      ; BLTBMOD=4*DY
        sub.w   d2,d3           ; D3=4*DY-2*DX
        move.w  d3,$dff052      ; BLTAPTL=4*DY-2*DX

                                ; prepara valore da scrivere in BLTCON1
        or.w    #$0001,d5       ; setta bit 0 (attiva line-mode)
        tst.w   d3
        bpl.s   OK1OR             ; se 4*DY-2*DX>0 salta..
        or.w    #$0040,d5       ; altrimenti setta il bit SIGN
OK1OR:
        move.w  d0,$dff040      ; BLTCON0
        move.w  d5,$dff042      ; BLTCON1
        sub.w   d2,d3           ; D3=4*DY-4*DX
        move.w  d3,$dff064      ; BLTAMOD=4*DY-4*DX
        move.l  a0,$dff048      ; BLTCPT - indirizzo schermo
        move.l  a0,$dff054      ; BLTDPT - indirizzo schermo
        move.w  d1,$dff058      ; BLTSIZE
        rts




mt_data dc.l 0

