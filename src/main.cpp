#include <iostream>
#include <string>
#include <cstdlib>   // Para std::stol, std::stof, std::stoi
#include <iomanip>   // Para std::fixed, std::setprecision (usado em Calibrador.cpp)
#include <limits>    // Para std::numeric_limits
#include <cmath>     // Para std::isinf (usado em lerParametros)

#include "../include/Vetor.hpp"
#include "../include/Estatisticas.hpp" // Para stats_init
#include "../include/Calibrador.hpp"    // Agora contém calcularNumeroQuebras e outras declarações
#include "../include/InsertionSort.hpp" // Para ordenar V_modelo_sorted ao prepará-lo

// A função lerParametros permanece a mesma da sua última versão funcional
bool lerParametros(long& seed, float& limiarCusto, float& a, float& b, float& c, int& tam) {
    std::string line;
    // 1. Semente
    if (!std::getline(std::cin, line)) { std::cerr << "Erro ao ler seed (EOF)." << std::endl; return false; }
    try { seed = std::stol(line); } catch (const std::exception& e) { std::cerr << "Erro ao converter seed: " << e.what() << " Linha: '" << line << "'" << std::endl; return false; }
    // 2. LimiarCusto
    if (!std::getline(std::cin, line)) { std::cerr << "Erro ao ler limiarCusto (EOF)." << std::endl; return false; }
    try { limiarCusto = std::stof(line); } catch (const std::exception& e) { std::cerr << "Erro ao converter limiarCusto: " << e.what() << " Linha: '" << line << "'" << std::endl; return false; }
    // 3. Coef A
    if (!std::getline(std::cin, line)) { std::cerr << "Erro ao ler coeficiente a (EOF)." << std::endl; return false; }
    try { a = std::stof(line); } catch (const std::exception& e) { std::cerr << "Erro ao converter coeficiente a: " << e.what() << " Linha: '" << line << "'" << std::endl; return false; }
    // 4. Coef B
    if (!std::getline(std::cin, line)) { std::cerr << "Erro ao ler coeficiente b (EOF)." << std::endl; return false; }
    try { b = std::stof(line); } catch (const std::exception& e) { std::cerr << "Erro ao converter coeficiente b: " << e.what() << " Linha: '" << line << "'" << std::endl; return false; }
    // 5. Coef C
    if (!std::getline(std::cin, line)) { std::cerr << "Erro ao ler coeficiente c (EOF)." << std::endl; return false; }
    try { c = std::stof(line); } catch (const std::exception& e) { std::cerr << "Erro ao converter coeficiente c: " << e.what() << " Linha: '" << line << "'" << std::endl; return false; }
    // 6. Tam
    if (!std::getline(std::cin, line)) { std::cerr << "Erro ao ler tam (EOF)." << std::endl; return false; }
    try { tam = std::stoi(line); } catch (const std::exception& e) { std::cerr << "Erro ao converter tam: " << e.what() << " Linha: '" << line << "'" << std::endl; return false; }

    if (tam <= 0) {
        std::cerr << "Erro: tam (Numero de Chaves) deve ser positivo. Lido: " << tam << std::endl;
        return false;
    }
    // Silenciar o aviso de tam pequeno para a saída final
    // if (tam < 6 && (std::isinf(limiarCusto) || limiarCusto > 0) ) {
    //      std::cerr << "Aviso: tam (" << tam << ") e pequeno..." << std::endl;
    // }
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

    Vetor<int> V_modelo(tam_vetor); // Constrói com capacidade tam_vetor, size 0
    std::string line_dado;
    for (int i = 0; i < tam_vetor; ++i) {
        if (!(std::cin >> line_dado)) { // Lê cada número como string
            std::cerr << "Erro ao ler o " << i + 1 << "-esimo elemento do vetor (EOF ou falha na leitura)." << std::endl;
            return 1;
        }
        try {
            V_modelo.push_back(std::stoi(line_dado));
        } catch (const std::exception& e) {
            std::cerr << "Erro ao converter o " << i + 1 << "-esimo elemento do vetor: " << e.what() << " Valor lido: '" << line_dado << "'" << std::endl;
            return 1;
        }
    }

    // Calcula as quebras iniciais para o cabeçalho
    int quebras_iniciais = calcularNumeroQuebras<int>(V_modelo, tam_vetor);

    // Imprime a linha de cabeçalho EXATAMENTE como no output desejado
    // (incluindo duas quebras de linha depois, se for o caso)
    std::cout << "size " << tam_vetor << " seed " << seed_val << " breaks " << quebras_iniciais << std::endl << std::endl;

    // Prepara V_modelo_sorted para determinaLimiarQuebras
    Vetor<int> V_modelo_sorted(V_modelo); // Construtor de cópia
    sortperf_t stats_para_ordenar_modelo; 
    stats_init(&stats_para_ordenar_modelo);
    if (V_modelo_sorted.size() > 0) {
        insertionSort(V_modelo_sorted, 0, V_modelo_sorted.size() - 1, &stats_para_ordenar_modelo);
    }

    // Chama determinaLimiarParticao - ele imprimirá sua própria saída iterativa
    int minTamParticao_calibrated = determinaLimiarParticao<int>(V_modelo, tam_vetor, coef_a, coef_b, coef_c, limiar_custo_convergencia);
    
    // Adiciona uma linha em branco entre as duas seções de calibração, se presente no output desejado.
    // O "output desejado" que você me mostrou não tem uma linha em branco aqui,
    // ele vai direto do final de determinaLimiarParticao para o "iter 0" de determinaLimiarQuebras.
    // Se a sua referência tiver, adicione: std::cout << std::endl;

    // Chama determinaLimiarQuebras - ele imprimirá sua própria saída iterativa
    /* int limiarQuebras_calibrated = */ determinaLimiarQuebras<int>( // O valor de retorno não é impresso no formato final
        V_modelo_sorted,
        tam_vetor,
        coef_a, coef_b, coef_c,
        limiar_custo_convergencia,
        minTamParticao_calibrated,
        seed_val
    );

    // Nenhuma saída adicional do main após as funções de calibração.

    return 0;
}