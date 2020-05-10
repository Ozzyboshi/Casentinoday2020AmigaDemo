CC 			= incbin.sh
CLEANFILES = uni54.h \
VampireItalialogo.raw VampireItalialogo.plt \
intermezzoV2.raw \
intermezzoV2.plt \
ball2bpl16x16_frame1.raw\
ball2bpl16x16_frame2.raw \
ball2bpl16x16_frame3.raw \
ball2bpl16x16_frame4.raw \
ball2bpl32x32_frame1.raw ball2bpl32x32_frame1_1.raw ball2bpl32x32_frame1_2.raw \
ball2bpl32x32_frame2.raw ball2bpl32x32_frame2_1.raw ball2bpl32x32_frame2_2.raw \
ball2bpl32x32_frame3.raw ball2bpl32x32_frame3_1.raw ball2bpl32x32_frame3_2.raw \
ball2bpl32x32_frame4.raw ball2bpl32x32_frame4_1.raw ball2bpl32x32_frame4_2.raw \
Aded320x224.plt Aded320x224.raw Aded320x224_1.raw Aded320x224_2.raw \
valkyrie320x244.raw valkyrie320x244palette.plt \
Aded320x224_1.h \
Aded320x224_2.h \
Aded320x224palette.h \
VampireItalialogo.h \
VampireItalialogopalette.h \
ball2bpl16x16_frame1.h \
ball2bpl16x16_frame1_palette.h \
ball2bpl16x16_frame2.h \
ball2bpl16x16_frame3.h \
ball2bpl16x16_frame4.h \
ball2bpl32x32_frame1_1.h \
ball2bpl32x32_frame1_2.h \
ball2bpl32x32_frame2_1.h \
ball2bpl32x32_frame2_2.h \
ball2bpl32x32_frame3_1.h \
ball2bpl32x32_frame3_2.h \
ball2bpl32x32_frame4_1.h \
ball2bpl32x32_frame4_2.h \
intermezzoV2.h \
intermezzoV2_palette.h \
valkyrie320x244.h \
valkyrie320x244palette.h


noinst_SCRIPTS = uni54 \
paper_cut.h \
VampireItalialogo.h VampireItalialogopalette.h \
intermezzoV2.h \
intermezzoV2_palette.h \
ball2bpl16x16_frame1.h ball2bpl16x16_frame1_palette.h \
ball2bpl16x16_frame2.h \
ball2bpl16x16_frame3.h \
ball2bpl16x16_frame4.h \
ball2bpl32x32_frame1_1.h ball2bpl32x32_frame1_2.h \
ball2bpl32x32_frame2_1.h ball2bpl32x32_frame2_2.h \
ball2bpl32x32_frame3_1.h ball2bpl32x32_frame3_2.h \
ball2bpl32x32_frame4_1.h ball2bpl32x32_frame4_2.h \
Aded320x224_1.h Aded320x224_2.h Aded320x224palette.h \
valkyrie320x244.h valkyrie320x244palette.h \
radiallinespositions.h

uni54: uni54.h
	@echo "Fonts header file generated"
uni54.h: uni54.fnt
	$(CC) $? $@ uni54

paper_cut.h: paper_cut.mod
	$(CC) $? $@ g_tPapercutMod

VampireItalialogo.raw VampireItalialogopalette.plt: VampireItalialogo.iff
	rm -f VampireItalialogo.raw VampireItalialogopalette.plt
	ilbm2raw VampireItalialogo.iff VampireItalialogo.raw -p VampireItalialogopalette.plt
VampireItalialogopalette.h: VampireItalialogopalette.plt
	$(CC) $? $@ vampireitaliapalette
VampireItalialogo.h: VampireItalialogo.raw
	$(CC) $? $@ vampireitalialogo

intermezzoV2.raw intermezzoV2_palette.plt: intermezzoV2.iff
	rm -f intermezzoV2.raw intermezzoV2_palette.plt
	ilbm2raw intermezzoV2.iff intermezzoV2.raw -p intermezzoV2_palette.plt
intermezzoV2.h: intermezzoV2.raw
	$(CC) $? $@ intermezzoV2
intermezzoV2_palette.h: intermezzoV2_palette.plt
	$(CC) $? $@ intermezzoV2_palette

ball2bpl16x16_frame1.h: ball2bpl16x16_frame1.raw
	$(CC) $? $@ ball2bpl16x16_frame1
ball2bpl16x16_frame1.raw ball2bpl16x16_frame1_palette.plt: ball2bpl16x16_frame1.iff
	rm -f ball2bpl16x16_frame1.raw ball2bpl16x16_frame1_palette.plt
	ilbm2raw ball2bpl16x16_frame1.iff ball2bpl16x16_frame1.raw -p ball2bpl16x16_frame1_palette.plt
ball2bpl16x16_frame1_palette.h: ball2bpl16x16_frame1_palette.plt
	$(CC) $? $@ ball2bpl16x16_frame1_palette

ball2bpl16x16_frame2.h: ball2bpl16x16_frame2.raw
	$(CC) $? $@ ball2bpl16x16_frame2
ball2bpl16x16_frame2.raw: ball2bpl16x16_frame2.iff
	rm -f ball2bpl16x16_frame2.raw
	ilbm2raw ball2bpl16x16_frame2.iff ball2bpl16x16_frame2.raw

ball2bpl16x16_frame3.h: ball2bpl16x16_frame3.raw
	$(CC) $? $@ ball2bpl16x16_frame3
ball2bpl16x16_frame3.raw: ball2bpl16x16_frame3.iff
	rm -f ball2bpl16x16_frame3.raw
	ilbm2raw ball2bpl16x16_frame3.iff ball2bpl16x16_frame3.raw

ball2bpl16x16_frame4.h: ball2bpl16x16_frame4.raw
	$(CC) $? $@ ball2bpl16x16_frame4
ball2bpl16x16_frame4.raw: ball2bpl16x16_frame4.iff
	rm -f ball2bpl16x16_frame4.raw
	ilbm2raw ball2bpl16x16_frame4.iff ball2bpl16x16_frame4.raw

ball2bpl32x32_frame1_1.h: ball2bpl32x32_frame1_1.raw
	$(CC) $? $@ ball2bpl32x32_frame1_1
ball2bpl32x32_frame1_2.h: ball2bpl32x32_frame1_2.raw
	$(CC) $? $@ ball2bpl32x32_frame1_2
ball2bpl32x32_frame1.raw: ball2bpl32x32_frame1.iff
	rm -f ball2bpl32x32_frame1.raw
	ilbm2raw ball2bpl32x32_frame1.iff ball2bpl32x32_frame1.raw
ball2bpl32x32_frame1_1.raw: ball2bpl32x32_frame1.raw
	rm -f $@
	../scripts/splitiff.sh ball2bpl32x32_frame1.raw ball2bpl32x32_frame1_1.raw 32 2 2 0
ball2bpl32x32_frame1_2.raw: ball2bpl32x32_frame1.raw
	rm -f $@
	../scripts/splitiff.sh ball2bpl32x32_frame1.raw ball2bpl32x32_frame1_2.raw 32 2 2 2


ball2bpl32x32_frame2_1.h: ball2bpl32x32_frame2_1.raw
	$(CC) $? $@ ball2bpl32x32_frame2_1
ball2bpl32x32_frame2_2.h: ball2bpl32x32_frame2_2.raw
	$(CC) $? $@ ball2bpl32x32_frame2_2
ball2bpl32x32_frame2.raw: ball2bpl32x32_frame2.iff
	rm -f ball2bpl32x32_frame2.raw
	ilbm2raw ball2bpl32x32_frame2.iff ball2bpl32x32_frame2.raw
ball2bpl32x32_frame2_1.raw: ball2bpl32x32_frame2.raw
	rm -f $@
	../scripts/splitiff.sh ball2bpl32x32_frame2.raw ball2bpl32x32_frame2_1.raw 32 2 2 0
ball2bpl32x32_frame2_2.raw: ball2bpl32x32_frame2.raw
	rm -f $@
	../scripts/splitiff.sh ball2bpl32x32_frame2.raw ball2bpl32x32_frame2_2.raw 32 2 2 2

ball2bpl32x32_frame3_1.h: ball2bpl32x32_frame3_1.raw
	$(CC) $? $@ ball2bpl32x32_frame3_1
ball2bpl32x32_frame3_2.h: ball2bpl32x32_frame3_2.raw
	$(CC) $? $@ ball2bpl32x32_frame3_2
ball2bpl32x32_frame3.raw: ball2bpl32x32_frame3.iff
	rm -f ball2bpl32x32_frame3.raw
	ilbm2raw ball2bpl32x32_frame3.iff ball2bpl32x32_frame3.raw
ball2bpl32x32_frame3_1.raw: ball2bpl32x32_frame3.raw
	rm -f $@
	../scripts/splitiff.sh ball2bpl32x32_frame3.raw ball2bpl32x32_frame3_1.raw 32 2 2 0
ball2bpl32x32_frame3_2.raw: ball2bpl32x32_frame3.raw
	rm -f $@
	../scripts/splitiff.sh ball2bpl32x32_frame3.raw ball2bpl32x32_frame3_2.raw 32 2 2 2

ball2bpl32x32_frame4_1.h: ball2bpl32x32_frame4_1.raw
	$(CC) $? $@ ball2bpl32x32_frame4_1
ball2bpl32x32_frame4_2.h: ball2bpl32x32_frame4_2.raw
	$(CC) $? $@ ball2bpl32x32_frame4_2
ball2bpl32x32_frame4.raw: ball2bpl32x32_frame4.iff
	rm -f ball2bpl32x32_frame4.raw
	ilbm2raw ball2bpl32x32_frame4.iff ball2bpl32x32_frame4.raw
ball2bpl32x32_frame4_1.raw: ball2bpl32x32_frame4.raw
	rm -f $@
	../scripts/splitiff.sh ball2bpl32x32_frame4.raw ball2bpl32x32_frame4_1.raw 32 2 2 0
ball2bpl32x32_frame4_2.raw: ball2bpl32x32_frame4.raw
	rm -f $@
	../scripts/splitiff.sh ball2bpl32x32_frame4.raw ball2bpl32x32_frame4_2.raw 32 2 2 2

Aded320x224_1.h: Aded320x224_1.raw
	$(CC) $? $@ Aded320x224_1
Aded320x224_2.h: Aded320x224_2.raw
	$(CC) $? $@ Aded320x224_2
Aded320x224_1.raw: Aded320x224.raw
	rm -f Aded320x224_1.raw
	../scripts/splitiff.sh Aded320x224.raw Aded320x224_1.raw 224 40 4 0
Aded320x224_2.raw: Aded320x224.raw
	rm -f Aded320x224_2.raw
	../scripts/splitiff.sh Aded320x224.raw Aded320x224_2.raw 224 40 4 40
Aded320x224.raw: Aded320x224.iff
	rm -f Aded320x224.raw Aded320x224.plt
	ilbm2raw Aded320x224.iff Aded320x224.raw
Aded320x224palette.h: Aded320x224.plt
	$(CC) $? $@ Aded320x224palette
Aded320x224.plt: Aded320x224.iff
	rm -f Aded320x224.raw Aded320x224.plt
	ilbm2raw Aded320x224.iff Aded320x224.raw -p Aded320x224.plt

valkyrie320x244.h: valkyrie320x244.raw
	$(CC) $? $@ valkyrie320x244

valkyrie320x244palette.h: valkyrie320x244palette.plt
	$(CC) $? $@ valkyrie320x244palette

valkyrie320x244.raw valkyrie320x244palette.plt: valkyrie320x244.iff
	rm -f valkyrie320x244.raw valkyrie320x244palette.plt
	ilbm2raw valkyrie320x244.iff valkyrie320x244.raw -p valkyrie320x244palette.plt

radiallinespositions.h: radiallinesallpositions.bin
	$(CC) $? $@ radiallinespositions
	