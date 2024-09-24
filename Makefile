all:
	g++ comp_num_main.cpp ComplexNumber.hpp ComplexNumber.cpp comp_num_sort.hpp comp_num_sort.cpp -o complex_numbers

clean:
	rm *.o complex_numbers

