#ifndef CALIBRADOR_HPP
#define CALIBRADOR_HPP

#include "Vetor.hpp"       // Para Vetor<T>
#include "Estatisticas.hpp"  // Para sortperf_t (contexto)

// Definição da função template calcularNumeroQuebras movida para o header
template<typename T>
int calcularNumeroQuebras(const Vetor<T>& V, int tam) {
    if (tam <= 1) {
        return 0;
    }
    int quebras = 0;
    for (int k = 1; k < tam; ++k) {
        // Assumindo que Vetor tem operator[] e T tem operator<
        if (V[k] < V[k-1]) {
            quebras++;
        }
    }
    return quebras;
}

// Declaração de determinaLimiarParticao
template<typename T>
int determinaLimiarParticao(
    Vetor<T>& V,
    int tam,
    float a, float b, float c,
    float limiarCusto
);

// Declaração de calculaNovaFaixa (não-template)
void calculaNovaFaixa(
    int indiceMelhor,
    int numMPS,
    int mpsValues[],
    int& newMinMPS,
    int& newMaxMPS
);

// Declaração de shuffleVector
template<typename T>
void shuffleVector(
    Vetor<T>& vet,
    int size,
    int numShuffle,
    long seed
);

// Declaração de determinaLimiarQuebras
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