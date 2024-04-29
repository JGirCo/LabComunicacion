#include <Wire.h>

#define MPU9250_ADDRESS 0x68 // Dirección por defecto del MPU-9250 en el bus I2C
#define PWR_MGMT_1      0x6B // Registro de gestión de energía
#define GYRO_CONFIG     0x1B // Configuración del giroscopio
#define ACCEL_CONFIG    0x1C // Configuración del acelerómetro
#define CONFIG          0x1A // Configuración general
#define MPU9250_ADDRESS     0x68 // Dirección I2C del MPU-9250
#define ACCEL_XOUT_H        0x3B // Dirección del primer registro de datos del acelerómetro
#define GYRO_XOUT_H         0x43 // Dirección del primer registro de datos del giroscopio

// void setup() {
//   Serial.begin(115200); // Inicia la comunicación serial
//   while (!Serial) {
//     ;  // wait for serial port to connect. Needed for native USB port only
//   }
//   Serial.println("Hello world");
//   Wire.begin();
//   setupMPU9250(); // Configura el MPU-9250 para usar el acelerómetro y el giroscopio
//   Serial.println("MPU-9250 inicializado para acelerómetro y giroscopio.");
// }

// void loop() {
//   // Implementa la lógica de lectura de datos aquí
//   int16_t accX, accY, accZ, gyroX, gyroY, gyroZ;
//
//   readAccelerometer(accX, accY, accZ);
//   readGyroscope(gyroX, gyroY, gyroZ);
//
//   Serial.print("Acelerómetro: X=");
//   Serial.print(accX);
//   Serial.print(" Y=");
//   Serial.print(accY);
//   Serial.print(" Z=");
//   Serial.println(accZ);
//
//   Serial.print("Giroscopio: X=");
//   Serial.print(gyroX);
//   Serial.print(" Y=");
//   Serial.print(gyroY);
//   Serial.print(" Z=");
//   Serial.println(gyroZ);
//
//   delay(1000); // Espera un segundo para la próxima lectura
// }

void readAccelerometer(int16_t &accX, int16_t &accY, int16_t &accZ) {
  Wire.beginTransmission(MPU9250_ADDRESS);
  Wire.write(ACCEL_XOUT_H); // Establece el registro desde el cual empezar la lectura
  Wire.endTransmission(false);
  Wire.requestFrom(MPU9250_ADDRESS, 6, true); // Solicita los 6 bytes del acelerómetro
  delay(100);
  accX = Wire.read() << 8 | Wire.read(); // Combina los bytes
  accY = Wire.read() << 8 | Wire.read(); // Combina los bytes
  accZ = Wire.read() << 8 | Wire.read(); // Combina los bytes
}

void readGyroscope(int16_t &gyroX, int16_t &gyroY, int16_t &gyroZ) {
  Wire.beginTransmission(MPU9250_ADDRESS);
  Wire.write(GYRO_XOUT_H); // Establece el registro desde el cual empezar la lectura
  Wire.endTransmission(false);
  Wire.requestFrom(MPU9250_ADDRESS, 6, true); // Solicita los 6 bytes del giroscopio
  delay(100);

  gyroX = Wire.read() << 8 | Wire.read(); // Combina los bytes
  gyroY = Wire.read() << 8 | Wire.read(); // Combina los bytes
  gyroZ = Wire.read() << 8 | Wire.read(); // Combina los bytes
}
void setupMPU9250() {
  Wire.begin(); // Inicializa la comunicación I2C
  Wire.beginTransmission(MPU9250_ADDRESS); // Comienza transmisión al dispositivo
  Wire.write(PWR_MGMT_1); // Selecciona el registro PWR_MGMT_1
  Wire.write(0x00); // Escribe 0x00 para despertar el dispositivo
  Wire.endTransmission(true);

  // Configura el rango del gdelay(100);iroscopio a ±250 grados/seg
  Wire.beginTransmission(MPU9250_ADDRESS);
  Wire.write(GYRO_CONFIG);
  Wire.write(0x00); // Configura el GYRO_CONFIG a 0x00
  Wire.endTransmission(true);

  // Configura el rango del acelerómetro a ±2g
  Wire.beginTransmission(MPU9250_ADDRESS);
  Wire.write(ACCEL_CONFIG);
  Wire.write(0x00); // Configura el ACCEL_CONFIG a 0x00
  Wire.endTransmission(true);
  // Configura el filtro pasa-bajas y la tasa de muestreo
  Wire.beginTransmission(MPU9250_ADDRESS);
  Wire.write(CONFIG);
  Wire.write(0x01); // Configura el CONFIG a 0x01 para usar el filtro pasa-bajas
  Wire.endTransmission(true);
}
