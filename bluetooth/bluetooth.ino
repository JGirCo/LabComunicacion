#include <BLEDevice.h>
#include <BLEUtils.h>
#include <BLEServer.h>

float temperature = 100; // Nivel de temperatura inicial
float humidity = 60; // Nivel de humedad inicial
float pressure = 16; // Nivel de humedad inicial
char strTemperature[10];
char strHumidity[10];
char strPressure[10];

BLECharacteristic *pTemperatureCharacteristic;
BLECharacteristic *pHumidityCharacteristic;
BLECharacteristic *pPressureCharacteristic;
void setup() {
  BLEDevice::init("JuanMa Termometro");
  BLEServer *pServer = BLEDevice::createServer();

  // Crear un servicio de batería
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

  pEnvSensService->start();
  pServer->getAdvertising()->start();
}

void loop() {
  pressure += 1;
  dtostrf(pressure,6,2,strPressure);
  pPressureCharacteristic->setValue(strPressure);
  pPressureCharacteristic->notify();
  delay(1000);
}
