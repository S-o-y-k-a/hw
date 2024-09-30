#include "ComplexNumber.hpp"
#include "cmp_num_sort.hpp"
#include <array>
 
void sort(ComplexNumber arr[], int size) {
	for (int i = 0; i < size; i++) {
		for (int j = 0; j < size - 1; j++) {
			if (arr[j].abs_value() > arr[j + 1].abs_value()) {
				ComplexNumber temp;
				temp = arr[j];
				arr[j] = arr[j + 1];
				arr[j + 1] = temp;
 
			}
		}
	}
}
