#include "oledLetters.h"
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
  Wire.write(0x27);
  Wire.endTransmission(true);

  Wire.beginTransmission(LCDADDRESS);
  Wire.write(0x00);
  Wire.write(0x22);
  Wire.write(0x00);
  Wire.write(0x00);
  Wire.endTransmission(true);

}

void loop() {
  // char characters[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789.:ñññ";
  // int numCharacters = sizeof(characters) - 1;
  // for (int i = 0; i < numCharacters; i++) {
  //   displayChar(characters[i]);
  //   delay(500);
  // }
  printOLED("TEMP:25.6", 0);
  printOLED("TEMP2:25.6", 1);
  printOLED("HUM:3.5", 2);
  printOLED("ACC:9.81", 3);
  delay(500);
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

void displayChar(char c){
  const byte* bitmap = getCharBitmap(c);
  Wire.beginTransmission(LCDADDRESS);
  Wire.write(0x40);

  for (int i = 0; i < 8; i++) {
    Wire.write(bitmap[i]);
  }
  Wire.endTransmission(true);
}

void printOLED(String string, int page){
  int strLength = string.length();

  Wire.beginTransmission(LCDADDRESS);
  Wire.write(0x00);
  Wire.write(0x22);
  Wire.write(page);
  Wire.write(page);
  Wire.endTransmission(true);
  for(int i = 0; i < strLength; i++){
    Wire.beginTransmission(LCDADDRESS);
    Wire.write(0x00);
    Wire.write(0x21);
    Wire.write(i*9);
    Wire.write(i*9+8);
    Wire.endTransmission(true);

    displayChar(string[i]);

    Wire.beginTransmission(LCDADDRESS);
    Wire.write(0x40);
    Wire.write(0x00);
    Wire.endTransmission(true);
  }
}
