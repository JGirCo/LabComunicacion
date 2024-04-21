#include <SPI.h>
#include <Wire.h>
// Dirección I2C del Si7021
#define SI7021_ADDR 0x40
// Comando para leer el registro de usuario
#define READ_USER_REG 0xE7
// Comando para escribir en el registro de usuario
#define WRITE_USER_REG 0xE6
// Comando de reinicio suave
#define SOFT_RESET 0xFE

#define HTU21D_ADDR 0x40 // Dirección I2C del HTU21D

// Comandos del HTU21D
#define TRIGGER_TEMP_MEASURE_HOLD 0xE3
#define TRIGGER_HUMD_MEASURE_HOLD 0xE5

#define MPU9250_ADDRESS 0x68
#define ACCEL_XOUT_H 0x3B
#define ACCEL_XOUT_L 0x3C
#define ACCEL_YOUT_H 0x3D
#define ACCEL_YOUT_L 0x3E
#define ACCEL_ZOUT_H 0x3F
#define ACCEL_ZOUT_L 0x40

#define GYRO_XOUT_H 0x43
#define GYRO_XOUT_L 0x44
#define GYRO_YOUT_H 0x45
#define GYRO_YOUT_L 0x46
#define GYRO_ZOUT_H 0x47
#define GYRO_ZOUT_L 0x48

#define MAG_ST1 0x02
#define WHO_AM_I_MPU9250 0x75 // Debería devolver 0x71

// Pines para SPI
#define PIN_CS 5 // Pin CS para el MPU9250

float temperatura, humedad;
int16_t temp, humidity;
SPISettings mpuSettings(1000000, MSBFIRST, SPI_MODE0); // Configuración de SPI para MPU9250

void setup() {
  Serial.begin(115200);
  Wire.begin(); 
  SPI.begin(); // Inicializa el bus SPI
  pinMode(PIN_CS, OUTPUT);
  digitalWrite(PIN_CS, HIGH); // Desactiva el dispositivo seleccionado

  setupMPU9250();
}
// ---------------------------------------------- LEER HUMEDAD Y TEMPERATURA DEL SENSOR SI7021 -----------------------------------------------------
int16_t leerHumedad(){
  Wire.beginTransmission(SI7021_ADDR);
  Wire.write(TRIGGER_TEMP_MEASURE_HOLD); // Envía comando de medición de temperatura
  Wire.endTransmission(false);
  Wire.requestFrom(SI7021_ADDR, 2, true); // Solicita 2 bytes de datos
  if( Wire.available() == 2){
    unsigned int rawTemp = (Wire.read()<<8)|(Wire.read());
    //rawTemp &= 0xFFFC; // Los últimos 2 bits son de estado, se deben poner en 0
    temp = rawTemp * (175.72 / 65536.0) - 46.85; // Fórmula del datasheet
  }
  Wire.endTransmission(true);
  return temp;
}
int16_t leerTemperatura(){
  Wire.beginTransmission(SI7021_ADDR);
  Wire.write(TRIGGER_HUMD_MEASURE_HOLD); // Envía comando de medición de humedad
  Wire.endTransmission();
  Wire.requestFrom(SI7021_ADDR, 2); // Solicita 2 bytes de datos
  if( Wire.available() == 2){
    unsigned int rawHumidity = (Wire.read()<<8)|(Wire.read());
    //rawHumidity &= 0xFFFC; // Los últimos 2 bits son de estado, se deben poner en 0
    humidity = rawHumidity * (125.0 / 65536.0) - 6; // Fórmula del datasheet
  }
  Wire.endTransmission(true);
  return humidity;
}
// -------------------------------------------- LEER ACELERÓMETRO Y GIROSCOPIO DEL SENSOR MPU9265 -------------------------------------------------
void setupMPU9250() {
  // Comprobar la comunicación con el MPU9250
  byte who = readRegister(MPU9250_ADDRESS, WHO_AM_I_MPU9250);
  if (who == 0x71) {
    Serial.println("MPU9250 conectado correctamente.");
  } else {
    Serial.print("Error de comunicación. Código recibido: ");
    Serial.println(who, HEX);
    while (1); // Bucle infinito
  }
  
  // Configurar el MPU9250 (añadir tu configuración aquí, esto es solo un ejemplo)
  writeRegister(MPU9250_ADDRESS, 0x6B, 0x00); // Salir de modo sleep
}
void leerSensoresMPU9250() {
  // Leer acelerómetro (esto es solo un ejemplo, necesitas leer y convertir los datos correctamente)
  int16_t accelX = readRegister16(MPU9250_ADDRESS, ACCEL_XOUT_H);
  int16_t accelY = readRegister16(MPU9250_ADDRESS, ACCEL_YOUT_H);
  int16_t accelZ = readRegister16(MPU9250_ADDRESS, ACCEL_ZOUT_H);
  float aX = (accelX/ 16384.0)*9.81;
  float aY = (accelY/ 16384.0)*9.81;
  float aZ = (accelZ/ 16384.0)*9.81;
  int16_t gyroX = readRegister16(MPU9250_ADDRESS, GYRO_XOUT_H);
  int16_t gyroY = readRegister16(MPU9250_ADDRESS, GYRO_YOUT_H);
  int16_t gyroZ = readRegister16(MPU9250_ADDRESS, GYRO_ZOUT_H);
  float GX = gyroX/131.0;
  float GY = gyroY/131.0;
  float GZ = gyroZ/131.0;

  Serial.print("aX: "); Serial.print(aX);
  Serial.print(" | aY: "); Serial.print(aY);
  Serial.print(" | aZ: "); Serial.print(aZ);
  Serial.println(" ");
  Serial.print("gX: "); Serial.print(GX);
  Serial.print(" | gY: "); Serial.print(GY);
  Serial.print(" | gZ: "); Serial.print(GZ);
  Serial.println(" ");
}

byte readRegister(byte device, byte reg) {
  digitalWrite(PIN_CS, LOW);
  SPI.beginTransaction(mpuSettings);
  SPI.transfer(reg | 0x80); // MSB a 1 para leer
  byte value = SPI.transfer(0x00); // Leer el valor
  SPI.endTransaction();
  digitalWrite(PIN_CS, HIGH);
  return value;
}
void writeRegister(byte device, byte reg, byte value) {
  digitalWrite(PIN_CS, LOW);
  SPI.beginTransaction(mpuSettings);
  SPI.transfer(reg & 0x7F); // MSB a 0 para escribir
  SPI.transfer(value);
  SPI.endTransaction();
  digitalWrite(PIN_CS, HIGH);
}

int16_t readRegister16(byte device, byte reg) {
  byte highByte, lowByte;
  highByte = readRegister(device, reg);
  lowByte = readRegister(device, reg + 1);
  return ((int16_t)highByte << 8) | lowByte;
}
// ------------------------------------------------------------ VOID LOOP -------------------------------------------------------------------------
void loop() {
  leerSensoresMPU9250();
  temperatura = leerTemperatura();
  humedad = leerHumedad();
  Serial.print("Temperatura: "); Serial.print(temperatura); Serial.println(" °C.");
  Serial.print("Humedad Relativa: "); Serial.print(humedad); Serial.println(" %.");
  Serial.println(" ");
  delay(500);
}
