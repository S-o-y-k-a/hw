all: complex_numbers

complex_numbers: comp_num_main.o ComplexNumber.o comp_num_sort.o
	g++ comp_num_main.o ComplexNumber.o comp_num_sort.o -o complex_numbers.o

comp_num_main.o: comp_num_main.cpp
	g++ -c comp_num_main.cpp -o comp_num_main.o

comp_num_sort.o: comp_num_sort.cpp
	g++ -c comp_num_sort.cpp -o comp_num_sort.o

ComplexNumber.o: ComplexNumber.cpp
	g++ -c ComplexNUmber.cpp -o ComplexNumber.o

clean:
	rm *.o complex_numbers

