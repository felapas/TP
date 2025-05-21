#include "Calibrador.hpp"
#include <iostream>
#include <cmath>

// Implementação de determinaLimiarParticao
template<typename T>
int determinaLimiarParticao(
    Vetor<T>& V, 
    int tam, 
    float a, 
    float b, 
    float c, 
    float limiarCusto
) {
    int minMPS = 2;
    int maxMPS = tam;
    float diffCusto = INFINITY;
    int melhorMPS = minMPS;
    
    while (diffCusto > limiarCusto && (maxMPS - minMPS) >= 5) {
        int passoMPS = (maxMPS - minMPS) / 5;
        if (passoMPS == 0) passoMPS = 1;
        
        int mpsValues[6];
        float custos[6];
        int numMPS = 0;
        
        // Gera valores de mps para testar
        for (int t = minMPS; t <= maxMPS && numMPS < 6; t += passoMPS) {
            mpsValues[numMPS] = t;
            numMPS++;
        }
        
        // Testa cada mps e coleta estatísticas
        for (int i = 0; i < numMPS; ++i) {
            int currentMPS = mpsValues[i];
            Vetor<T> copia(V);
            sortperf_t stats;
            stats_init(&stats);
            
            OrdenadorUniversal(copia, copia.size(), currentMPS, 0, &stats);
            
            float custo = a * stats.comps + b * stats.moves + c * stats.calls;
            custos[i] = custo;
            
            std::cout << "mps " << currentMPS << " cost " << custo 
                      << " cmp " << stats.comps << " move " << stats.moves 
                      << " calls " << stats.calls << std::endl;
        }
        
        // Encontra o mps com menor custo
        int indiceMelhor = 0;
        for (int i = 1; i < numMPS; ++i) {
            if (custos[i] < custos[indiceMelhor]) {
                indiceMelhor = i;
            }
        }
        melhorMPS = mpsValues[indiceMelhor];
        
        // Calcula nova faixa
        int newMinMPS, newMaxMPS;
        calculaNovaFaixa(indiceMelhor, numMPS, mpsValues, newMinMPS, newMaxMPS);
        
        diffCusto = std::abs(custos[0] - custos[numMPS - 1]);
        minMPS = newMinMPS;
        maxMPS = newMaxMPS;
    }
    return melhorMPS;
}

// Implementação de calculaNovaFaixa
template<typename T>
void calculaNovaFaixa(
    int indiceMelhor, 
    int numMPS, 
    int mpsValues[], 
    int& newMinMPS, 
    int& newMaxMPS
) {
    int newMinIndex, newMaxIndex;
    
    if (indiceMelhor == 0) {
        newMinIndex = 0;
        newMaxIndex = 2;
    } else if (indiceMelhor == numMPS - 1) {
        newMinIndex = (numMPS - 3 > 0) ? numMPS - 3 : 0;
        newMaxIndex = numMPS - 1;
    } else {
        newMinIndex = indiceMelhor - 1;
        newMaxIndex = indiceMelhor + 1;
    }
    
    newMinIndex = (newMinIndex < 0) ? 0 : newMinIndex;
    newMaxIndex = (newMaxIndex > numMPS - 1) ? numMPS - 1 : newMaxIndex;
    
    newMinMPS = mpsValues[newMinIndex];
    newMaxMPS = mpsValues[newMaxIndex];
}

// Instanciação explícita para tipos int
template int determinaLimiarParticao<int>(Vetor<int>&, int, float, float, float, float);
template void calculaNovaFaixa<int>(int, int, int[], int&, int&);