#include "../include/InsertionSort.hpp"
#include "../include/Vetor.hpp"       // For Vetor<T>
#include "../include/Estatisticas.hpp" // For sortperf_t and inc functions

// Implementação da função de ordenação Insertion Sort
template<typename T>
void insertionSort(Vetor<T>& v, int l, int r, sortperf_t* s) {
    inccalls(s, 1); // Count the call to insertionSort itself
    for (int i = l + 1; i <= r; i++) {
        T aux = v[i]; // This move is accounted for in incmove(s, 2) at the end
        int j = i - 1;

        // Shift elements of v[l..i-1], that are greater than aux,
        // to one position ahead of their current position
        while (j >= l && aux < v[j]) {
            inccmp(s, 1);      // Count comparison if (j >= l && aux < v[j]) is true
            v[j + 1] = v[j];
            incmove(s, 1);     // Count this shift
            j--;
        }
        v[j + 1] = aux;      // Place aux in its correct sorted position
        
        // This comparison is for the case that terminates the while loop
        // (either j < l, or aux >= v[j] if j >=l ).
        inccmp(s, 1);
        
        // These two moves are for:
        // 1. T aux = v[i]; (loading the element to be inserted)
        // 2. v[j + 1] = aux; (placing the element in its sorted position)
        incmove(s, 2);
    }
}

// Explicit template instantiation for int
template void insertionSort<int>(Vetor<int>& v, int l, int r, sortperf_t* s);