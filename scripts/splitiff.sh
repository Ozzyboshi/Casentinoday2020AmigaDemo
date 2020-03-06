#!/bin/bash

# syntax: splitiff.sh infile outfile heightimg chunkwidth bitplanes skipoffset

#ex ./splitiff.sh Aded320x224.raw Aded320x224img7.raw 224 40 4 40
# ./splitiff.sh Aded320x224.raw Aded320x224img1.raw 224 40 4 0

set -e

INFILE=$1
OUTFILE=$2

if ! test -f "$INFILE"; then
    echo "$INFILE does not exist"
    exit 1
fi

if test -f "$OUTFILE"; then
    echo "$OUTFILE already exists"
    exit 1
fi

START=0
END=$(($3*$5))
READCHUNK=$4
SKIPOFFSET=$(($6))

#echo $SKIPOFFSET
#FILESIZE=$(stat -c%s "$1")
#echo "FILESIZE is $FILESIZE"
#ROWLENGTH=$(($FILESIZE/$END))
#echo "ROWLENGTH : $ROWLENGTH"
 
for (( c=$START; c<$END; c++ ))
do
	# echo -n "$c "
	SKIP=$(($READCHUNK*$c+READCHUNK*$c+$SKIPOFFSET))
	# echo "dd if=$INFILE of=$OUTFILE bs=1 ibs=1 obs=1 skip=$SKIP count=$READCHUNK conv=notrunc  oflag=append"
	dd if="$INFILE" of="$OUTFILE" bs=1 ibs=1 obs=1 skip="$SKIP" count="$READCHUNK" conv=notrunc  oflag=append status=none
	# sleep 1
done
 

