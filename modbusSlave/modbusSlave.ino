#include <WiFi.h>
#include <HardwareSerial.h>
#include <BLEDevice.h>
#include <BLEUtils.h>
#include <BLEServer.h>

#include "i2cSI7021.h"
#include "spiMPU9250.h"
#include "i2cOLED.h"
#include "spiBMP280.h"
#include "bluetooth.h"

#define I2C_SDA 25
#define I2C_SCL 26

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


bool pulsed = false;
#define LED1 2
#define LED2 4
#define PUL1 21
#define PUL2 22

const char* ssid = "Pixel_5028";
const char* password = "romeo1234";

WiFiServer server(502);

union shiftFloat {
  float f;
  unsigned long l;
};
unsigned int registers[21];

BLECharacteristic *pTemperatureCharacteristic;
BLECharacteristic *pHumidityCharacteristic;
BLECharacteristic *pPressureCharacteristic;

void setup() {
  registers[3] = 62;
  Serial.begin(115200);
  Wire.begin(I2C_SDA,I2C_SCL);
  pinMode(LED1, OUTPUT);
  pinMode(LED2, OUTPUT);
  pinMode(PUL1, INPUT);
  pinMode(PUL2, INPUT);

  delay(10);

  // We start by connecting to a WiFi network

  Serial.println();
  Serial.println();
  initializeOLED();
  initializeSi7021();

  SPI.begin(PIN_SCK, PIN_MISO, PIN_MOSI); // Inicializar SPI
  pinMode(PIN_CS, OUTPUT);
  digitalWrite(PIN_CS, HIGH); // Desactivar el chip select inicialmente
  pinMode(BMP280_CS_PIN, OUTPUT);
  digitalWrite(BMP280_CS_PIN, HIGH); // Desactivar el dispositivo SPI inicialmente
  setupMPU9250();
  setupBMP280();

  clearOLED();
  printOLED("INITIALIZED", 0);
  delay(500);
  clearOLED();

  setupBluetooth();

  modbus();

  Serial.print("Connecting to ");
  Serial.println(ssid);

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("WiFi connected.");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
  server.begin();
}

void loop() {

  modbus();
  WiFiClient client = server.available();  // listen for incoming clients

  if (client) {                     // if you get a client,
    Serial.println("New Client.");  // print a message out the serial port
    while (client.connected()) {  // loop while the client's connected
      String data = getTCP(client);
      Serial.println("-----------------------------------");
      printHEX(data);
      Serial.println("-----------------------------------");
      registers[6] = int(millis()/1000);
      interpretMessage(data, client);
      modbus();
    }
    // close the connection:
    client.stop();
    Serial.println("Client Disconnected.");
  }
}

void modbus() {
  digitalWrite(LED1, registers[0] > 0 ? HIGH : LOW);
  digitalWrite(LED2, registers[1] > 0 ? HIGH : LOW);
  if (registers[2]) {
    // for (int i = 0; i < 9; i++) {
    //   Serial.println(registers[i]);
    // }
    Serial.print("Acc = ");
    Serial.println(registers[9]);
    Serial.print("angularVel = ");
    Serial.println(registers[10]);
    Serial.print("temp = ");
    Serial.println(registers[11]);
    Serial.print("hum = ");
    Serial.println(registers[12]);
  }


  registers[4] = int(millis()/1000);

  if (digitalRead(PUL1) == HIGH) {
    if (!pulsed) {
      Serial.println("pulsed!");
      registers[5]++;
    }
    pulsed = true;
  }
  else {
    pulsed = false;
  }
  registers[7] = (digitalRead(PUL1) == HIGH ? 1 : 0);
  registers[8] = (digitalRead(PUL2) == HIGH ? 1 : 0);

  int16_t accX, accY, accZ, gyroX, gyroY, gyroZ;

  readAccelerometer(accX, accY, accZ);
  float trueAccX = accX * (9.81/16384);
  float trueAccY = accY * (9.81/16384);
  float trueAccZ = accZ * (9.81/16384);
  shiftFloat totalAcc, totalGyro, temp, hum, temp2, pres;

  totalAcc.f = sqrt(pow(trueAccX,2) + pow(trueAccY,2) + pow(trueAccZ,2));
  registers[9] = totalAcc.l >> 16;
  registers[10] = totalAcc.l & 0xFFFF;

  readGyroscope(gyroX, gyroY, gyroZ);
  float trueGyroX = gyroX * (125/16384);
  float trueGyroY = gyroY * (125/16384);
  float trueGyroZ = gyroZ * (125/16384);

  totalGyro.f = sqrt(pow(trueGyroX,2) + pow(trueGyroY,2) + pow(trueGyroZ,2));
  registers[11] = totalGyro.l >> 16;
  registers[12] = totalGyro.l & 0xFFFF;

  temp.f = readTemp();
  registers[13] = temp.l >> 16;
  registers[14] = temp.l & 0xFFFF;

  hum.f = readHum();
  registers[15] = hum.l >> 16;
  registers[16] = hum.l & 0xFFFF;

  readBMP280(temp2.f, pres.f);
  registers[17] = temp2.l >> 16;
  registers[18] = temp2.l & 0xFFFF;
  registers[19] = pres.l >> 16;
  registers[20] = pres.l & 0xFFFF;

  shareValue(pPressureCharacteristic,pres.f);
  shareValue(pTemperatureCharacteristic,temp.f);
  shareValue(pHumidityCharacteristic,hum.f);

  printOLED("ACC: " + String(totalAcc.f,2),0);
  printOLED("TEMP: " + String(temp.f,2),1);
  printOLED("TEMP2: " + String(temp2.f,2),2);
  printOLED("PRES: " + String(pres.f,2),3);
}

String getTCP(WiFiClient client) {
  String tcpBytes = "";
  while (!(tcpBytes.substring(tcpBytes.length() - 2) == "UU") && client.connected()) {
    modbus();
    if (client.available()) {        // if there's bytes to read from the client,
      char tcpByte = client.read();  // read a byte, then Serial.write(c);                    // print it out the serial monitor
      tcpBytes += tcpByte;
    }
  }
  int a2 = tcpBytes.indexOf("a2") + 2;
  int UU = tcpBytes.indexOf("UU") - 2;
  tcpBytes = tcpBytes.substring(a2, UU);
  return tcpBytes;
}

void interpretMessage(String msg, WiFiClient client) {
  if (msg[7] == 0x03) {  //holding registers
    holdingRegisters(msg, client);
  }
  else if (msg[7] == 0x06) {  //holding registers
    singleRegister(msg, client);
  }
}

void holdingRegisters(String msg, WiFiClient client) {
    modbus();
    Serial.println("number of registers" + int(msg[11]));
    for (int i = 0;i < msg[11]+1; i++) {
      Serial.println(registers[i],HEX);
      client.write(registers[i] >> 8);
      client.write(registers[i] & 0xFF);
    }
    client.print("UU");
}

void singleRegister(String msg, WiFiClient client) {
    int registerIndex = msg[9];
    Serial.println("Changed:" + String(registerIndex));
    registers[registerIndex] =  msg[11];
    Serial.println("To:" + String(registers[registerIndex]));
    modbus();
    String out = "a2";
    out += msg;
    client.print(out);
    printHEX(out);
}

void printHEX(String str) {
  for (int i = 0; i < str.length(); i++) {
    if (str[i] <= 0x0F){
            Serial.print("0");
        }
    Serial.print(str[i], HEX);
  }
  Serial.println("");
}

char hex2char(String hexString) {
  for (int i = 0; i < hexString.length(); i += 2) {
    String hexPair = hexString.substring(i, i + 2);
    char character = (char)strtol(hexPair.c_str(), NULL, 16);
    return character;
  }
}

String int2char(int val) {
  char highByte = (val >> 8) & 0xFF;  // Extract the high byte
  char lowByte = val & 0xFF;          // Extract the low byte
  return ("" + highByte + lowByte);
}

void setupBluetooth() {
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
