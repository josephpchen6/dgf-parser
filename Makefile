
all:
	g++ -Wall -O2 -Iinclude -o parser "dgf_parser.cpp" -lxml2
	gcc -Wall -O2 -Iinclude -o dkvig "dkvig.c"
	gcc -Wall -O2 -Iinclude -o fdvig "fdvig.c"
	./dkvig
	./fdvig
	rm -f dkvig fdvig
	

