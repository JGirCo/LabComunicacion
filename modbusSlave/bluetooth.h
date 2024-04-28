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
  shareValue(pPressureCharacteristic, pressure);
  shareValue(pHumidityCharacteristic, pressure);
  shareValue(pTemperatureCharacteristic, pressure);
  delay(100);
}

void shareValue(BLECharacteristic *characteristic, float value) {
  char strfValue[10];
  dtostrf(value,6,2,strfValue);
  characteristic->setValue(strfValue);
  characteristic->notify();
}
