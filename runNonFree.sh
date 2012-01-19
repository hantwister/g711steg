#!/bin/bash
# Use a non-free application to gain more statistics
# http://www.itu.int/rec/T-REC-P.862-200102-I/en
# May require a license

if [ ! -f "${FILE}.wav" ]
then
	touch "${FILE}.lock"
        sox "${FILE}"{,.wav} 2>&1 > /dev/null
	rm -f "${FILE}.lock"
fi

sox "${PREFIX}${WORST}"{,.wav} 2>&1 > /dev/null

while [ -f "${FILE}.lock" ]
do
	sleep 5
done

../pesq/pesqmain +8000 "${FILE}.wav" "${PREFIX}${WORST}.wav" 2>&1 > "${PREFIX}${FILENOSLASH}.pesq.txt"

