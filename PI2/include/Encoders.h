#ifndef ENCODERS_H
#define ENCODERS_H

#include <Arduino.h>

// Inicializa os pinos e as interrupções
void initEncoders();

// Retorna a contagem atual de pulsos
long lerPassosEsq();
long lerPassosDir();

// Reseta os contadores (útil para iniciar uma nova manobra)
void resetarEncoders();

#endif