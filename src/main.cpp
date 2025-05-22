#include <iostream>
#include <string>
#include <vector>    // Temporariamente para ler, depois copiar para Vetor<T>
#include <algorithm> // Para std::sort, se usado para V_modelo_sorted
#include <cstdlib>   // Para std::stol, std::stof, std::stoi
#include <limits>    // Para std::numeric_limits (alternativa a INFINITY de cmath)


#include "../include/Vetor.hpp"
#include "../include/Estatisticas.hpp"
#include "../include/Calibrador.hpp"
#include "../include/InsertionSort.hpp" // Para ordenar V_modelo_sorted
#include "../include/QuickSortMedian3.hpp" // Alternativa para ordenar

// Função lerParametros permanece a mesma...
bool lerParametros(long& seed, float& limiarCusto, float& a, float& b, float& c, int& tam) {
    std::string line;

    // 1. Semente aleatória
    if (!std::getline(std::cin, line)) { std::cerr << "Erro ao ler seed (EOF)." << std::endl; return false; }
    try {
        seed = std::stol(line);
    } catch (const std::exception& e) {
        std::cerr << "Erro ao converter seed: " << e.what() << " Linha: '" << line << "'" << std::endl;
        return false;
    }

    // 2. Limiar de Convergência
    if (!std::getline(std::cin, line)) { std::cerr << "Erro ao ler limiarCusto (EOF)." << std::endl; return false; }
    try {
        limiarCusto = std::stof(line);
    } catch (const std::exception& e) {
        std::cerr << "Erro ao converter limiarCusto: " << e.what() << " Linha: '" << line << "'" << std::endl;
        return false;
    }

    // 3. Coeficiente das Comparações (a)
    if (!std::getline(std::cin, line)) { std::cerr << "Erro ao ler coeficiente a (EOF)." << std::endl; return false; }
    try {
        a = std::stof(line);
    } catch (const std::exception& e) {
        std::cerr << "Erro ao converter coeficiente a: " << e.what() << " Linha: '" << line << "'" << std::endl;
        return false;
    }

    // 4. Coeficiente das Movimentações (b)
    if (!std::getline(std::cin, line)) { std::cerr << "Erro ao ler coeficiente b (EOF)." << std::endl; return false; }
    try {
        b = std::stof(line);
    } catch (const std::exception& e) {
        std::cerr << "Erro ao converter coeficiente b: " << e.what() << " Linha: '" << line << "'" << std::endl;
        return false;
    }

    // 5. Coeficiente das Chamadas (c)
    if (!std::getline(std::cin, line)) { std::cerr << "Erro ao ler coeficiente c (EOF)." << std::endl; return false; }
    try {
        c = std::stof(line);
    } catch (const std::exception& e) {
        std::cerr << "Erro ao converter coeficiente c: " << e.what() << " Linha: '" << line << "'" << std::endl;
        return false;
    }

    // 6. Número de Chaves (tam)
    if (!std::getline(std::cin, line)) { std::cerr << "Erro ao ler tam (EOF)." << std::endl; return false; }
    try {
        tam = std::stoi(line);
    } catch (const std::exception& e) {
        std::cerr << "Erro ao converter tam: " << e.what() << " Linha: '" << line << "'" << std::endl;
        return false;
    }

    if (tam <= 0) {
        std::cerr << "Erro: tam (Numero de Chaves) deve ser positivo. Lido: " << tam << std::endl;
        return false;
    }
    if (tam < 6 && (std::isinf(limiarCusto) || limiarCusto > 0) ) {
         std::cerr << "Aviso: tam (" << tam << ") e pequeno, a calibracao pode nao ser efetiva ou nao executar os loops de refinamento." << std::endl;
    }
    return true;
}


int main() {
    std::ios_base::sync_with_stdio(false);
    std::cin.tie(NULL);

    long seed_val;
    float limiar_custo_convergencia;
    float coef_a, coef_b, coef_c;
    int tam_vetor;

    if (!lerParametros(seed_val, limiar_custo_convergencia, coef_a, coef_b, coef_c, tam_vetor)) {
        return 1;
    }

    // MODIFICADO: Ler os dados do vetor V_modelo da entrada padrão
    Vetor<int> V_modelo(tam_vetor); // Constrói com capacidade tam_vetor, size 0
    std::string line_dado;
    for (int i = 0; i < tam_vetor; ++i) {
        if (!(std::cin >> line_dado)) { // Lê cada número como string para conversão robusta
            std::cerr << "Erro ao ler o " << i+1 << "-esimo elemento do vetor (EOF ou falha)." << std::endl;
            return 1;
        }
        try {
            V_modelo.push_back(std::stoi(line_dado));
        } catch (const std::exception& e) {
            std::cerr << "Erro ao converter o " << i+1 << "-esimo elemento do vetor: " << e.what() << " Linha: '" << line_dado << "'" << std::endl;
            return 1;
        }
    }
    // Consumir o restante da linha após o último número, se houver (ex: um newline)
    // std::getline(std::cin, line_dado); // Opcional, pode não ser necessário com `std::cin >>`

    // Prepare V_modelo_sorted: copie V_modelo e ordene-o
    Vetor<int> V_modelo_sorted(V_modelo); // Usa o construtor de cópia do seu Vetor
    
    sortperf_t stats_para_ordenar_modelo; // Stats temporários para ordenar o modelo
    stats_init(&stats_para_ordenar_modelo);
    // Você pode usar qualquer um dos seus algoritmos de ordenação para preparar V_modelo_sorted.
    // O quickSort3 híbrido pode ser uma boa escolha, ou insertionSort se tam_vetor for pequeno.
    // Usando insertionSort para simplicidade aqui, ou quickSort3 se preferir:
    if (V_modelo_sorted.size() > 0) { // Só ordene se não estiver vazio
        // Para quickSort3, precisamos de um minTamParticao_temp. Um valor pequeno como 10-20 é razoável.
        // Ou, como esta ordenação é apenas uma preparação, podemos usar um mais simples.
        insertionSort(V_modelo_sorted, 0, V_modelo_sorted.size() - 1, &stats_para_ordenar_modelo);
    }


    // --- Calibrar minTamParticao ---
    std::cout << "CALIBRACAO DO LIMIAR DE PARTICAO MINIMA (minTamParticao)" << std::endl;
    std::cout << "-------------------------------------------------------" << std::endl;
    int minTamParticao_calibrated = determinaLimiarParticao<int>(V_modelo, tam_vetor, coef_a, coef_b, coef_c, limiar_custo_convergencia);
    std::cout << "-------------------------------------------------------" << std::endl;

    // --- Calibrar limiarQuebras ---
    std::cout << "CALIBRACAO DO LIMIAR DE QUEBRAS (limiarQuebras)" << std::endl;
    std::cout << "----------------------------------------------" << std::endl;
    int limiarQuebras_calibrated = determinaLimiarQuebras<int>(
        V_modelo_sorted, // Passa o vetor ordenado
        tam_vetor,
        coef_a, coef_b, coef_c,
        limiar_custo_convergencia,
        minTamParticao_calibrated,
        seed_val
    );
    std::cout << "----------------------------------------------" << std::endl;

    // Final summary
    std::cout << "Calibracao concluida." << std::endl;
    std::cout << "MinTamParticao Determinado: " << minTamParticao_calibrated << std::endl;
    std::cout << "LimiarQuebras Determinado: " << limiarQuebras_calibrated << std::endl;

    return 0;
}