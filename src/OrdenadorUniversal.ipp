#ifndef ORDENADORUNIVERSAL_IPP
#define ORDENADORUNIVERSAL_IPP

#include "OrdenadorUniversal.hpp"

template<typename T>
void ordenadorUniversal(Vetor<T>& V, int tam, int minTamParticao, int limiarQuebras, sortperf_t* s) {
    if (s->numeroQuebras < limiarQuebras) {
        insertionSort(V, tam, s); 
    } else {
        if (tam > minTamParticao) {
            quickSortMedian3(V, tam, s); 
        } else {
            insertionSort(V, tam, s); 
        }
    }
}

#endif