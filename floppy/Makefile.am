CLEANFILES = casentino2020.adf

noinst_DATA = casentino2020.adf

casentino2020.adf all:
	cp ../src/casentino2020 adf/
	exe2adf-linux64bit -l casentino2020 -a casentino2020.adf -d adf -k -c -w -0
	echo "adf file generated"