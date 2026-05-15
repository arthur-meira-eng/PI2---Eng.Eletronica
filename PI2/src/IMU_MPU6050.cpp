#include "../include/IMU_MPU6050.h"
#include <math.h>

Adafruit_MPU6050 mpu;
static bool imuOk = false;
static bool kalmanInicializado = false;

static constexpr float Q_ANGLE = 0.001;
static constexpr float Q_BIAS = 0.003;
static constexpr float R_MEASURE = 0.03;
static constexpr float ALPHA_TEMP = 0.10;
static constexpr float RAD_TO_DEG_F = 57.2957795;

struct Kalman1D {
    float angle = 0.0;
    float bias = 0.0;
    float p00 = 0.0;
    float p01 = 0.0;
    float p10 = 0.0;
    float p11 = 0.0;
};

static Kalman1D kalmanEixoX;
static Kalman1D kalmanEixoY;
static unsigned long ultimoMicrosIMU = 0;

static float accBrutoX = 0.0;
static float accBrutoY = 0.0;
static float accBrutoZ = 0.0;
static float gyroBrutoX = 0.0;
static float gyroBrutoY = 0.0;
static float gyroBrutoZ = 0.0;
static float gyroCorrigidoX = 0.0;
static float gyroCorrigidoY = 0.0;
static float gyroCorrigidoZ = 0.0;
static float anguloAccelX = 0.0;
static float anguloAccelY = 0.0;
static float anguloKalmanX = 0.0;
static float anguloKalmanY = 0.0;
static float tempFiltrada = 0.0;
static float ultimoDt = 0.0;

static float filtrarExponencial(float valorAtual, float valorAnterior, float alpha) {
    return (alpha * valorAtual) + ((1.0 - alpha) * valorAnterior);
}

static float atualizarKalman(Kalman1D &estado, float anguloMedido, float gyroRate, float dt) {
    float rate = gyroRate - estado.bias;
    estado.angle += dt * rate;

    estado.p00 += dt * (dt * estado.p11 - estado.p01 - estado.p10 + Q_ANGLE);
    estado.p01 -= dt * estado.p11;
    estado.p10 -= dt * estado.p11;
    estado.p11 += Q_BIAS * dt;

    float s = estado.p00 + R_MEASURE;
    float k0 = estado.p00 / s;
    float k1 = estado.p10 / s;
    float erro = anguloMedido - estado.angle;

    estado.angle += k0 * erro;
    estado.bias += k1 * erro;

    float p00Temp = estado.p00;
    float p01Temp = estado.p01;

    estado.p00 -= k0 * p00Temp;
    estado.p01 -= k0 * p01Temp;
    estado.p10 -= k1 * p00Temp;
    estado.p11 -= k1 * p01Temp;

    return estado.angle;
}

static void calcularAngulosAcelerometro(float accX, float accY, float accZ,
                                        float &anguloX, float &anguloY) {
    anguloX = atan2(accY, accZ) * RAD_TO_DEG_F;
    anguloY = atan2(-accX, sqrt((accY * accY) + (accZ * accZ))) * RAD_TO_DEG_F;
}

static void inicializarKalmanComAcelerometro(const sensors_event_t &a, const sensors_event_t &temp) {
    calcularAngulosAcelerometro(a.acceleration.x, a.acceleration.y, a.acceleration.z,
                                anguloAccelX, anguloAccelY);

    kalmanEixoX = Kalman1D();
    kalmanEixoY = Kalman1D();
    kalmanEixoX.angle = anguloAccelX;
    kalmanEixoY.angle = anguloAccelY;
    anguloKalmanX = anguloAccelX;
    anguloKalmanY = anguloAccelY;
    tempFiltrada = temp.temperature;
    ultimoMicrosIMU = micros();
    ultimoDt = 0.0;
    kalmanInicializado = true;
}

static void atualizarIMU() {
    if (!imuOk) {
        return;
    }

    sensors_event_t a, g, temp;
    mpu.getEvent(&a, &g, &temp);

    accBrutoX = a.acceleration.x;
    accBrutoY = a.acceleration.y;
    accBrutoZ = a.acceleration.z;
    gyroBrutoX = g.gyro.x;
    gyroBrutoY = g.gyro.y;
    gyroBrutoZ = g.gyro.z;

    if (!kalmanInicializado) {
        inicializarKalmanComAcelerometro(a, temp);
        gyroCorrigidoX = gyroBrutoX;
        gyroCorrigidoY = gyroBrutoY;
        gyroCorrigidoZ = gyroBrutoZ;
        return;
    }

    unsigned long agora = micros();
    ultimoDt = (agora - ultimoMicrosIMU) / 1000000.0;
    ultimoMicrosIMU = agora;

    if (ultimoDt <= 0.0 || ultimoDt > 0.2) {
        ultimoDt = 0.01;
    }

    calcularAngulosAcelerometro(accBrutoX, accBrutoY, accBrutoZ, anguloAccelX, anguloAccelY);

    float gyroRateX = gyroBrutoX * RAD_TO_DEG_F;
    float gyroRateY = gyroBrutoY * RAD_TO_DEG_F;

    anguloKalmanX = atualizarKalman(kalmanEixoX, anguloAccelX, gyroRateX, ultimoDt);
    anguloKalmanY = atualizarKalman(kalmanEixoY, anguloAccelY, gyroRateY, ultimoDt);

    gyroCorrigidoX = (gyroRateX - kalmanEixoX.bias) / RAD_TO_DEG_F;
    gyroCorrigidoY = (gyroRateY - kalmanEixoY.bias) / RAD_TO_DEG_F;
    gyroCorrigidoZ = gyroBrutoZ;
    tempFiltrada = filtrarExponencial(temp.temperature, tempFiltrada, ALPHA_TEMP);
}

static const char *rangeAcelerometroTexto(mpu6050_accel_range_t range) {
    switch (range) {
        case MPU6050_RANGE_2_G: return "+-2g";
        case MPU6050_RANGE_4_G: return "+-4g";
        case MPU6050_RANGE_8_G: return "+-8g";
        case MPU6050_RANGE_16_G: return "+-16g";
        default: return "desconhecido";
    }
}

static const char *rangeGiroscopioTexto(mpu6050_gyro_range_t range) {
    switch (range) {
        case MPU6050_RANGE_250_DEG: return "+-250 deg/s";
        case MPU6050_RANGE_500_DEG: return "+-500 deg/s";
        case MPU6050_RANGE_1000_DEG: return "+-1000 deg/s";
        case MPU6050_RANGE_2000_DEG: return "+-2000 deg/s";
        default: return "desconhecido";
    }
}

static const char *larguraFiltroTexto(mpu6050_bandwidth_t bandwidth) {
    switch (bandwidth) {
        case MPU6050_BAND_260_HZ: return "260 Hz";
        case MPU6050_BAND_184_HZ: return "184 Hz";
        case MPU6050_BAND_94_HZ: return "94 Hz";
        case MPU6050_BAND_44_HZ: return "44 Hz";
        case MPU6050_BAND_21_HZ: return "21 Hz";
        case MPU6050_BAND_10_HZ: return "10 Hz";
        case MPU6050_BAND_5_HZ: return "5 Hz";
        default: return "desconhecido";
    }
}

void initIMU() {
    Serial.println("[MPU] Inicializando MPU6050...");
    Serial.println("[MPU] Endereco esperado: 0x68 (ou 0x69 se AD0 estiver em HIGH).");

    Wire.beginTransmission(0x68);
    uint8_t status68 = Wire.endTransmission();
    Wire.beginTransmission(0x69);
    uint8_t status69 = Wire.endTransmission();
    Serial.printf("[MPU] Scan I2C -> 0x68: %s (%u) | 0x69: %s (%u)\n",
                  status68 == 0 ? "OK" : "sem resposta", status68,
                  status69 == 0 ? "OK" : "sem resposta", status69);

    if (!mpu.begin()) {
        imuOk = false;
        kalmanInicializado = false;
        Serial.println("[MPU] ERRO: falha ao encontrar MPU6050.");
        Serial.println("[MPU] Verifique VCC 3.3V, GND, SDA=GPIO 21, SCL=GPIO 22 e pull-ups do I2C.");
        return;
    }

    imuOk = true;
    kalmanInicializado = false;
    mpu.setAccelerometerRange(MPU6050_RANGE_8_G);
    mpu.setGyroRange(MPU6050_RANGE_500_DEG);
    mpu.setFilterBandwidth(MPU6050_BAND_21_HZ);

    sensors_event_t a, g, temp;
    mpu.getEvent(&a, &g, &temp);
    accBrutoX = a.acceleration.x;
    accBrutoY = a.acceleration.y;
    accBrutoZ = a.acceleration.z;
    gyroBrutoX = g.gyro.x;
    gyroBrutoY = g.gyro.y;
    gyroBrutoZ = g.gyro.z;
    inicializarKalmanComAcelerometro(a, temp);

    Serial.println("[MPU] MPU6050 detectado com sucesso.");
    Serial.printf("[MPU] Config -> Accel: %s | Gyro: %s | Filtro: %s\n",
                  rangeAcelerometroTexto(mpu.getAccelerometerRange()),
                  rangeGiroscopioTexto(mpu.getGyroRange()),
                  larguraFiltroTexto(mpu.getFilterBandwidth()));
    Serial.printf("[MPU] Kalman -> Q_angle=%.4f | Q_bias=%.4f | R_measure=%.4f\n",
                  Q_ANGLE, Q_BIAS, R_MEASURE);
    Serial.printf("[MPU] Angulo inicial -> Eixo X: %.2f deg | Eixo Y: %.2f deg\n",
                  anguloKalmanX, anguloKalmanY);
}

void lerAceleracao(float &x, float &y, float &z) {
    float gyroX, gyroY, gyroZ, temperatura;
    lerIMU(x, y, z, gyroX, gyroY, gyroZ, temperatura);
}

void lerIMU(float &accX, float &accY, float &accZ,
            float &gyroX, float &gyroY, float &gyroZ,
            float &temperatura) {
    if (!imuOk) {
        accX = accY = accZ = 0.0;
        gyroX = gyroY = gyroZ = 0.0;
        temperatura = 0.0;
        return;
    }

    atualizarIMU();

    accX = accBrutoX;
    accY = accBrutoY;
    accZ = accBrutoZ;
    gyroX = gyroCorrigidoX;
    gyroY = gyroCorrigidoY;
    gyroZ = gyroCorrigidoZ;
    temperatura = tempFiltrada;
}

void lerAngulosIMU(float &anguloX, float &anguloY) {
    if (!imuOk) {
        anguloX = 0.0;
        anguloY = 0.0;
        return;
    }

    atualizarIMU();

    anguloX = anguloKalmanX;
    anguloY = anguloKalmanY;
}

void diagnosticarIMU() {
    static unsigned long ultimoLog = 0;

    if (!imuOk) {
        if (millis() - ultimoLog >= 500) {
            ultimoLog = millis();
            Serial.println("[MPU TESTE] MPU indisponivel. Confira conexoes e reinicie o ESP32.");
        }
        return;
    }

    atualizarIMU();

    if (millis() - ultimoLog < 500) {
        return;
    }

    ultimoLog = millis();

    Serial.printf("[MPU] X: %.2f deg | Y: %.2f deg\n", anguloKalmanX, anguloKalmanY);
    Serial2.printf("MPU_X:%.2f|MPU_Y:%.2f\n", anguloKalmanX, anguloKalmanY);
}
