
all:
	g++ -Wall -O2 -Iinclude -o parser "dgf_parser.cpp" -lxml2
	gcc -Wall -O2 -Iinclude -o dkvig "dkvig.c"
	./dkvig
	rm -f dkvig
	

