#include <Wire.h>
#define LCDADDRESS 0x3C
#define I2C_SDA 25
#define I2C_SCL 26
int num = 1;
void setup() {
  Wire.begin(I2C_SDA, I2C_SCL);
  Serial.begin(115200);

  initializeLCD();
  for (uint16_t i=0; i<=128*8;i++){
    Wire.beginTransmission(LCDADDRESS);
    Wire.write(0x40);
    Wire.write(0x00);
    Wire.endTransmission();
  }
  // sendCommand(0xAE);
  // sendCommand(0xAf);
  // sendCommand(0xA7);
  Wire.beginTransmission(LCDADDRESS);
  Wire.write(0x00);
  Wire.write(0x21);
  Wire.write(0x20);
  Wire.write(0x2F);
  Wire.endTransmission(true);

  Wire.beginTransmission(LCDADDRESS);
  Wire.write(0x00);
  Wire.write(0x22);
  Wire.write(0x00);
  Wire.write(0x00);
  Wire.endTransmission(true);

}

void loop() {
  Wire.beginTransmission(LCDADDRESS);
  Wire.write(0x40);
  // Wire.write(B00000000);
  // Wire.write(B00100100);
  // Wire.write(B00100100);
  // Wire.write(B10000001);
  // Wire.write(B10000001);
  // Wire.write(B01000010);
  // Wire.write(B00111100);
  // Wire.write(B00000000);
  // Wire.write(B00000000);
  // Wire.write(B00000000);
  // Wire.write(B00000000);
  // Wire.write(B00000000);
  // Wire.write(B00000000);
  // Wire.write(B00000000);
  // Wire.write(B00000000);
  // Wire.write(B00000000);

Wire.write(B11000011);
Wire.write(B10111101);
Wire.write(B01000010);
Wire.write(B10100101);
Wire.write(B10000001);
Wire.write(B10011001);
Wire.write(B11011011);
Wire.write(B01100110);
Wire.write(B11011011);
Wire.write(B10000001);
Wire.write(B10011001);
Wire.write(B10011001);
Wire.write(B10011001);
Wire.write(B10111101);
Wire.write(B10011001);
Wire.write(B10000001);
  Wire.endTransmission(true);

  delay(50);
}

void sendCommand(uint8_t command) {
  Wire.beginTransmission(LCDADDRESS);
  Wire.write(0x00);
  Wire.write(command);
  Wire.endTransmission(true);
}

void initializeLCD() {
  Wire.beginTransmission(LCDADDRESS);
  Wire.write(0x00); // Control byte for commands

  // Turn off the LCD display
  Wire.write(0xAE);

  // Set display clock divide ratio/oscillator frequency
  Wire.write(0xD5);
  Wire.write(0x80);

  // Set multiplex ratio
  Wire.write(0xA8);
  Wire.write(0x1F); // 128x32

  // Set display offset
  Wire.write(0xD3);
  Wire.write(0x00); // No offset

  // Set start line address
  Wire.write(0x40 | 0x00);

  // Charge Pump setting
  Wire.write(0x8D);
  Wire.write(0x14); // Enable charge pump

  // Memory addressing mode
  Wire.write(0x20);
  Wire.write(0x00); // Horizontal addressing mode

  // Set address and page
  Wire.write(0x21);
  Wire.write(0);
  Wire.write(127);

  Wire.write(0x22);
  Wire.write(0);
  Wire.write(7);

  // Set segment re-map
  Wire.write(0xA0 | 0x1);

  // Set COM output scan direction
  Wire.write(0xC8);

  // Set COM pins hardware configuration
  Wire.write(0xDA);
  Wire.write(0x02);

  // Set contrast control
  Wire.write(0x81);
  Wire.write(0x8F);

  // Set pre-charge period
  Wire.write(0xD9);
  Wire.write(0xF1);

  // Set VCOMH deselect level
  Wire.write(0xDB);
  Wire.write(0x40);

  // Entire display ON
  Wire.write(0xA4);


  // Normal display
  Wire.write(0xA6);
  //clean up display

  // Turn on the LCD display
  Wire.write(0x00);
  Wire.write(0xAF);
  Wire.endTransmission();

}
