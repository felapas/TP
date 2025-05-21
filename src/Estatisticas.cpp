
#include "../include/Estatisticas.hpp"

#include <iostream>



// Zera contadores

void stats_init(sortperf_t* s) {

    s->calls = 0;

    s->comps = 0;

    s->moves = 0;

}



// Imprime resultados em ostream

void stats_print(const sortperf_t* s, std::ostream& os) {

    os << "Calls: " << s->calls << "\n";

    os << "Comparisons: " << s->comps << "\n";

    os << "Moves: " << s->moves << "\n";

}



// Incrementadores

void inccalls(sortperf_t* s, size_t v) {

    s->calls += v;

}



void inccmp(sortperf_t* s, size_t v) {

    s->comps += v;

}



void incmove(sortperf_t* s, size_t v) {

    s->moves += v;

}
