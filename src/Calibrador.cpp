#include "../include/Calibrador.hpp"
#include "../include/Vetor.hpp"
#include "../include/Estatisticas.hpp"
#include "../include/OrdenadorUniversal.hpp"
#include "../include/InsertionSort.hpp"

#include <iostream>
#include <cmath>     // For std::abs and INFINITY
#include <iomanip>   // For std::fixed and std::setprecision
#include <cstdlib>   // For drand48, srand48 (POSIX specific)

// --- Static Helper Functions (defined at the top) ---

static float calculaCustoDetalhado(const sortperf_t* s, float a, float b, float c) {
    return a * s->comps + b * s->moves + c * s->calls;
}

// This helper was used in the refactored determinaLimiarParticao.
// If determinaLimiarQuebras needs similar logic, it can use it or have its own.
static void imprimeEstatisticasMPS(int mps, float custo, const sortperf_t* s) {
    std::cout << "mps " << mps << " cost " << std::fixed << std::setprecision(9) << custo
              << " cmp " << s->comps << " move " << s->moves
              << " calls " << s->calls << std::endl;
}

// This helper was used in the refactored determinaLimiarParticao.
static int encontraIndiceMPMenorCusto(int numMPSTestados, const float custosCalculados[]) {
    if (numMPSTestados <= 0) {
        return -1;
    }
    int indiceMelhor = 0;
    for (int i = 1; i < numMPSTestados; ++i) {
        if (custosCalculados[i] < custosCalculados[indiceMelhor]) {
            indiceMelhor = i;
        }
    }
    return indiceMelhor;
}

// --- Non-template function calculaNovaFaixa (Definition) ---
// Declaration is now non-template in Calibrador.hpp
void calculaNovaFaixa(
    int indiceMelhor,
    int numMPS,
    int mpsValues[], // Array of tested values (MPS or LQ)
    int& newMin,     // Output: new minimum for the range
    int& newMax      // Output: new maximum for the range
) {
    int newMinIndex, newMaxIndex;

    if (numMPS <= 0) {
        newMin = (numMPS > 0 && mpsValues) ? mpsValues[0] : 2; // Default for MPS, adapt if for LQ
        newMax = (numMPS > 0 && mpsValues) ? mpsValues[numMPS-1] : 2;
        return;
    }
    if (numMPS == 1) {
        newMin = mpsValues[0];
        newMax = mpsValues[0];
        return;
    }

    if (indiceMelhor == 0) {
        newMinIndex = 0;
        newMaxIndex = (numMPS > 2) ? 2 : numMPS - 1;
    } else if (indiceMelhor == numMPS - 1) {
        newMinIndex = (numMPS - 3 >= 0) ? numMPS - 3 : 0;
        newMaxIndex = numMPS - 1;
    } else {
        newMinIndex = indiceMelhor - 1;
        newMaxIndex = indiceMelhor + 1;
    }

    newMinIndex = (newMinIndex < 0) ? 0 : newMinIndex;
    newMaxIndex = (newMaxIndex >= numMPS) ? numMPS - 1 : newMaxIndex;
    if (newMinIndex > newMaxIndex) newMinIndex = newMaxIndex; // Should not happen if logic above is correct

    newMin = mpsValues[newMinIndex];
    newMax = mpsValues[newMaxIndex];
}

// --- Template function determinaLimiarParticao (Definition) ---
template<typename T>
int determinaLimiarParticao(
    Vetor<T>& V,
    int tam,
    float a, float b, float c,
    float limiarCusto
) {
    int minMPS = 2;
    int maxMPS = tam;
    float diffCusto = INFINITY; // Use std::numeric_limits<float>::infinity() for more robustness if available
    int melhorMPSGlobal = (tam > 0) ? minMPS : 0; // Default if tam is 0 or 1
    int iteracao = 0;

    if (tam <= 1) return 0; // Or a sensible default for very small vectors

    while (diffCusto > limiarCusto && (maxMPS - minMPS) >= 5) { // Loop condition from previous logic
        std::cout << "iter " << iteracao << std::endl;
        int passoMPS = (maxMPS - minMPS) / 5;
        if (passoMPS == 0) passoMPS = 1;

        int mpsValores[6];
        float custos[6];
        int numMPSTestados = 0;

        for (int t = minMPS; t <= maxMPS && numMPSTestados < 6; t += passoMPS) {
            mpsValores[numMPSTestados++] = t;
        }
        if (numMPSTestados > 0 && numMPSTestados < 6 && mpsValores[numMPSTestados-1] < maxMPS) {
             bool maxMPSTested = false;
             for(int k=0; k<numMPSTestados; ++k) if(mpsValores[k] == maxMPS) {maxMPSTested = true; break;}
             if(!maxMPSTested) mpsValores[numMPSTestados++] = maxMPS;
        }
         if (numMPSTestados == 0 && (maxMPS - minMPS) < 5 && (maxMPS-minMPS) >=0) { // Range too small for step logic
            for (int t = minMPS; t <= maxMPS && numMPSTestados < 6; ++t) mpsValores[numMPSTestados++] = t;
        }
        if (numMPSTestados == 0) break;


        for (int i = 0; i < numMPSTestados; ++i) {
            int currentMPS = mpsValores[i];
            Vetor<T> copia(V);
            sortperf_t stats;
            stats_init(&stats);
            ordenadorUniversal(copia, copia.size(), currentMPS, 0, &stats); // limiarQuebras = 0 for this calibration
            custos[i] = calculaCustoDetalhado(&stats, a, b, c); // Calls static helper
            imprimeEstatisticasMPS(currentMPS, custos[i], &stats); // Calls static helper
        }

        int indiceMelhorMPS = encontraIndiceMPMenorCusto(numMPSTestados, custos); // Calls static helper
         if (indiceMelhorMPS == -1 && numMPSTestados > 0) indiceMelhorMPS = 0;
         else if (numMPSTestados == 0) break;


        melhorMPSGlobal = mpsValores[indiceMelhorMPS];
        int newMinMPS_val, newMaxMPS_val;
        calculaNovaFaixa(indiceMelhorMPS, numMPSTestados, mpsValores, newMinMPS_val, newMaxMPS_val); // Calls non-template function
        
        minMPS = newMinMPS_val;
        maxMPS = newMaxMPS_val;

        if (minMPS < 2) minMPS = 2; // Practical lower bound for MPS
        if (maxMPS > tam) maxMPS = tam;
        if (maxMPS < minMPS) maxMPS = minMPS; // Ensure range validity


        if (numMPSTestados > 1) diffCusto = std::abs(custos[0] - custos[numMPSTestados - 1]);
        else diffCusto = 0.0; // Stop if only one point tested
        
        std::cout << "nummps " << numMPSTestados
                  << " limParticao " << melhorMPSGlobal
                  << " mpsdiff " << std::fixed << std::setprecision(9) << diffCusto << std::endl;
        iteracao++;
    }
    return melhorMPSGlobal;
}


// --- Template function shuffleVector (Definition from your provided code) ---
template<typename T>
void shuffleVector(Vetor<T>& vet, int size, int numShuffle, long seed) {
    srand48(seed);
    if (size <= 1) return;
    for (int t = 0; t < numShuffle; t++) {
        int p1 = 0, p2 = 0;
        do {
            p1 = static_cast<int>(drand48() * size);
            if (p1 >= size) p1 = size - 1;
            if (p1 < 0) p1 = 0;
            p2 = static_cast<int>(drand48() * size);
            if (p2 >= size) p2 = size - 1;
            if (p2 < 0) p2 = 0;
        } while (p1 == p2 && size > 1);
        if (p1 == p2) continue;
        T temp = vet[p1];
        vet[p1] = vet[p2];
        vet[p2] = temp;
    }
}

// --- Template function determinaLimiarQuebras (Definition from your provided code) ---
// Ensure it uses the globally defined static calculaCustoDetalhado
// and the non-template calculaNovaFaixa.
template<typename T>
int determinaLimiarQuebras(
    Vetor<T>& V_modelo_original_sorted,
    int tam,
    float a, float b, float c,
    float limiarCustoConvergencia,
    int minTamParticao_calibrated,
    long seed
) {
    if (tam <= 1) {
        return 0;
    }

    int minLQ = 0;
    int maxLQ = tam - 1;

    int melhorLQGlobal = 0;
    float minAbsDiffGlobal = INFINITY; // Use std::numeric_limits<float>::infinity();

    float diffVariacaoAbsDiff = INFINITY;
    int iteracao = 0;

    while (diffVariacaoAbsDiff > limiarCustoConvergencia && (maxLQ - minLQ) >= 5) {
        std::cout << "iter " << iteracao << std::endl;

        int passoLQ = (maxLQ - minLQ) / 5;
        if (passoLQ == 0) {
            passoLQ = 1;
        }

        int lqValoresTest[6];
        float custosIS[6];
        float custosQS[6];
        float absDiffsCost[6];
        int numLQTestados = 0;

        for (int currentLQ_val = minLQ; currentLQ_val <= maxLQ && numLQTestados < 6; currentLQ_val += passoLQ) {
            lqValoresTest[numLQTestados++] = currentLQ_val;
        }
        if (numLQTestados > 0 && numLQTestados < 6 && lqValoresTest[numLQTestados-1] < maxLQ) {
             bool maxLQAlreadyTested = false;
             for(int k=0; k<numLQTestados; ++k) {
                 if(lqValoresTest[k] == maxLQ) {
                     maxLQAlreadyTested = true;
                     break;
                 }
             }
             if(!maxLQAlreadyTested) {
                lqValoresTest[numLQTestados++] = maxLQ;
             }
        }
        if (numLQTestados == 0 && (maxLQ - minLQ) < 5 && (maxLQ-minLQ) >=0 ) { // Range too small
            for (int currentLQ_val = minLQ; currentLQ_val <= maxLQ && numLQTestados < 6; ++currentLQ_val) {
                lqValoresTest[numLQTestados++] = currentLQ_val;
            }
        }
        if (numLQTestados == 0) break;


        for (int i = 0; i < numLQTestados; ++i) {
            int lq_candidate = lqValoresTest[i];
            Vetor<T> V_test_base(V_modelo_original_sorted);
            shuffleVector(V_test_base, tam, lq_candidate, seed);

            Vetor<T> V_for_QS(V_test_base);
            sortperf_t stats_qs;
            stats_init(&stats_qs);
            ordenadorUniversal(V_for_QS, tam, minTamParticao_calibrated, 0, &stats_qs);
            custosQS[i] = calculaCustoDetalhado(&stats_qs, a, b, c); // Correct: calls static helper
            std::cout << "qs lq " << lq_candidate << " cost " << std::fixed << std::setprecision(9) << custosQS[i]
                      << " cmp " << stats_qs.comps << " move " << stats_qs.moves
                      << " calls " << stats_qs.calls << std::endl;

            Vetor<T> V_for_IS(V_test_base);
            sortperf_t stats_is;
            stats_init(&stats_is);
            insertionSort(V_for_IS, 0, tam - 1, &stats_is);
            custosIS[i] = calculaCustoDetalhado(&stats_is, a, b, c); // Correct: calls static helper
            std::cout << "in lq " << lq_candidate << " cost " << std::fixed << std::setprecision(9) << custosIS[i]
                      << " cmp " << stats_is.comps << " move " << stats_is.moves
                      << " calls " << stats_is.calls << std::endl;

            absDiffsCost[i] = std::abs(custosIS[i] - custosQS[i]);
        }

        int indiceMelhorLQ_iter = 0;
        if (numLQTestados > 0) {
            float minAbsDiff_iter_val = absDiffsCost[0];
            indiceMelhorLQ_iter = 0; // Initialize assuming first is best
            for (int k = 1; k < numLQTestados; ++k) {
                if (absDiffsCost[k] < minAbsDiff_iter_val) {
                    minAbsDiff_iter_val = absDiffsCost[k];
                    indiceMelhorLQ_iter = k;
                }
            }
        } else {
             break;
        }

        int lq_iter_best_val = lqValoresTest[indiceMelhorLQ_iter];

        if (absDiffsCost[indiceMelhorLQ_iter] < minAbsDiffGlobal) {
            minAbsDiffGlobal = absDiffsCost[indiceMelhorLQ_iter];
            melhorLQGlobal = lq_iter_best_val;
        } else if (absDiffsCost[indiceMelhorLQ_iter] == minAbsDiffGlobal) {
            if (lq_iter_best_val < melhorLQGlobal) {
                melhorLQGlobal = lq_iter_best_val;
            }
        }

        int newMinLQ, newMaxLQ;
        calculaNovaFaixa(indiceMelhorLQ_iter, numLQTestados, lqValoresTest, newMinLQ, newMaxLQ); // Correct: calls non-template
        
        minLQ = newMinLQ;
        maxLQ = newMaxLQ;

        if (minLQ < 0) minLQ = 0;
        if (maxLQ >= tam) maxLQ = tam - 1;
        if (maxLQ < minLQ) maxLQ = minLQ;

        if (numLQTestados > 1) {
            diffVariacaoAbsDiff = std::abs(absDiffsCost[0] - absDiffsCost[numLQTestados - 1]);
        } else {
            diffVariacaoAbsDiff = 0.0;
        }

        std::cout << "numlq " << numLQTestados
                  << " limQuebras " << lq_iter_best_val
                  << " lqdiff " << std::fixed << std::setprecision(9) << diffVariacaoAbsDiff << std::endl;
        iteracao++;
    }
    return melhorLQGlobal;
}

// --- Explicit Instantiations (at the end of the file) ---
template int determinaLimiarParticao<int>(Vetor<int>&, int, float, float, float, float);
// No explicit instantiation for non-template calculaNovaFaixa

template void shuffleVector<int>(Vetor<int>& vet, int size, int numShuffle, long seed);
template int determinaLimiarQuebras<int>(
    Vetor<int>& V_modelo_original_sorted,
    int tam,
    float a, float b, float c,
    float limiarCustoConvergencia,
    int minTamParticao_calibrated,
    long seed
);