#ifndef RFID_RC522_H
#define RFID_RC522_H

#include <Arduino.h>
#include <SPI.h>
#include <MFRC522.h>

void initRFID();
String lerTagRFID(); // Retorna o ID da tag como String

#endif