all:
	vasmm68k_mot -devpac -Fhunk -esc bootimg.s -o bootimg.o
	vlink bootimg.o -o bootimg
clean:
	rm -f bootimg.o bootimg
