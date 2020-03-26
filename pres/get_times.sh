for i in {4..9999}; do
	{ time ./noCache ./boards/1-boards/${i}.board; } &>> noCacheTimes2.txt
done
