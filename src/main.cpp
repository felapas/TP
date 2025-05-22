#include <iostream>
#include <string>
#include <cstdlib>   // Para std::stol, std::stoi (std::stod é parte de <string>)
#include <iomanip>   // Para std::fixed, std::setprecision
#include <limits>    // Para std::numeric_limits
#include <cmath>     // Para std::isinf
#include <fstream>   // Para std::ifstream

#include "../include/Vetor.hpp"
#include "../include/Estatisticas.hpp" // Para stats_init
#include "../include/Calibrador.hpp"    // Agora contém calcularNumeroQuebras e outras declarações
#include "../include/InsertionSort.hpp" // Para ordenar V_modelo_sorted ao prepará-lo

// A função lerParametros agora recebe um std::istream& para ler
// e foi movida para dentro do main para simplificar, ou pode ser mantida separada
// se preferir, apenas ajuste para passar o ifstream.

int main(int argc, char *argv[]) {
    std::ios_base::sync_with_stdio(false);
    // std::cin.tie(NULL); // Não é mais necessário se toda a entrada vem do arquivo

    if (argc < 2) {
        std::cerr << "Erro: Nome do arquivo de entrada nao fornecido." << std::endl;
        std::cerr << "Uso: ./bin/tp1.out <arquivo_de_entrada>" << std::endl;
        return 1;
    }

    std::ifstream inputFile(argv[1]);
    if (!inputFile) {
        std::cerr << "Erro ao abrir o arquivo de entrada: " << argv[1] << std::endl;
        return 1;
    }

    long seed_val;
    double limiar_custo_convergencia;
    double coef_a, coef_b, coef_c;
    int tam_vetor;

    // Leitura dos parâmetros diretamente do inputFile
    std::string line;
    // 1. Semente
    if (!std::getline(inputFile, line)) { std::cerr << "Erro ao ler seed (EOF)." << std::endl; inputFile.close(); return 1; }
    try { seed_val = std::stol(line); } catch (const std::exception& e) { std::cerr << "Erro ao converter seed: " << e.what() << " Linha: '" << line << "'" << std::endl; inputFile.close(); return 1; }
    // 2. LimiarCusto
    if (!std::getline(inputFile, line)) { std::cerr << "Erro ao ler limiarCusto (EOF)." << std::endl; inputFile.close(); return 1; }
    try { limiar_custo_convergencia = std::stod(line); } catch (const std::exception& e) { std::cerr << "Erro ao converter limiarCusto: " << e.what() << " Linha: '" << line << "'" << std::endl; inputFile.close(); return 1; }
    // 3. Coef A
    if (!std::getline(inputFile, line)) { std::cerr << "Erro ao ler coeficiente a (EOF)." << std::endl; inputFile.close(); return 1; }
    try { coef_a = std::stod(line); } catch (const std::exception& e) { std::cerr << "Erro ao converter coeficiente a: " << e.what() << " Linha: '" << line << "'" << std::endl; inputFile.close(); return 1; }
    // 4. Coef B
    if (!std::getline(inputFile, line)) { std::cerr << "Erro ao ler coeficiente b (EOF)." << std::endl; inputFile.close(); return 1; }
    try { coef_b = std::stod(line); } catch (const std::exception& e) { std::cerr << "Erro ao converter coeficiente b: " << e.what() << " Linha: '" << line << "'" << std::endl; inputFile.close(); return 1; }
    // 5. Coef C
    if (!std::getline(inputFile, line)) { std::cerr << "Erro ao ler coeficiente c (EOF)." << std::endl; inputFile.close(); return 1; }
    try { coef_c = std::stod(line); } catch (const std::exception& e) { std::cerr << "Erro ao converter coeficiente c: " << e.what() << " Linha: '" << line << "'" << std::endl; inputFile.close(); return 1; }
    // 6. Tam
    if (!std::getline(inputFile, line)) { std::cerr << "Erro ao ler tam (EOF)." << std::endl; inputFile.close(); return 1; }
    try { tam_vetor = std::stoi(line); } catch (const std::exception& e) { std::cerr << "Erro ao converter tam: " << e.what() << " Linha: '" << line << "'" << std::endl; inputFile.close(); return 1; }

    if (tam_vetor <= 0) {
        std::cerr << "Erro: tam (Numero de Chaves) deve ser positivo. Lido: " << tam_vetor << std::endl;
        inputFile.close();
        return 1;
    }

    Vetor<int> V_modelo(tam_vetor); // Constrói com capacidade tam_vetor, size 0
    std::string line_dado;
    for (int i = 0; i < tam_vetor; ++i) {
        if (!(inputFile >> line_dado)) { // Lê cada número como string do inputFile
            std::cerr << "Erro ao ler o " << i + 1 << "-esimo elemento do vetor (EOF ou falha na leitura)." << std::endl;
            inputFile.close();
            return 1;
        }
        try {
            V_modelo.push_back(std::stoi(line_dado));
        } catch (const std::exception& e) {
            std::cerr << "Erro ao converter o " << i + 1 << "-esimo elemento do vetor: " << e.what() << " Valor lido: '" << line_dado << "'" << std::endl;
            inputFile.close();
            return 1;
        }
    }
    inputFile.close(); // Fecha o arquivo após terminar de usá-lo

    // Calcula as quebras iniciais para o cabeçalho
    int quebras_iniciais = calcularNumeroQuebras<int>(V_modelo, tam_vetor); // [cite: 167]

    // Imprime a linha de cabeçalho EXATAMENTE como no output desejado
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