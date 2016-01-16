all: zinetd zweb zupload zpap cltzupload cltzpap

zinetd: zinetd.o 
	gcc zinetd.o -o zinetd

zinetd.o: zinetd.c 
	gcc -c zinetd.c -o zinetd.o

zweb: zWeb.o
	gcc zWeb.o -o zweb

zweb.o: zWeb.c
	gcc -c zWeb.c -o zweb.o

zupload: zUpload.o
	gcc zUpload.o -o zupload

zupload.o: zUpload.c
	gcc -c zUpload.c -o zupload.o

zpap: zpap.o
	gcc zpap.o -o zpap

zpap.o: zpap.c
	gcc -c zpap.c -o zpap.o

cltzupload: cltzupload.o
	gcc cltzupload.o -o cltzupload

cltzupload.o: cltzUpload.c
	gcc -c cltzUpload.c -o cltzupload.o

cltzpap: cltzpap.o
	gcc cltzpap.o -o cltzpap

cltzpap.o: cltzpap.c
	gcc -c cltzpap.c -o cltzpap.o
