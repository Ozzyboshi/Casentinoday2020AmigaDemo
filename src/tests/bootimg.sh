root@99b3a0faba50:/data/src# vasmm68k_mot -Fhunk -quiet -esc  -DINTERLACE=0 ./bootimg.s  -o ./bootimg.o -I/usr/local/amiga/os-include
root@99b3a0faba50:/data/src# vlink bootimg.o  -o bootimg
vasmm68k_mot -Fhunk -quiet -esc  -DINTERLACE=0 ./copperlist.s  -o ./bootimg.o -I/opt/amiga/m68k-amigaos/ndk-include
