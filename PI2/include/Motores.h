#ifndef MOTORES_H
#define MOTORES_H

#include <Arduino.h>

// Funções disponíveis para o software
void initMotores();
void acionarMotorEsq(int velocidade);
void acionarMotorDir(int velocidade);
void acionarMotorCarga(int velocidade, bool subir);
void pararTudo();

#endif