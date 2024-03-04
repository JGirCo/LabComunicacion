/*
 created for arduino 25 Nov 2012
 by Tom Igoe

ported for sparkfun esp32
31.01.2017 by Jan Hendrik Berlin

 */

#include <WiFi.h>

int registers[9];
bool pulsed = false;
int LED1 = 5;
int LED2 = 6;
int PUL1 = 2;
int PUL2 = 3;

const char* ssid     = "Giraldo_Correa";
const char* password = "Romeo2011";

WiFiServer server(502);

void setup()
{
    Serial.begin(115200);
    pinMode(5, OUTPUT);      // set the LED pin mode

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

void loop(){

    WiFiClient client = server.available();   // listen for incoming clients

    if (client) {                             // if you get a client,
        Serial.println("New Client.");           // print a message out the serial port
        client.println("hello");
        while (client.connected()) {            // loop while the client's connected
            String data = getTCP(client);
            Serial.println(data);
        }
        // close the connection:
        client.stop();
        Serial.println("Client Disconnected.");
    }
}

void modbus() {
    analogWrite(LED1, registers[0]);
    analogWrite(LED2, registers[1]);

    registers[4] = int(millis);

    if (digitalRead(PUL1)) {
        if (not pulsed) {
            registers[5] = registers[5]++;
        }
        pulsed = true;
    }
    else {
        pulsed = false;
    }
    registers[6] = digitalRead(PUL2);
    registers[7] = digitalRead(PUL1);
}

String getTCP(WiFiClient client) {
    String tcpBytes = "";
    while (!(tcpBytes.substring(tcpBytes.length() - 2) == "UU") && client.connected()) {
        if (client.available()) {             // if there's bytes to read from the client,
            char tcpByte = client.read();             // read a byte, then Serial.write(c);                    // print it out the serial monitor
            tcpBytes += tcpByte;
        }
    }
    int a2 = tcpBytes.indexOf("a2") + 2;
    int UU = tcpBytes.indexOf("UU");
    tcpBytes = tcpBytes.substring(a2, UU);
    return tcpBytes;
}
