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

static double calculaCustoDetalhado(const sortperf_t* s, double a, double b, double c) {
    return a * s->comps + b * s->moves + c * s->calls;
}

// This helper was used in the refactored determinaLimiarParticao.
// If determinaLimiarQuebras needs similar logic, it can use it or have its own.
static void imprimeEstatisticasMPS(int mps, double custo, const sortperf_t* s) {
    std::cout << "mps " << mps << " cost " << std::fixed << std::setprecision(9) << custo
              << " cmp " << s->comps << " move " << s->moves
              << " calls " << s->calls << std::endl;
}

// This helper was used in the refactored determinaLimiarParticao.
static int encontraIndiceMPMenorCusto(int numMPSTestados, const double custosCalculados[]) {
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
    double a, double b, double c,
    double limiarCusto
) {
    int minMPS_atual = 2;
    int maxMPS_atual = tam;
    float diffCusto_atual = std::numeric_limits<float>::infinity();
    int melhorMPSGlobal = (tam > 0) ? minMPS_atual : 0;
    int iteracao = 0;

    const int MAX_PONTOS_ARRAY = 10; // Tamanho do array para mpsValores e custos
    int mpsValores[MAX_PONTOS_ARRAY];
    double custos[MAX_PONTOS_ARRAY];
    
    int numMPSTestados_iter_anterior = MAX_PONTOS_ARRAY; // Para entrar no loop na primeira vez

    if (tam <= 1) return 0;

    // Calcula o passoMPS uma vez por iteração, antes de gerar os pontos
    int passoMPS_iter = (maxMPS_atual - minMPS_atual) / 5; // 5 é o divisor do PDF
    if (passoMPS_iter == 0) passoMPS_iter = 1;

    // Loop principal: usa contagem de pontos da iteração anterior
    while (diffCusto_atual > limiarCusto && numMPSTestados_iter_anterior >= 5) {
        std::cout << "iter " << iteracao << std::endl;
        
        int numMPSTestados_nesta_iter = 0; // Zera para a iteração corrente

        // Geração de pontos simplificada, como em Ordenador.cpp
        for (int mps_cand = minMPS_atual; mps_cand <= maxMPS_atual; mps_cand += passoMPS_iter) {
            if (numMPSTestados_nesta_iter < MAX_PONTOS_ARRAY) {
                mpsValores[numMPSTestados_nesta_iter] = mps_cand;
                // O processamento do ponto (ordenação, cálculo de custo) vem depois
                numMPSTestados_nesta_iter++;
            } else {
                break; // Evita estourar o array
            }
        }
        
        // Caso especial: se o loop não gerou nenhum ponto, mas a faixa é um único ponto.
        if (numMPSTestados_nesta_iter == 0 && minMPS_atual == maxMPS_atual && MAX_PONTOS_ARRAY > 0) {
            mpsValores[numMPSTestados_nesta_iter++] = minMPS_atual;
        }
        
        // Se ainda nenhum ponto foi gerado (faixa inválida ou array muito pequeno), sair.
        if (numMPSTestados_nesta_iter == 0) {
            break;
        }

        // Testa cada currentMPS gerado
        for (int i = 0; i < numMPSTestados_nesta_iter; ++i) {
            int currentMPS = mpsValores[i];
            Vetor<T> copia(V); 
            sortperf_t stats;
            stats_init(&stats);
            // limiarQuebras = 0 para focar na calibração de minTamParticao
            ordenadorUniversal(copia, copia.size(), currentMPS, 0, &stats);
            custos[i] = calculaCustoDetalhado(&stats, a, b, c);
            imprimeEstatisticasMPS(currentMPS, custos[i], &stats); 
        }

        int indiceMelhorMPS = encontraIndiceMPMenorCusto(numMPSTestados_nesta_iter, custos);
        // Se todos os custos forem iguais/infinitos, ou se só houve um ponto.
        if (indiceMelhorMPS == -1 && numMPSTestados_nesta_iter > 0) {
             indiceMelhorMPS = 0;
        } else if (numMPSTestados_nesta_iter == 0) { // Segurança
            break;
        }
        
        melhorMPSGlobal = mpsValores[indiceMelhorMPS];
        
        // Lógica para determinar newMin_idx e newMax_idx para o cálculo de diffCusto
        // e para definir a próxima faixa [minMPS_atual, maxMPS_atual]
        // Isso espelha como Ordenador.cpp usa calculaNovaFaixa para obter índices
        // e depois usa esses índices para pegar os custos e os novos limites de mps.
        int newMin_idx, newMax_idx;
        if (numMPSTestados_nesta_iter == 1) { // Faixa colapsou ou só um ponto testado
            newMin_idx = 0;
            newMax_idx = 0;
        } else if (indiceMelhorMPS == 0) {
            newMin_idx = 0;
            newMax_idx = (numMPSTestados_nesta_iter > 2) ? 2 : numMPSTestados_nesta_iter - 1;
        } else if (indiceMelhorMPS == numMPSTestados_nesta_iter - 1) {
            newMax_idx = numMPSTestados_nesta_iter - 1;
            newMin_idx = (numMPSTestados_nesta_iter - 3 >= 0) ? numMPSTestados_nesta_iter - 3 : 0;
        } else {
            newMin_idx = indiceMelhorMPS - 1;
            newMax_idx = indiceMelhorMPS + 1;
        }
        // Ajustes de limite para os índices (importante se numMPSTestados_nesta_iter < 3)
        newMin_idx = std::max(0, newMin_idx);
        newMax_idx = std::min(numMPSTestados_nesta_iter - 1, newMax_idx);
        if (newMin_idx > newMax_idx && numMPSTestados_nesta_iter > 0) newMin_idx = newMax_idx;


        if (numMPSTestados_nesta_iter > 0) { // Apenas calcula se há pontos
            diffCusto_atual = std::abs(custos[newMin_idx] - custos[newMax_idx]);
            // Atualiza a faixa para a próxima iteração usando os VALORES dos mps nos índices determinados
            minMPS_atual = mpsValores[newMin_idx];
            maxMPS_atual = mpsValores[newMax_idx];
        } else {
            diffCusto_atual = 0.0; // Para o loop se não houver pontos
        }
        

        // Garante limites práticos para MPS para a PRÓXIMA iteração
        if (minMPS_atual < 2) minMPS_atual = 2;
        if (maxMPS_atual > tam) maxMPS_atual = tam; 
        if (maxMPS_atual < minMPS_atual) maxMPS_atual = minMPS_atual; 
        
        // Recalcula passoMPS_iter para a PRÓXIMA iteração
        passoMPS_iter = (maxMPS_atual - minMPS_atual) / 5;
        if (passoMPS_iter == 0) passoMPS_iter = 1;
        
        std::cout << "nummps " << numMPSTestados_nesta_iter
                  << " limParticao " << melhorMPSGlobal 
                  << " mpsdiff " << std::fixed << std::setprecision(6) << diffCusto_atual << std::endl;
        std::cout << std::endl; 
        
        numMPSTestados_iter_anterior = numMPSTestados_nesta_iter; // Guarda para a condição do while
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
    double a, double b, double c,
    double limiarCustoConvergencia,
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
    double minAbsDiffGlobal = std::numeric_limits<double>::infinity();
    


    // Variáveis para o loop de refinamento
    float diffVariacaoAbsDiff = std::numeric_limits<float>::infinity();
    int iteracao = 0;
    
    // Renomeia maxLQ_val_para_teste para maxLQ para uso no loop, mantendo minLQ
    int current_minLQ = minLQ;
    int current_maxLQ = maxLQ_val_para_teste;
    int numLQTestadosNaIteracaoAnterior = 6;
    
    // Loop de refinamento iterativo
    while (diffVariacaoAbsDiff > limiarCustoConvergencia && numLQTestadosNaIteracaoAnterior >= 5) {
        std::cout << "iter " << iteracao << std::endl;

        int passoLQ = (current_maxLQ - current_minLQ) / 5;
        if (passoLQ == 0) {
            passoLQ = 1;
        }
        const int MAX_LQ_PONTOS_TESTE = 100;
        int lqValoresTest[MAX_LQ_PONTOS_TESTE];
        double custosIS[MAX_LQ_PONTOS_TESTE];
        double custosQS[MAX_LQ_PONTOS_TESTE];
        double absDiffsCost[MAX_LQ_PONTOS_TESTE];
        int numLQTestados = 0;

        // Gera valores candidatos de LQ para testar
        for (int lq_cand_val = current_minLQ; lq_cand_val <= current_maxLQ; lq_cand_val += passoLQ) {
    if (numLQTestados < MAX_LQ_PONTOS_TESTE) {
        lqValoresTest[numLQTestados++] = lq_cand_val;
    } else {
        // Se o número de candidatos exceder o limite, não adiciona mais
        break;
    }
}
        if (numLQTestados > 0 && numLQTestados < 6 && lqValoresTest[numLQTestados-1] < current_maxLQ) {
             bool maxLQAlreadyTested = false;
             for(int k=0; k<numLQTestados; ++k) if(lqValoresTest[k] == current_maxLQ) {maxLQAlreadyTested = true; break;}
             if(!maxLQAlreadyTested) lqValoresTest[numLQTestados++] = current_maxLQ;
        }
        if (numLQTestados == 0 && (current_maxLQ - current_minLQ) < 5 && (current_maxLQ - current_minLQ) >= 0) {
    // Se passoLQ era > (max-min), o loop acima pode não ter rodado.
    // Preenche com todos os pontos possíveis na faixa pequena, respeitando o limite do array.
    int tempPasso = 1; // Para faixas pequenas, testar todos os pontos
    for (int lq_cand_val = current_minLQ; lq_cand_val <= current_maxLQ; lq_cand_val += tempPasso) {
        if (numLQTestados < MAX_LQ_PONTOS_TESTE) {
            lqValoresTest[numLQTestados++] = lq_cand_val;
        } else {
            break;
        }
    }
}

        if (numLQTestados == 0 && current_minLQ <= current_maxLQ) {
    if (numLQTestados < MAX_LQ_PONTOS_TESTE) { // Verifica se há espaço
       lqValoresTest[numLQTestados++] = current_minLQ;
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
            double minAbsDiff_iter_val = absDiffsCost[0];
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

        double is_cost_at_new_min_lq = -1.0f;
        double is_cost_at_new_max_lq = -1.0f;

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

        numLQTestadosNaIteracaoAnterior = numLQTestados;

        // Garante limites
        if (current_minLQ < minLQ) current_minLQ = minLQ; // Não ir abaixo do mínimo absoluto
        if (current_maxLQ > maxLQ_val_para_teste) current_maxLQ = maxLQ_val_para_teste; // Não ir acima do máximo absoluto
        if (current_maxLQ < current_minLQ) current_maxLQ = current_minLQ;
        
        std::cout << "numlq " << numLQTestados
                  << " limQuebras " << lq_iter_best_val 
                  << " lqdiff " << std::fixed << std::setprecision(6) << diffVariacaoAbsDiff << std::endl;

        if (diffVariacaoAbsDiff > limiarCustoConvergencia || numLQTestadosNaIteracaoAnterior > 5) {
            std::cout << std::endl; 
        }
        iteracao++;
    }
    return melhorLQGlobal;
}


// --- Explicit Instantiations (at the end of the file) ---
template int determinaLimiarParticao<int>(Vetor<int>&, int, double, double, double, double);
// No explicit instantiation for non-template calculaNovaFaixa

template void shuffleVector<int>(Vetor<int>& vet, int size, int numShuffle, long seed);
template int determinaLimiarQuebras<int>(
    Vetor<int>& V_modelo_original_sorted,
    int tam,
    double a, double b, double c,
    double limiarCustoConvergencia,
    int minTamParticao_calibrated,
    long seed
);