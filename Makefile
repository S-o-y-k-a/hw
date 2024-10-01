all: read-file

read-file: read-file.cpp
	g++ read-file.cpp -o read-file

clear:
	rm read-file
