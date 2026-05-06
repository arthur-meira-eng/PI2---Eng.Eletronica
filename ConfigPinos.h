#ifndef CONFIG_PINOS_H
#define CONFIG_PINOS_H

// --- Motores (Etiquetas do Esquema) ---
#define PWM_ESQ   27
#define DIR_ESQ   26
#define PWM_DIR   25
#define DIR_DIR   33
#define PWM_CARG  32
#define DIR_CARG  12

// --- Sensores I2C (SDA=21, SCL=22) ---
#define XSHUT_1   14  // VL53L0X U25
#define XSHUT_2   13  // VL53L0X U26

// --- Sensor Ultrassônico HC-SR04 ---
#define TRIG_PIN  4
#define ECHO_PIN  16

// --- RFID RC522 (SPI) ---
#define SS_PIN    5
#define RST_PIN   22 // Ajustar se necessário com base no RST do RFID

// --- Encoders ---
#define ENCODER_A_PIN  34  // CN10
#define ENCODER_B_PIN  35  // CN11

#endif