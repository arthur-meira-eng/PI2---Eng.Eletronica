#line 1 "/home/yaba/Sandbox/PI2---Eng.Eletronica/PI2/include/ComunicacaoRasp.h"
#ifndef COMUNICACAO_RASP_H
#define COMUNICACAO_RASP_H

#include <Arduino.h>

// UART2 - Pinos 16 (RX) e 17 (TX)
void initComunicacao();
void lerComandos();
void enviarDados();

#endif