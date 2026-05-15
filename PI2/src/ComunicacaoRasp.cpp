#include "../include/ComunicacaoRasp.h"
#include "../include/Motores.h"
#include "../include/ADC_ADS1115.h"
#include "../include/Encoders.h"
#include "../include/IMU_MPU6050.h"

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
    // Envia o basico: bateria, corrente e odometria para a Rasp monitorar.
    float imuX, imuY;
    lerAngulosIMU(imuX, imuY);

    Serial2.print("BAT1:"); Serial2.print(lerTensaoBateria(1));
    Serial2.print("|BAT2:"); Serial2.print(lerTensaoBateria(2));
    Serial2.print("|M1_I:"); Serial2.print(lerCorrenteMotor(1));
    Serial2.print("|M2_I:"); Serial2.print(lerCorrenteMotor(2));
    Serial2.print("|ENC_ESQ:"); Serial2.print(lerPassosEsq());
    Serial2.print("|ENC_DIR:"); Serial2.print(lerPassosDir());
    Serial2.print("|MPU_X:"); Serial2.print(imuX);
    Serial2.print("|MPU_Y:"); Serial2.println(imuY);
}
