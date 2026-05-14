#ifndef CONFIG_PINOS_H
#define CONFIG_PINOS_H

// Use 1 para testar somente o RFID sem inicializar sensores I2C/motores.
// Necessario quando o RST do RC522 esta no GPIO 22, que tambem e o SCL padrao.
#define MODO_TESTE_RFID 0

// Use 1 para testar somente o sensor ultrassonico HC-SR04.
#define MODO_TESTE_ULTRASSOM 0

// Use 1 para testar somente o MPU6050 no barramento I2C.
#define MODO_TESTE_MPU 1

// --- Motores (Etiquetas do Esquema) ---
#define PWM_ESQ   27
#define DIR_ESQ   26
#define PWM_DIR   25
#define DIR_DIR   33
#define PWM_CARG  32
#define DIR_CARG  12

// --- Sensores I2C (SDA=21, SCL=22) ---
#define I2C_SDA   21
#define I2C_SCL   22
#define XSHUT_1   14  // VL53L0X U25
#define XSHUT_2   13  // VL53L0X U26

// --- Sensor Ultrassônico HC-SR04 ---
#define TRIG_PIN  18
#define ECHO_PIN  5

// --- RFID RC522 (SPI) ---
#define SS_PIN    5

#if MODO_TESTE_RFID
    #define RST_PIN   22
#else
    #define RST_PIN   17
#endif

#define IRQ_PIN   4
#define RFID_USAR_IRQ 1

// --- Encoders ---
#define ENCODER_A_PIN  34  // CN10
#define ENCODER_B_PIN  35  // CN11

#endif
