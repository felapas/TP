#ifndef ORDENADORUNIVERSAL_HPP
#define ORDENADORUNIVERSAL_HPP

#include "Vetor.hpp"
#include "Estatisticas.hpp"
#include "QuickSortMedian3.hpp"
#include "InsertionSort.hpp"


template<typename T>
void ordenadorUniversal(Vetor<T>& V, int tam, int minTamParticao, int limiarQuebras, sortperf_t* s);

#endif // ORDENADORUNIVERSAL_HPP