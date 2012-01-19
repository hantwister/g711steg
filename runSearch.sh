for i in audio/*.al miao-*/*.al
do
	./miao-search -l 2 -u 20 "${i}" > "${i}.miaosearch.txt"
done
