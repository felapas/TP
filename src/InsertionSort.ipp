// InsertionSort.ipp
#ifndef INSERTIONSORT_IPP
#define INSERTIONSORT_IPP

// Implementação da função de ordenação Insertion Sort
template<typename T>
void insertionSort(Vetor<T>& v, int l, int r, sortperf_t* s) {
    inccalls(s, 1);
    for (int i = l + 1; i <= r; i++) {
        T aux = v[i];
        int j = i - 1;
        while (j >= l && aux < v[j]) {
            inccmp(s, 1);
            v[j + 1] = v[j];
            j--;
            incmove(s, 1);
        }
        v[j + 1] = aux;
        inccmp(s, 1);
        incmove(s, 2);
    }
};

#endif
