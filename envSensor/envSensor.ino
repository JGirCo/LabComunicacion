#include <HardwareSerial.h>
#include <BLEDevice.h>
#include <BLEUtils.h>
#include <BLEServer.h>

#include "i2cSI7021.h"
#include "spiBMP280.h"
#include "bluetooth.h"
#include "spiMPU9250.h"

#define I2C_SDA 25
#define I2C_SCL 26

#define PIN_SCK   18   // Pin SCK para SPI
#define PIN_MISO  19   // Pin MISO para SPI
#define PIN_MOSI  23   // Pin MOSI para SPI
#define PIN_CS    5    // Pin CS para SPI

#define SI7021_ADDR 0x40
#define READ_USER_REG_SI 0xE7
#define WRITE_USER_REG_SI 0xE6
#define TEMP_REGISTER 0xE3
#define HUM_REGISTER 0xE5

#define MPU9250_ADDRESS 0x68 // Dirección por defecto del MPU-9250 en el bus I2C
#define PWR_MGMT_1      0x6B // Registro de gestión de energía
#define GYRO_CONFIG     0x1B // Configuración del giroscopio
#define ACCEL_CONFIG    0x1C // Configuración del acelerómetro
#define CONFIG          0x1A // Configuración general
#define MPU9250_ADDRESS     0x68 // Dirección I2C del MPU-9250
#define ACCEL_XOUT_H        0x3B // Dirección del primer registro de datos del acelerómetro
#define GYRO_XOUT_H         0x43 // Dirección del primer registro de datos del giroscopio

#define CHARACTERISTIC_UUID "3355e2a9-05db-4b21-843b-1a722701998c"

BLECharacteristic *pTemperatureCharacteristic;
BLECharacteristic *pHumidityCharacteristic;
BLECharacteristic *pPressureCharacteristic;
BLECharacteristic *pAccelerationCharacteristic;

void setup() {
  Wire.begin(I2C_SDA,I2C_SCL);
  initializeSi7021();

  SPI.begin(PIN_SCK, PIN_MISO, PIN_MOSI); // Inicializar SPI
  pinMode(PIN_CS, OUTPUT);
  digitalWrite(PIN_CS, HIGH); // Desactivar el chip select inicialmente
  pinMode(BMP280_CS_PIN, OUTPUT);
  digitalWrite(BMP280_CS_PIN, HIGH); // Desactivar el dispositivo SPI inicialmente
  setupMPU9250();
  setupBMP280();

  BLEDevice::init("JuanMa Termometro");
  BLEServer *pServer = BLEDevice::createServer();
  BLEService *pEnvSensService = pServer->createService("181A");

  pTemperatureCharacteristic = pEnvSensService->createCharacteristic(
    "2a1c",
    BLECharacteristic::PROPERTY_READ |
    BLECharacteristic::PROPERTY_NOTIFY
  );

  pHumidityCharacteristic = pEnvSensService->createCharacteristic(
    "2a6f",
    BLECharacteristic::PROPERTY_READ |
    BLECharacteristic::PROPERTY_NOTIFY
  );

  pPressureCharacteristic = pEnvSensService->createCharacteristic(
    "2a6d",
    BLECharacteristic::PROPERTY_READ |
    BLECharacteristic::PROPERTY_NOTIFY
  );

  pAccelerationCharacteristic = pEnvSensService->createCharacteristic(
    CHARACTERISTIC_UUID,
    BLECharacteristic::PROPERTY_READ |
    BLECharacteristic::PROPERTY_NOTIFY
  );

  pEnvSensService->start();
  pServer->getAdvertising()->start();
}

void loop() {
  float temp, hum, temp2, pres;
  int16_t accX, accY, accZ, gyroX, gyroY, gyroZ;

  readAccelerometer(accX, accY, accZ);
  float trueAccX = accX * (9.81/16384);
  float trueAccY = accY * (9.81/16384);
  float trueAccZ = accZ * (9.81/16384);

  temp = readTemp();
  hum = readHum();
  readBMP280(temp2, pres);
  shareValue(pPressureCharacteristic,pres);
  shareValue(pTemperatureCharacteristic,temp);
  shareValue(pHumidityCharacteristic,hum);
  shareValue(pAccelerationCharacteristic,trueAccZ);
}
