
#ifndef QUICKSORTMEDIAN3_IPP
#define QUICKSORTMEDIAN3_IPP

#include "Estatisticas.hpp"

// Implementação da função de troca
template<typename T>
void swap(T* xp, T* yp, sortperf_t* s) {
    T temp = *xp;
    *xp = *yp;
    *yp = temp;
    incmove(s, 3);
}

// Implementação da função para calcular a mediana de 3 elementos
template<typename T>
T median(const T& a, const T& b, const T& c) {
    if ((a <= b) && (b <= c)) return b;
    if ((a <= c) && (c <= b)) return c;
    if ((b <= a) && (a <= c)) return a;
    if ((b <= c) && (c <= a)) return c;
    if ((c <= a) && (a <= b)) return a;
    return b;
}

// Implementação da função para realizar a partição usando a mediana de 3
template<typename T>
void partition3(Vetor<T>& A, int l, int r, int* i, int* j, sortperf_t* s) {
    T x = median(A[l], A[(l + r) / 2], A[r]);
    *i = l;
    *j = r;
    do {
        while (A[*i] < x) {
            inccmp(s, 1);
            (*i)++;
        }
        inccmp(s, 1);
        while (A[*j] > x) {
            inccmp(s, 1);
            (*j)--;
        }
        inccmp(s, 1);
        if (*i <= *j) {
            swap(&A[*i], &A[*j], s);
            (*i)++;
            (*j)--;
        }
    } while (*i <= *j);

    inccalls(s, 1);
}

// Implementação da função de ordenação QuickSort usando a mediana de 3
template<typename T>
void quickSort3(Vetor<T>& A, int l, int r, sortperf_t* s) {
    int i, j;
    inccalls(s, 1);
    if (l < r) {
        partition3(A, l, r, &i, &j, s);
        if (l < j) quickSort3(A, l, j, s);
        if (i < r) quickSort3(A, i, r, s);
    }
}
;
#endif