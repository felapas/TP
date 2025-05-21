#ifndef INSERTIONSORT_HPP
#define INSERTIONSORT_HPP

#include "Vetor.hpp"
#include "Estatisticas.hpp"

// Declaração da função de ordenação Insertion Sort
template<typename T>
void insertionSort(Vetor<T>& v, int l, int r, sortperf_t* s);

// Inclui implementações do template
#include "InsertionSort.ipp"

#endif // INSERTIONSORT_HPP
