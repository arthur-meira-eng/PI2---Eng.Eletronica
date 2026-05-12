#include "../include/ComunicacaoRasp.h"
#include "../include/Motores.h"
#include "../include/ADC_ADS1115.h"

void initComunicacao() {
    Serial2.begin(115200, SERIAL_8N1, 16, 17);
}

void lerComandos() {
    if (Serial2.available() > 0) {
        String msg = Serial2.readStringUntil('\n');
        msg.trim();

        // O software pode enviar "PARAR", "FRENTE", etc.
        if (msg == "PARAR") {
            pararTudo();
        }
    }
}

void enviarDados() {
    // Envia o básico: Bateria e Corrente para a Rasp monitorar
    Serial2.print("BAT1:"); Serial2.print(lerTensaoBateria(1));
    Serial2.print("|M1_I:"); Serial2.println(lerCorrenteMotor(1));
}