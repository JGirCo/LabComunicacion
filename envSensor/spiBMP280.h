#include <SPI.h>

#define BMP280_CS_PIN 14 // Pin CS del BMP280 conectado al pin 5 del Arduino
#define BMP280_MISO_PIN 19 // Pin MISO del BMP280 conectado al pin 19 del Arduino
#define BMP280_MOSI_PIN 23 // Pin MOSI del BMP280 conectado al pin 23 del Arduino
#define BMP280_SCK_PIN 18 // Pin SCK del BMP280 conectado al pin 18 del Arduino

// Definición de registros del BMP280
#define BMP280_REG_DIG_T1 0x88
#define BMP280_REG_CTRL_MEAS 0xF4
#define BMP280_REG_CONFIG 0xF5
#define BMP280_REG_PRESS_MSB 0xF7

#define BMP280_VAL_CONFIG 0x1C
#define BMP280_VAL_CTRL_MEAS 0x2F
#define PRESION_NIVEL_MAR 101.325 // Presión promedio al nivel del mar en kPa (101.325 kPa)

// Variables para almacenar los valores leídos del BMP280
uint8_t vals1[6];
uint8_t compensa1[24];
int32_t temp_bits1;
int32_t press_bits1;
int32_t t_fine;
int32_t tempcomp1;
uint32_t presscomp1;
float press_final1 = 0.0;
float temp_final1 = 0.0;

SPISettings settings(1000000, MSBFIRST, SPI_MODE0); // Configuración SPI (1 MHz, MSB primero, modo 0)

void writeSPI(uint8_t reg, uint8_t val) {
  SPI.beginTransaction(settings);
  digitalWrite(BMP280_CS_PIN, LOW);
  SPI.transfer(reg & 0x7F); // Bit 7 en 0 para escribir en el registro
  SPI.transfer(val);
  digitalWrite(BMP280_CS_PIN, HIGH);
  SPI.endTransaction();
}

void readSPI(uint8_t reg, uint8_t* data, uint8_t len) {
  SPI.beginTransaction(settings);
  digitalWrite(BMP280_CS_PIN, LOW);
  SPI.transfer(reg | 0x80); // Bit 7 en 1 para lectura del registro
  for (uint8_t i = 0; i < len; i++) {
    data[i] = SPI.transfer(0x00); // Enviar datos nulos para recibir datos del sensor
  }
  digitalWrite(BMP280_CS_PIN, HIGH);
  SPI.endTransaction();
}

int32_t compensateTemp(int32_t adc_T, uint8_t comp[24]) {
  // Implementa la compensación de temperatura
    //y comp, que es un array de bytes uint8_t de longitud 24. La función devuelve un valor de tipo int32_t.
  int32_t var1, var2, T;// Se declaran tres variables: var1, var2 y T, todas de tipo int32_t para almacenar valores enteros con signo
  //de 32 bits

  //Se declaran tres variables dig_T1, dig_T2 y dig_T3, de tipo uint16_t para almacenar valores enteros sin signo de 16 bits.
  //Luego, se les asignan valores calculados a partir de elementos del array comp que representan coeficientes de calibración.
  //Los valores se obtienen combinando elementos del array comp utilizando operaciones de bits y desplazamientos
  uint16_t dig_T1, dig_T2, dig_T3;
  dig_T1=comp[0]+(comp[1]<<8);
  dig_T2=comp[2]+(comp[3]<<8);
  dig_T3=comp[4]+(comp[5]<<8);
  // se calcula la compensacion de la temperatura
  var1=((((adc_T>>3)-((int32_t)dig_T1<<1)))*((int32_t)dig_T2))>>11;
  var2=(((((adc_T>>4)-((int32_t)dig_T1))*((adc_T>>4)-((int32_t)dig_T1)))>>12)*((int32_t)dig_T3))>>14;
  t_fine=var1+var2;
  T=(t_fine*5+128)>>8;// tempertura final
  return T;
}

uint32_t compensatePress(int32_t adc_P, uint8_t comp[24]) {
  // Implementa la compensación de presión
 ///que es un array de bytes uint8_t de longitud 24. La función devuelve un valor de tipo uint32_t, que es un entero sin signo de 32 bits
  int32_t var1, var2; //enteros de 32 bits con signo
  uint32_t p;//entero sin signo de 32 bits
  uint16_t dig_P1, dig_P2, dig_P3, dig_P4, dig_P5, dig_P6, dig_P7, dig_P8, dig_P9;
  //se les asignan valores calculados a partir de elementos del array comp que representan coeficientes de calibración.
  //Los valores se obtienen combinando elementos del array computilizando operaciones de bits y desplazamientos.
  dig_P1=comp[6]+(comp[7]<<8);
  dig_P2=comp[8]+(comp[9]<<8);
  dig_P3=comp[10]+(comp[11]<<8);
  dig_P4=comp[12]+(comp[13]<<8);
  dig_P5=comp[14]+(comp[15]<<8);
  dig_P6=comp[16]+(comp[17]<<8);
  dig_P7=comp[18]+(comp[19]<<8);
  dig_P8=comp[20]+(comp[21]<<8);
  dig_P9=comp[22]+(comp[23]<<8);
  //se hace la compensacion de presion y se utilizan coeficientes de calibracion
  var1=(((int32_t)t_fine)>>1)-(int32_t)64000;
  var2=(((var1>>2)*(var1>>2))>>11)*((int32_t)dig_P6);
  var2=var2+((var1*((int32_t)dig_P5))<<1);
  var2=(var2>>2)+(((int32_t)dig_P4)<<16);
  var1=(((dig_P3*(((var1>>2)*(var1>>2))>>3))>>3)+((((int32_t)dig_P2)*var1)>>1))>>18;
  var1=((((32768+var1))*((int32_t)dig_P1))>>15);
  if(var1==0){
    return 0;//prevenir division por cero
  }
  //se determina la presion compensada , y se hace una comprobacion para evitar desbordamientos y realizar divisiones adecuadas
  p=(((uint32_t)(((int32_t)1048576)-adc_P)-(var2>>12)))*3125;
  if(p<0x80000000){
    p=(p<<1)/((uint32_t)var1);
  }
  else{
    p=(p/(uint32_t)var1)*2;
  }
  // en estas lineas se ajustael valor de la presion compensada p , el resultado se almacena en p y se devuelve este valor
  var1=(((int32_t)dig_P9)*((int32_t)(((p>>3)*(p>>3))>>13)))>>12;
  var2=(((int32_t)(p>>2))*((int32_t)dig_P8))>>13;
  p=(uint32_t)((int32_t)p+((var1+var2+dig_P7)>>4));
  return p;
}

void setupBMP280() {
  // Configurar el BMP280
  writeSPI(BMP280_REG_CTRL_MEAS, BMP280_VAL_CTRL_MEAS);
  writeSPI(BMP280_REG_CONFIG, BMP280_VAL_CONFIG);

  delay(200); // Esperar a que el dispositivo se configure antes de comenzar a leer
}

void readBMP280(float &temp_final1, float &press_final1) {
  readSPI(BMP280_REG_PRESS_MSB, vals1, 6);
  readSPI(BMP280_REG_DIG_T1, compensa1, 24);

  press_bits1 = (vals1[0] << 12) + (vals1[1] << 4) + (vals1[2] >> 4);
  temp_bits1 = (vals1[3] << 12) + (vals1[4] << 4) + (vals1[5] >> 4);

  tempcomp1 = compensateTemp(temp_bits1, compensa1);
  temp_final1 = (float)tempcomp1 / 100.0;

  presscomp1 = compensatePress(press_bits1, compensa1);
  press_final1 = (float)presscomp1 / 1000.0;
}
