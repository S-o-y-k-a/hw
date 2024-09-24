#include "cmp_num_sort.hpp"
#include "ComplexNumber.hpp"
#include <iostream>
#include <array>
 
int main()
{
 
    ComplexNumber arr[3];
 
    arr[0] = ComplexNumber(5, 4);
 
    arr[1] = ComplexNumber(5, -6);
 
    std::cout << "unsorted: \n";
    for (int i = 0; i < 3; i++) {
        std::cout << arr[i] << "\n";
    }
 
    sort(arr, 3);
 
    std::cout << "sorted: \n";
    for (int i = 0; i < 3; i++) {
        std::cout << arr[i] << "\n";
    }
}
