
#ifndef ESTATISTICAS_HPP
#define ESTATISTICAS_HPP

#include <iostream>

// Estrutura de performance
typedef struct {
size_t calls;
size_t comps;
size_t moves;
} sortperf_t;

// Zera contadores
void stats_init(sortperf_t* s);
// Imprime resultados em ostream
void stats_print(const sortperf_t* s, std::ostream& os);

// Incrementadores
void inccalls(sortperf_t* s, size_t v);
void inccmp(sortperf_t* s, size_t v);
void incmove(sortperf_t* s, size_t v);

#endif // STATS_H