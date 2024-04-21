#include <SPI.h>

#define PIN_SCK   18   // Pin SCK para SPI
#define PIN_MISO  19   // Pin MISO para SPI
#define PIN_MOSI  23   // Pin MOSI para SPI
#define PIN_CS    5    // Pin CS para SPI

#define MPU9250_SPI_READ  0x80
#define PWR_MGMT_1        0x6B
#define GYRO_CONFIG       0x1B
#define ACCEL_CONFIG      0x1C
#define CONFIG            0x1A

// void setup() {
//   Serial.begin(115200);
//   SPI.begin(PIN_SCK, PIN_MISO, PIN_MOSI, PIN_CS); // Inicializar SPI
//   pinMode(PIN_CS, OUTPUT);
//   digitalWrite(PIN_CS, HIGH); // Desactivar el chip select inicialmente
//
//   // Configurar el MPU9250
//   setupMPU9250();
// }
//
// void loop() {
//   // Leer datos del acelerómetro
//   float accelData[3];
//   readAccelData(accelData);
//   Serial.print("Acelerómetro (g): X=");
//   Serial.print(accelData[0]);
//   Serial.print("\tY=");
//   Serial.print(accelData[1]);
//   Serial.print("\tZ=");
//   Serial.println(accelData[2]);
//
//   // Leer datos del giroscopio
//   float gyroData[3];
//   readGyroData(gyroData);
//   Serial.print("Giroscopio (grados/seg): X=");
//   Serial.print(gyroData[0]);
//   Serial.print("\tY=");
//   Serial.print(gyroData[1]);
//   Serial.print("\tZ=");
//   Serial.println(gyroData[2]);
// }
//
void readAccelData(float* accelData) {
  // Activar chip select
  digitalWrite(PIN_CS, LOW);

  // Leer datos del acelerómetro
  SPI.transfer(MPU9250_SPI_READ | 0x3B); // Registro de inicio del acelerómetro
  for (int i = 0; i < 3; i++) {
    accelData[i] = SPI.transfer(0) << 8 | SPI.transfer(0);
  }

  // Desactivar chip select
  digitalWrite(PIN_CS, HIGH);
}

void readGyroData(float* gyroData) {
  // Activar chip select
  digitalWrite(PIN_CS, LOW);

  // Leer datos del giroscopio
  SPI.transfer(MPU9250_SPI_READ | 0x43); // Registro de inicio del giroscopio
  for (int i = 0; i < 3; i++) {
    gyroData[i] = SPI.transfer(0) << 8 | SPI.transfer(0);
  }

  // Desactivar chip select
  digitalWrite(PIN_CS, HIGH);
}

void setupMPU9250() {
  // Despertar el dispositivo y configurar el giroscopio y acelerómetro
  digitalWrite(PIN_CS, LOW);
  SPI.transfer(PWR_MGMT_1);
  SPI.transfer(0x00); // Configurar para encender el dispositivo
  digitalWrite(PIN_CS, HIGH);

  delay(100);

  digitalWrite(PIN_CS, LOW);
  SPI.transfer(GYRO_CONFIG);
  SPI.transfer(0x00); // Configurar el rango del giroscopio a ±250 grados/seg
  digitalWrite(PIN_CS, HIGH);

  delay(100);

  digitalWrite(PIN_CS, LOW);
  SPI.transfer(ACCEL_CONFIG);
  SPI.transfer(0x00); // Configurar el rango del acelerómetro a ±2g
  digitalWrite(PIN_CS, HIGH);

  delay(100);

  digitalWrite(PIN_CS, LOW);
  SPI.transfer(CONFIG);
  SPI.transfer(0x01); // Configurar el filtro pasa-bajas y la tasa de muestreo
  digitalWrite(PIN_CS, HIGH);

  delay(100);

  Serial.println("MPU9250 configurado correctamente.");
}
