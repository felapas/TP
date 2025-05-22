#include "../include/OrdenadorUniversal.hpp"
#include "../include/Vetor.hpp"
#include "../include/Estatisticas.hpp"
#include "../include/QuickSortMedian3.hpp" // Contém a declaração de quickSort3
#include "../include/InsertionSort.hpp"   // Contém a declaração de insertionSort

// Helper function to calculate breaks, as per problem description
// Esta função é local para este arquivo .cpp.
template<typename T>
static int calcularNumeroQuebras(const Vetor<T>& V, int tam) {
    if (tam <= 1) {
        return 0;
    }
    int quebras = 0;
    for (int k = 1; k < tam; ++k) {
        if (V[k] < V[k-1]) { // Assumindo que o operador < está definido para T
            quebras++;
        }
    }
    return quebras;
}

template<typename T>
void ordenadorUniversal(Vetor<T>& V, int tam, int minTamParticao, int limiarQuebras, sortperf_t* s) {
    // Assume-se que s é inicializado pelo chamador (ex: stats_init(s);) antes desta função.
    
    int numQuebrasAtual = 0;
    if (tam > 1) {
        numQuebrasAtual = calcularNumeroQuebras<T>(V, tam);
    }

    if (numQuebrasAtual < limiarQuebras) {
        insertionSort<T>(V, 0, tam - 1, s);
    } else {
        // Se o vetor não está "quase ordenado", decide entre QuickSort híbrido e InsertionSort
        // com base no tamanho total do vetor e minTamParticao.
        if (tam > minTamParticao) {
            // Vetor é grande o suficiente para o QuickSort (que agora é híbrido).
            // CORREÇÃO AQUI: Passar 'minTamParticao' para quickSort3.
            quickSort3<T>(V, 0, tam - 1, minTamParticao, s);
        } else {
            // Vetor é pequeno, usa InsertionSort diretamente.
            insertionSort<T>(V, 0, tam - 1, s);
        }
    }
}

// Instanciação explícita para int
template void ordenadorUniversal<int>(Vetor<int>& V, int tam, int minTamParticao, int limiarQuebras, sortperf_t* s);
// Nenhuma instanciação explícita necessária para calcularNumeroQuebras<int> pois é uma função template estática
// e será instanciada quando ordenadorUniversal<int> for.