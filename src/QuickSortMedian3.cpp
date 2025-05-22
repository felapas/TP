#include "../include/QuickSortMedian3.hpp"
#include "../include/Estatisticas.hpp"
#include "../include/Vetor.hpp"
#include "../include/InsertionSort.hpp" // Necessário para chamar insertionSort

// swap, median, partition3 permanecem os mesmos...

// Implementação da função de troca (exemplo, mantenha a sua)
template<typename T>
void swap(T* xp, T* yp, sortperf_t* s) {
    T temp = *xp;
    *xp = *yp;
    *yp = temp;
    incmove(s, 3);
}

template<typename T>
T median(const T& pa, const T& pb, const T& pc, sortperf_t* s) {
    // A lógica com if/else if/else para 3 comparações:
    inccmp(s,1); // pa < pb
    if (pa < pb) {
        inccmp(s,1); // pb < pc
        if (pb < pc) return pb; // pa < pb < pc
        else { // pa < pb AND pc <= pb
            inccmp(s,1); // pa < pc
            if (pa < pc) return pc; // pa < pc <= pb
            else return pa;       // pc <= pa < pb
        }
    } else { // pb <= pa
        inccmp(s,1); // pa < pc
        if (pa < pc) return pa; // pb <= pa < pc
        else { // pb <= pa AND pc <= pa
            inccmp(s,1); // pb < pc
            if (pb < pc) return pc; // pb < pc <= pa
            else return pb;       // pc <= pb <= pa
        }
    }
}

// Implementação da função para realizar a partição (exemplo, mantenha a sua)
template<typename T>
void partition3(Vetor<T>& A, int l, int r, int* i, int* j, sortperf_t* s) {
    T x = median(A[l], A[(l+r)/2], A[r], s); 
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
            swap<T>(&A[*i], &A[*j], s);
            (*i)++;
            (*j)--;
        }
    } while (*i <= *j);
    inccalls(s, 1); // Mantendo a lógica original do seu partition3
}


// Modificado: quickSort3 agora é híbrido
template<typename T>
void quickSort3(Vetor<T>& A, int l, int r, int minTamParticao, sortperf_t* s) {
    if (l >= r) { // Condição base: partição vazia ou com um elemento
        return;
    }

    // Se o tamanho da partição atual (r - l + 1) for menor ou igual ao limiar, use InsertionSort
    if ((r - l + 1) <= minTamParticao) {
        insertionSort(A, l, r, s); // insertionSort já incrementa suas próprias chamadas (inccalls)
    } else {
        // Caso contrário, prossiga com o particionamento do QuickSort
        inccalls(s, 1); // Incrementa chamadas para esta etapa de QuickSort
        int i, j;
        partition3<T>(A, l, r, &i, &j, s); // partition3 também incrementa chamadas na sua implementação original

        // Chamadas recursivas para as sub-partições
        if (l < j) {
            quickSort3<T>(A, l, j, minTamParticao, s);
        }
        if (i < r) {
            quickSort3<T>(A, i, r, minTamParticao, s);
        }
    }
}

// Explicit template instantiations for int
template void swap<int>(int* xp, int* yp, sortperf_t* s);
template int median<int>(const int& pa, const int& pb, const int& pc, sortperf_t* s);
template void partition3<int>(Vetor<int>& A, int l, int r, int* i, int* j, sortperf_t* s);
// Modificado: Atualizar a instanciação explícita
template void quickSort3<int>(Vetor<int>& A, int l, int r, int minTamParticao, sortperf_t* s);