#!/bin/bash
# This assumes you have many .al files in an audio subdirectory.
# The files will be processed in parallel.
for i in audio/*.al
do
	./runOne.sh "$i" &
done
