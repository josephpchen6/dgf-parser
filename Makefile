
all:
	g++ -std=c++17 -Wall -O2 -Iinclude -o parser "dgf_parser.cpp" -lxml2
	gcc -Wall -O2 -Iinclude -o fdvig "fdvig.c"
	gcc -Wall -O2 -Iinclude -o dkvig "dkvig.c"
	gcc -Wall -O2 -Iinclude -o pinvig "pinvig.c"
	./dkvig
	./fdvig
	./pinvig
	rm -f dkvig fdvig pinvig
	

