#include <Wire.h>

#define I2C_SDA 25
#define I2C_SCL 26
//de calibración de temper14
#define SI7021_ADDR 0x40
#define READ_USER_REG 0xE7
#define WRITE_USER_REG 0xE6

void initializeSi7021() {
  // Leer el registro de usuario actual
  Wire.beginTransmission(SI7021_ADDR);
  Wire.write(READ_USER_REG);
  Wire.endTransmission();
  delay(25);
  Wire.requestFrom(SI7021_ADDR, 1);
  Serial.println("Starting Configuration...");
  delay(25);
  if (Wire.available()) {
    Serial.println("Configuration began");
    byte userReg = Wire.read(); // Leer el valor actual del registro de usuario

    // Configurar el calentador apagado y la resolución a 12 bits humedad, 14 bits temperatura
    // Calentador apagado: asegurarse de que el bit 2 está en 0
    // Resolución 0b00: asegurarse de que los bits 7 y 0 están en 0
    userReg &= ~(1 << 2); // Apagar el calentador
    userReg &= 0x7E; // Limpia los bits de resolución manteniendo el resto igual
    userReg |= 0b00; // Configura los nuevos bits de resolución para 12 bits humedad, 14 bits temperatura

    // Escribir el nuevo valor del registro de usuario
    Wire.beginTransmission(SI7021_ADDR);
    Wire.write(WRITE_USER_REG);
    Wire.write(userReg);
    Wire.endTransmission();
    Wire.beginTransmission(0x40);
    Wire.write(0xFE); //0xE3
    Wire.endTransmission();
    Serial.println("initilization sucessful");
  }

}

float readTemp() {
  Wire.beginTransmission(0x40);
  Wire.write(0xE3);
  Wire.endTransmission();
  delay(100);
  Wire.requestFrom(0x40, (uint8_t) 2);
  delay(100);

  float temperature = 0.0;
  if(Wire.available() == 2) {
    int   Temp      = (Wire.read() << 8 );
    Temp      |= Wire.read();
    temperature = (Temp*175.72/65536)-46.85;
  }
  return temperature;
}

float readHum() {
  Wire.beginTransmission(0x40);
  Wire.write(0xE5 );
  Wire.endTransmission();
  delay(25);
  Wire.requestFrom(0x40, (uint8_t) 2);
  delay(25);
  float humidity = 0.0;
  if(Wire.available() == 2) {
       int   Hum      = (Wire.read() << 8 );
             Hum      |= Wire.read();
             humidity = ((Hum * 125.0) / 65536) - 6;
  }
  return humidity;
}
