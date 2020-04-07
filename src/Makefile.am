bin_PROGRAMS = casentino2020
casentino2020_LDADD = music_ptr_linkable.o
casentino2020_SOURCES = main.c casentino.c vectors.c queues.c acecustom.c stage1.c stage2.c physics.c matrix.c stageattraction.c collisionsmanagement.c stage3dcube.c

music_ptr_linkable.o: music_ptr_linkable.s
	vasmm68k_mot -Fhunk -quiet -esc  -DINTERLACE=0 ./music_ptr_linkable.s  -o ./music_ptr_linkable.o -I/usr/local/amiga/os-include