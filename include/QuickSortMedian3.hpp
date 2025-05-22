#ifndef QUICKSORTMEDIAN3_HPP
#define QUICKSORTMEDIAN3_HPP

#include "Vetor.hpp"
#include "Estatisticas.hpp"

// Declarações
template<typename T>
void swap(T* xp, T* yp, sortperf_t* s);

template<typename T>
T median(const T& pa, const T& pb, const T& pc, sortperf_t* s);

template<typename T>
void partition3(Vetor<T>& A, int l, int r, int* i, int* j, sortperf_t* s);


// Modificado: adicionado minTamParticao
template<typename T>
void quickSort3(Vetor<T>& A, int l, int r, int minTamParticao, sortperf_t* s);

#endif // QUICKSORTMEDIAN3_HPP