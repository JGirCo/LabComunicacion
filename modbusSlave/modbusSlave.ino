/*
 created for arduino 25 Nov 2012
 by Tom Igoe

ported for sparkfun esp32
31.01.2017 by Jan Hendrik Berlinn
/*  */


#include <WiFi.h>
#include <HardwareSerial.h>

unsigned int registers[9];
bool pulsed = false;
int LED1 = 18;
int LED2 = 19;
int PUL1 = 22;
int PUL2 = 23;

const char* ssid = "Pixel_5028";
const char* password = "romeo1234";

WiFiServer server(502);


void setup() {
  registers[3] = 62;
  Serial.begin(115200);
  pinMode(LED1, OUTPUT);
  pinMode(LED2, OUTPUT);
  pinMode(PUL1, INPUT);
  pinMode(PUL2, INPUT);

  delay(10);

  // We start by connecting to a WiFi network

  Serial.println();
  Serial.println();
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
    for (int i = 0; i < 9; i++) {
      Serial.println(registers[i]);
    }
  }

  registers[4] = int(millis()/1000);

  if (digitalRead(PUL1) == HIGH) {
    if (!pulsed) {
      Serial.println("pulsed!");
      registers[5]++;
    }
    pulsed = true;
  } else {
    pulsed = false;
  }
  registers[7] = (digitalRead(PUL1) == HIGH ? 1 : 0);
  registers[8] = (digitalRead(PUL2) == HIGH ? 1 : 0);
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
    Serial.println("Received! :D");
    holdingRegisters(msg, client);
  }
  else if (msg[7] == 0x06) {  //holding registers
    Serial.println("Received! >:D");
    singleRegister(msg, client);
  }
}

void holdingRegisters(String msg, WiFiClient client) {
    modbus();
    client.print("a2");
    Serial.println(msg[11]);
    for (int i = 0;i < msg[11]; i++) {
      Serial.println(registers[i]);
      client.write(registers[i]);
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
