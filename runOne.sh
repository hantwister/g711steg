#!/bin/bash
# You may choose to comment out the various references to runNonFree.sh.

export FILE=`echo "${1}"`
export FILENOSLASH=`echo "${FILE}" | cut -f 2 -d '/'`
export WORST=`echo "${FILENOSLASH}.worst.al"`

# Run the algorithms giving a worst-case scenario
for j in 0 1 3
do
	export PREFIX=`echo "aoki-j=${j}/"`
	mkdir "${PREFIX}" 2>&1 > /dev/null
	./main-aoki -j "${j}" -s "${PREFIX}${FILENOSLASH}.avg.txt" -d "${PREFIX}${FILENOSLASH}.csv" "${FILE}" "${PREFIX}${WORST}" 2>&1 > "${PREFIX}${FILENOSLASH}.out.txt"
	./runNonFree.sh
done

for b in 16000 24000 32000 40000
do
	export PREFIX=`echo "ito-b=${b}/"`
	mkdir "${PREFIX}" 2>&1 > /dev/null
	./main-ito -b "${b}" -s "${PREFIX}${FILENOSLASH}.avg.txt" -d "${PREFIX}${FILENOSLASH}.csv" "${FILE}" "${PREFIX}${WORST}" 2>&1 > "${PREFIX}${FILENOSLASH}.out.txt"
	./runNonFree.sh
done

export PREFIX=`echo "lsb-work/"`
mkdir "${PREFIX}" 2>&1 > /dev/null
./main-lsb -s "${PREFIX}${FILENOSLASH}.avg.txt" -d "${PREFIX}${FILENOSLASH}.csv" "${FILE}" "${PREFIX}${WORST}" 2>&1 > "${PREFIX}${FILENOSLASH}.out.txt"
./runNonFree.sh

for ((k=2;k<20;k+=2))
do
	for ((l=32;l<=96;l+=16))
	do
		export PREFIX=`echo "miao-k=${k}-l=${l}/"`
		mkdir "${PREFIX}" 2>&1 > /dev/null
		./main-miao -k "${k}" -l "${l}" -s "${PREFIX}${FILENOSLASH}.avg.txt" -d "${PREFIX}${FILENOSLASH}.csv" "${FILE}" "${PREFIX}${WORST}" 2>&1 > "${PREFIX}${FILENOSLASH}.out.txt"
		./runNonFree.sh
	done
done

for b in 16000 24000 32000 40000
do
	export PREFIX=`echo "neal-b=${b}/"`
	mkdir "${PREFIX}" 2>&1 > /dev/null
	./main-neal -b "${b}" -s "${PREFIX}${FILENOSLASH}.avg.txt" -d "${PREFIX}${FILENOSLASH}.csv" "${FILE}" "${PREFIX}${WORST}" 2>&1 > "${PREFIX}${FILENOSLASH}.out.txt"
	./runNonFree.sh
done
