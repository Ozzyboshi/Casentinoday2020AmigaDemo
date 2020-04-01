; Lezione4b.s   VISUALIZZAZIONE DI UNA FIGURA IN 320*256 a 3 plane (8 colori)

	SECTION CODE,CODE_C

Inizio:
        move.l  4.w,a6          ; Execbase in a6
        jsr     -$78(a6)        ; Disable - ferma il multitasking
        lea     GfxName(PC),a1  ; Indirizzo del nome della lib da aprire in a1
        jsr     -$198(a6)       ; OpenLibrary
        move.l  d0,GfxBase      ; salvo l'indirizzo base GFX in GfxBase
        move.l  d0,a6
        move.l  $26(a6),OldCop  ; salviamo l'indirizzo della copperlist vecchia

;*****************************************************************************
;       FACCIAMO PUNTARE I BPLPOINTERS NELLA COPPELIST AI NOSTRI BITPLANES
;*****************************************************************************


        MOVE.L  #PIC,d0         ; in d0 mettiamo l'indirizzo della PIC,
                                ; ossia dove inizia il primo bitplane

        LEA     BPLPOINTERS,A1  ; in a1 mettiamo l'indirizzo dei
                                ; puntatori ai planes della COPPERLIST
        MOVEQ   #2,D1           ; numero di bitplanes -1 (qua sono 3)
                                ; per eseguire il ciclo col DBRA
POINTBP:
        move.w  d0,6(a1)        ; copia la word BASSA dell'indirizzo del plane
                                ; nella word giusta nella copperlist
        swap    d0              ; scambia le 2 word di d0 (es: 1234 > 3412)  
                                ; mettendo la word ALTA al posto di quella   
                                ; BASSA, permettendone la copia col move.w!! 
        move.w  d0,2(a1)        ; copia la word ALTA dell'indirizzo del plane
                                ; nella word giusta nella copperlist
        swap    d0              ; scambia le 2 word di d0 (es: 3412 > 1234)
                                ; rimettendo a posto l'indirizzo.
        ADD.L   #40*256,d0      ; Aggiungiamo 10240 ad D0, facendolo puntare  
                                ; al secondo bitplane (si trova dopo il primo)
                                ; (cioe' aggiungiamo la lunghezza di un plane)
                                ; Nei cicli seguenti al primo faremo puntare
                                ; al terzo, al quarto bitplane eccetera.

        addq.w  #8,a1           ; a1 ora contiene l'indirizzo dei prossimi    
                                ; bplpointers nella copperlist da scrivere.   
        dbra    d1,POINTBP      ; Rifai D1 volte POINTBP (D1=num of bitplanes)
 
;

        move.l  #COPPERLIST,$dff080     ; Puntiamo la nostra COP 
        move.w  d0,$dff088              ; Facciamo partire la COP

        move.w  #0,$dff1fc              ; FMODE - Disattiva l'AGA  
        move.w  #$c00,$dff106           ; BPLCON3 - Disattiva l'AGA
mouse:
        btst    #6,$bfe001      ; tasto sinistro del mouse premuto?
        bne.s   mouse           ; se no, torna a mouse:

        move.l  OldCop(PC),$dff080      ; Puntiamo la cop di sistema
        move.w  d0,$dff088              ; facciamo partire la vecchia cop

        move.l  4.w,a6
        jsr     -$7e(a6)        ; Enable - riabilita il Multitasking
        move.l  GfxBase(PC),a1  ; Base della libreria da chiudere   
        jsr     -$19e(a6)       ; Closelibrary - chiudo la graphics lib
        rts                     ; USCITA DAL PROGRAMMA

;       Dati

GfxName:
        dc.b    "graphics.library",0,0

GfxBase:                ; Qua ci va l'indirizzo di base per gli Offset
        dc.l    0       ; della graphics.library

OldCop:                 ; Qua ci va l'indirizzo della vecchia COP di sistema
        dc.l    0

        SECTION GRAPHIC,DATA_C

COPPERLIST:

        ; Facciamo puntare gli sprite a ZERO, per eliminarli, o ce li troviamo
        ; in giro impazziti a disturbare!!!

        dc.w    $120,$0000,$122,$0000,$124,$0000,$126,$0000,$128,$0000
        dc.w    $12a,$0000,$12c,$0000,$12e,$0000,$130,$0000,$132,$0000
        dc.w    $134,$0000,$136,$0000,$138,$0000,$13a,$0000,$13c,$0000
        dc.w    $13e,$0000

        dc.w    $8e,$2c81       ; DiwStrt       (registri con valori normali)
        dc.w    $90,$2cc1       ; DiwStop 
        dc.w    $92,$0038       ; DdfStart
        dc.w    $94,$00d0       ; DdfStop
        dc.w    $102,0          ; BplCon1
        dc.w    $104,0          ; BplCon2
        dc.w    $108,0          ; Bpl1Mod
        dc.w    $10a,0          ; Bpl2Mod

; il BPLCON0 ($dff100) Per uno schermo a 3 bitplanes: (8 colori)

                    ; 5432109876543210
        dc.w    $100,%0011001000000000  ; bits 13 e 12 accesi!! (3 = %011)

;       Facciamo puntare i bitplanes direttamente mettendo nella copperlist
;       i registri $dff0e0 e seguenti qua di seguito con gli indirizzi
;       dei bitplanes che saranno messi dalla routine POINTBP

BPLPOINTERS:
        dc.w $e0,$0000,$e2,$0000        ;primo   bitplane - BPL0PT
        dc.w $e4,$0000,$e6,$0000        ;secondo bitplane - BPL1PT
        dc.w $e8,$0000,$ea,$0000        ;terzo   bitplane - BPL2PT

;       Gli 8 colori della figura sono definiti qui:

        dc.w    $0180,$000      ; color0
        dc.w    $0182,$475      ; color1
        dc.w    $0184,$fff      ; color2
        dc.w    $0186,$ccc      ; color3
        dc.w    $0188,$999      ; color4
        dc.w    $018a,$232      ; color5
        dc.w    $018c,$777      ; color6
        dc.w    $018e,$444      ; color7

;       Inserite qua eventuali effetti coi WAIT

        dc.w    $FFFF,$FFFE     ; Fine della copperlist
;       Ricordatevi di selezionare la directory dove si trova la figura
;       in questo caso basta scrivere: "V df0:SORGENTI2"


PIC:
        incbin  "crow_img_8.raw"       ; qua carichiamo la figura in RAW,
                                        ; convertita col KEFCON, fatta di
                                        ; 3 bitplanes consecutivi

        end
