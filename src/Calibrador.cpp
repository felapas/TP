#include "../include/Calibrador.hpp"
#include "../include/Vetor.hpp"
#include "../include/Estatisticas.hpp"
#include "../include/OrdenadorUniversal.hpp"
#include "../include/InsertionSort.hpp"

#include <iostream>
#include <cmath>     // For std::abs and INFINITY
#include <iomanip>   // For std::fixed and std::setprecision
#include <cstdlib>   // For drand48, srand48 (POSIX specific)
#include <limits>    // For std::numeric_limits

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

// --- Template function determinaLimiarParticao (Definição com correção no diffCusto) ---
template<typename T>
int determinaLimiarParticao(
    Vetor<T>& V,
    int tam,
    float a, float b, float c,
    float limiarCusto
) {
    int minMPS = 2;
    int maxMPS = tam;
    // Usar std::numeric_limits para INFINITY é mais portável se <limits> for incluído
    // Mas cmath::INFINITY também é comum.
    float diffCusto = std::numeric_limits<float>::infinity(); 
    int melhorMPSGlobal = (tam > 0) ? minMPS : 0;
    int iteracao = 0;

    if (tam <= 1) return 0;

    while (diffCusto > limiarCusto && (maxMPS - minMPS) >= 5) {
        std::cout << "iter " << iteracao << std::endl;
        int passoMPS = (maxMPS - minMPS) / 5;
        if (passoMPS == 0) passoMPS = 1;

        int mpsValores[6];
        float custos[6];
        int numMPSTestados = 0;

        // Gera mpsValores para testar
        for (int t = minMPS; t <= maxMPS && numMPSTestados < 6; t += passoMPS) {
            mpsValores[numMPSTestados++] = t;
        }
        // Garante que maxMPS seja testado se foi pulado e há espaço
        if (numMPSTestados > 0 && numMPSTestados < 6 && mpsValores[numMPSTestados-1] < maxMPS) {
             bool maxMPSTested = false;
             for(int k=0; k<numMPSTestados; ++k) if(mpsValores[k] == maxMPS) {maxMPSTested = true; break;}
             if(!maxMPSTested) mpsValores[numMPSTestados++] = maxMPS;
        }
        // Se o range é pequeno e o passo não gerou pontos, preenche com todos os pontos possíveis
        if (numMPSTestados == 0 && (maxMPS - minMPS) < 5 && (maxMPS-minMPS) >=0) {
            for (int t = minMPS; t <= maxMPS && numMPSTestados < 6; ++t) mpsValores[numMPSTestados++] = t;
        }
        if (numMPSTestados == 0) break; // Não pode prosseguir se nenhum MPS foi selecionado

        // Testa cada currentMPS
        for (int i = 0; i < numMPSTestados; ++i) {
            int currentMPS = mpsValores[i];
            Vetor<T> copia(V); // Trabalha em uma cópia
            sortperf_t stats;
            stats_init(&stats);
            // limiarQuebras = 0 para focar na calibração de minTamParticao
            ordenadorUniversal(copia, copia.size(), currentMPS, 0, &stats);
            custos[i] = calculaCustoDetalhado(&stats, a, b, c);
            imprimeEstatisticasMPS(currentMPS, custos[i], &stats); // Usa o helper estático
        }

        int indiceMelhorMPS = encontraIndiceMPMenorCusto(numMPSTestados, custos); // Usa o helper estático
        if (indiceMelhorMPS == -1 && numMPSTestados > 0) indiceMelhorMPS = 0; // Default para o primeiro se todos os custos forem iguais/infinitos
        else if (numMPSTestados == 0) break; // Segurança, não deveria acontecer se o if anterior pegou

        melhorMPSGlobal = mpsValores[indiceMelhorMPS];
        
        int newMinMPS_val, newMaxMPS_val;
        calculaNovaFaixa(indiceMelhorMPS, numMPSTestados, mpsValores, newMinMPS_val, newMaxMPS_val);
        
        // Atualiza minMPS e maxMPS para a *próxima* iteração (ainda não usados para diffCusto atual)
        // Estes são os valores que DEFINIRÃO a próxima faixa de busca.
        // O diffCusto deve ser sobre os custos destes valores na *iteração atual*.

        // *** CORREÇÃO DO CÁLCULO DE diffCusto ***
        float custo_no_novo_min_mps = -1.0f;
        float custo_no_novo_max_mps = -1.0f;

        // Encontra os custos correspondentes a newMinMPS_val e newMaxMPS_val
        // DENTRO dos mpsValores e custos da iteração ATUAL.
        for (int k = 0; k < numMPSTestados; ++k) {
            if (mpsValores[k] == newMinMPS_val) {
                custo_no_novo_min_mps = custos[k];
            }
            if (mpsValores[k] == newMaxMPS_val) {
                custo_no_novo_max_mps = custos[k];
            }
        }

        if (custo_no_novo_min_mps != -1.0f && custo_no_novo_max_mps != -1.0f) {
            diffCusto = std::abs(custo_no_novo_min_mps - custo_no_novo_max_mps);
        } else if (newMinMPS_val == newMaxMPS_val && numMPSTestados > 0) { 
            // Se a nova faixa colapsou para um único ponto que foi testado
            diffCusto = 0.0; // Não há diferença de custo nos extremos da nova faixa
        } else if (numMPSTestados <= 1 && iteracao > 0) {
             // Se apenas um ponto foi testado na iteração atual (e não é a primeira iteração)
             // o refinamento não pode continuar com base na diferença dos extremos.
            diffCusto = 0.0; // Força a parada ou usa limiarCusto
        }
        else {
            // Fallback: Se os custos para os novos limites não puderam ser encontrados (não deveria acontecer)
            // ou se é a primeira iteração e apenas um ponto foi testado,
            // usar a diferença dos extremos testados pode ser um fallback,
            // mas o ideal é que newMinMPS_val e newMaxMPS_val sempre estejam em mpsValores.
            // Se numMPSTestados > 1, podemos usar o diff dos extremos testados como um fallback mais robusto.
            if (numMPSTestados > 1) {
                 diffCusto = std::abs(custos[0] - custos[numMPSTestados - 1]);
            } else {
                 diffCusto = 0.0; // Para se apenas um ponto foi testado no total.
            }
        }

        // Atualiza o range para a próxima iteração
        minMPS = newMinMPS_val;
        maxMPS = newMaxMPS_val;

        // Garante limites práticos para MPS
        if (minMPS < 2) minMPS = 2;
        if (maxMPS > tam) maxMPS = tam;
        if (maxMPS < minMPS) maxMPS = minMPS; // Garante que maxMPS não seja menor que minMPS
        
        std::cout << "nummps " << numMPSTestados
                  << " limParticao " << melhorMPSGlobal // melhorMPSGlobal é o MPS com menor custo na iteração
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
    Vetor<T>& V_modelo_original_sorted, // Vetor modelo já ordenado
    int original_tam,                   // Renomeado para clareza
    float a, float b, float c,
    float limiarCustoConvergencia,
    int minTamParticao_calibrated,
    long seed
) {
    if (original_tam <= 1) {
        return 0; // Limiar não significativo para vetores muito pequenos
    }

    // Define a faixa para os valores de Limiar de Quebras (LQ) a serem testados
    int minLQ = 1;
    int maxLQ_val_para_teste = original_tam / 2; // Explora quebras até metade do tamanho do vetor

    // Garante que maxLQ não seja menor que minLQ (para tam muito pequenos)
    if (maxLQ_val_para_teste < minLQ) {
        // Isso aconteceria se original_tam < 2. Já tratado pelo primeiro if.
        // Se original_tam = 2 ou 3, maxLQ_val_para_teste = 1. Faixa [1,1].
        maxLQ_val_para_teste = minLQ; 
    }
    // Se, após ajuste, a faixa [minLQ, maxLQ_val_para_teste] for inválida ou muito pequena
    // para o loop de refinamento, o loop while pode não executar.
    int melhorLQGlobal = (minLQ <= maxLQ_val_para_teste) ? minLQ : 0;
    float minAbsDiffGlobal = std::numeric_limits<float>::infinity();
    


    // Variáveis para o loop de refinamento
    float diffVariacaoAbsDiff = std::numeric_limits<float>::infinity();
    int iteracao = 0;
    
    // Renomeia maxLQ_val_para_teste para maxLQ para uso no loop, mantendo minLQ
    int current_minLQ = minLQ;
    int current_maxLQ = maxLQ_val_para_teste;


    // Loop de refinamento iterativo
    while (diffVariacaoAbsDiff > limiarCustoConvergencia && (current_maxLQ - current_minLQ) >= 5) {
        std::cout << "iter " << iteracao << std::endl;

        int passoLQ = (current_maxLQ - current_minLQ) / 5;
        if (passoLQ == 0) {
            passoLQ = 1;
        }

        int lqValoresTest[6];
        float custosIS[6];
        float custosQS[6];
        float absDiffsCost[6];
        int numLQTestados = 0;

        // Gera valores candidatos de LQ para testar
        for (int lq_cand_val = current_minLQ; lq_cand_val <= current_maxLQ && numLQTestados < 6; lq_cand_val += passoLQ) {
            lqValoresTest[numLQTestados++] = lq_cand_val;
        }
        if (numLQTestados > 0 && numLQTestados < 6 && lqValoresTest[numLQTestados-1] < current_maxLQ) {
             bool maxLQAlreadyTested = false;
             for(int k=0; k<numLQTestados; ++k) if(lqValoresTest[k] == current_maxLQ) {maxLQAlreadyTested = true; break;}
             if(!maxLQAlreadyTested) lqValoresTest[numLQTestados++] = current_maxLQ;
        }
        if (numLQTestados == 0 && (current_maxLQ - current_minLQ) < 5 && (current_maxLQ - current_minLQ) >=0 ) {
            for (int lq_cand_val = current_minLQ; lq_cand_val <= current_maxLQ && numLQTestados < 6; ++lq_cand_val) {
                lqValoresTest[numLQTestados++] = lq_cand_val;
            }
        }
        if (numLQTestados == 0) break;


        for (int i = 0; i < numLQTestados; ++i) {
            int lq_candidate = lqValoresTest[i]; // Este é o número de shuffles
            
            Vetor<T> V_test_base(V_modelo_original_sorted);
            // USA original_tam para o tamanho do vetor
            shuffleVector(V_test_base, original_tam, lq_candidate, seed);

            // Avalia caminho QuickSort/Híbrido
            Vetor<T> V_for_QS(V_test_base);
            sortperf_t stats_qs;
            stats_init(&stats_qs);
            // USA original_tam. limiarQuebras=0 para forçar lógica de partição por tamanho.
            ordenadorUniversal(V_for_QS, original_tam, minTamParticao_calibrated, 0, &stats_qs);
            custosQS[i] = calculaCustoDetalhado(&stats_qs, a, b, c);
            std::cout << "qs lq " << lq_candidate << " cost " << std::fixed << std::setprecision(9) << custosQS[i]
                      << " cmp " << stats_qs.comps << " move " << stats_qs.moves
                      << " calls " << stats_qs.calls << std::endl;

            // Avalia caminho InsertionSort
            Vetor<T> V_for_IS(V_test_base);
            sortperf_t stats_is;
            stats_init(&stats_is);
            // USA original_tam
            insertionSort(V_for_IS, 0, original_tam - 1, &stats_is);
            custosIS[i] = calculaCustoDetalhado(&stats_is, a, b, c);
            std::cout << "in lq " << lq_candidate << " cost " << std::fixed << std::setprecision(9) << custosIS[i]
                      << " cmp " << stats_is.comps << " move " << stats_is.moves
                      << " calls " << stats_is.calls << std::endl;

            absDiffsCost[i] = std::abs(custosIS[i] - custosQS[i]);
        }

        int indiceMelhorLQ_iter = 0;
        if (numLQTestados > 0) {
            float minAbsDiff_iter_val = absDiffsCost[0];
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

        // Atualiza o melhor LQ global encontrado
        // minAbsDiffGlobal não foi inicializada corretamente antes, agora está
        if (iteracao == 0 || absDiffsCost[indiceMelhorLQ_iter] < minAbsDiffGlobal) {
            minAbsDiffGlobal = absDiffsCost[indiceMelhorLQ_iter];
            melhorLQGlobal = lq_iter_best_val;
        } else if (absDiffsCost[indiceMelhorLQ_iter] == minAbsDiffGlobal) {
            if (lq_iter_best_val < melhorLQGlobal) { // Critério de desempate: menor LQ
                melhorLQGlobal = lq_iter_best_val;
            }
        }
        
        int newMinLQ_val, newMaxLQ_val;
        calculaNovaFaixa(indiceMelhorLQ_iter, numLQTestados, lqValoresTest, newMinLQ_val, newMaxLQ_val);
        
      // Deve ser a diferença absoluta dos CUSTOS DO INSERTIONSORT (custosIS)
        // nos extremos da nova faixa de busca (newMinLQ_val, newMaxLQ_val).

        float is_cost_at_new_min_lq = -1.0f;
        float is_cost_at_new_max_lq = -1.0f;

        // Encontra os custos de InsertionSort correspondentes a newMinLQ_val e newMaxLQ_val
        // DENTRO dos lqValoresTest e custosIS da iteração ATUAL.
        for (int k = 0; k < numLQTestados; ++k) {
            if (lqValoresTest[k] == newMinLQ_val) { // newMinLQ_val foi determinado por calculaNovaFaixa
                is_cost_at_new_min_lq = custosIS[k];
            }
            if (lqValoresTest[k] == newMaxLQ_val) { // newMaxLQ_val foi determinado por calculaNovaFaixa
                is_cost_at_new_max_lq = custosIS[k];
            }
        }

        if (is_cost_at_new_min_lq != -1.0f && is_cost_at_new_max_lq != -1.0f) {
            diffVariacaoAbsDiff = std::abs(is_cost_at_new_min_lq - is_cost_at_new_max_lq);
        } else if (newMinLQ_val == newMaxLQ_val && numLQTestados > 0) { 
            // Se a nova faixa colapsou para um único ponto que foi testado
            diffVariacaoAbsDiff = 0.0; // Não há diferença de custo nos extremos da nova faixa
        } else {
            // Fallback: se os custos para os novos limites não puderam ser encontrados
            // (não deveria acontecer se calculaNovaFaixa e a lógica de busca de k funcionarem)
            // ou se poucos pontos foram testados. Usar a diferença dos extremos testados como antes.
            if (numLQTestados > 1) {
                 diffVariacaoAbsDiff = std::abs(custosIS[0] - custosIS[numLQTestados - 1]);
            } else {
                 diffVariacaoAbsDiff = 0.0; 
            }
        }

        // Atualiza o range para a próxima iteração
        current_minLQ = newMinLQ_val; // current_minLQ e current_maxLQ são os que vc usa no começo do while
        current_maxLQ = newMaxLQ_val;

        // Garante limites
        if (current_minLQ < minLQ) current_minLQ = minLQ; // Não ir abaixo do mínimo absoluto
        if (current_maxLQ > maxLQ_val_para_teste) current_maxLQ = maxLQ_val_para_teste; // Não ir acima do máximo absoluto
        if (current_maxLQ < current_minLQ) current_maxLQ = current_minLQ;
        
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