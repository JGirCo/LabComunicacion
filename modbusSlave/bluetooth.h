#include <BLEDevice.h>
#include <BLEUtils.h>
#include <BLEServer.h>


void shareValue(BLECharacteristic *characteristic, float value) {
  char strfValue[10];
  dtostrf(value,6,2,strfValue);
  characteristic->setValue(strfValue);
  characteristic->notify();
}
