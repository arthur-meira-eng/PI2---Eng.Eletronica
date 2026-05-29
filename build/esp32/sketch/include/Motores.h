#line 1 "/Users/yaba/Sandbox/PI2---Eng.Eletronica/PI2/include/Motores.h"
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