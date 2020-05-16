version = @PACKAGE_VERSION@
TODAY=`date '+%d%m%Y'`

CLEANFILES = casentino2020.adf
noinst_DATA = casentino2020.adf

casentino2020.adf all:
	cp ../src/casentino2020 adf/
	cp ../bootimg/bootimg adf/
	exe2adf-linux64bit -l "casentino2020$(version)" -a "casentino2020_$(TODAY).adf" -d adf -k -c -w 
	echo "adf file generated"

clean:
	rm -rf *.adf