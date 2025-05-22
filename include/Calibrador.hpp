#ifndef CALIBRADOR_HPP
#define CALIBRADOR_HPP

#include "Vetor.hpp"       // For Vetor<T> type used in function signatures
#include "Estatisticas.hpp"  // For sortperf_t context

// Declares the function to determine the optimal minimum partition size for QuickSort.
template<typename T>
int determinaLimiarParticao(
    Vetor<T>& V,
    int tam,
    float a, float b, float c,
    float limiarCusto
);

// Declares calculaNovaFaixa as a NON-TEMPLATE function.
// Its definition will be in Calibrador.cpp.
void calculaNovaFaixa(
    int indiceMelhor,
    int numMPS,
    int mpsValues[],
    int& newMinMPS,
    int& newMaxMPS
);

// Declares the function to shuffle a vector.
template<typename T>
void shuffleVector(
    Vetor<T>& vet,
    int size,
    int numShuffle,
    long seed
);

// Declares the function to determine the optimal threshold for the number of breaks.
template<typename T>
int determinaLimiarQuebras(
    Vetor<T>& V_modelo_original_sorted,
    int tam,
    float a, float b, float c,
    float limiarCustoConvergencia,
    int minTamParticao_calibrated,
    long seed
);

#endif // CALIBRADOR_HPP