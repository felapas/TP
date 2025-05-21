#ifndef CALIBRADOR_HPP
#define CALIBRADOR_HPP

#include "Vetor.hpp"
#include "Estatisticas.hpp"
#include "OrdenadorUniversal.hpp"
#include <iostream>
#include <cmath>

template<typename T>
int determinaLimiarParticao(
    Vetor<T>& V, 
    int tam, 
    float a, 
    float b, 
    float c, 
    float limiarCusto
);

template<typename T>
void calculaNovaFaixa(
    int indiceMelhor, 
    int numMPS, 
    int mpsValues[], 
    int& newMinMPS, 
    int& newMaxMPS
);

#endif