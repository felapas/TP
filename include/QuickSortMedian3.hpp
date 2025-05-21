#ifndef QUICKSORTMEDIAN3_HPP
#define QUICKSORTMEDIAN3_HPP

#include "Vetor.hpp"
#include "Estatisticas.hpp"

// Declarações
template<typename T>
T median(const T& a, const T& b, const T& c);

template<typename T>
void partition3(Vetor<T>& A, int l, int r, int* i, int* j, sortperf_t* s);

template<typename T>
void quickSort3(Vetor<T>& A, int l, int r, sortperf_t* s);

// Inclui implementações de templates
#include "QuicksortMedian3.ipp"

#endif // QUICKSORTMEDIAN3_HPP
