/*
 *
 * IoT WS 2015/16 Project - Mohd Ahmed Khan, Saion Chatterjee
 *
 * Board Used - Intel Galileo Gen 2, Sensor- RFID RC522
 * 
 * Typical pin layout used:
 * -----------------------------------------------------------------------------------------
 *             MFRC522      Arduino       Arduino   Arduino    Arduino          Arduino
 *             Reader/PCD   Uno           Mega      Nano v3    Leonardo/Micro   Pro Micro
 * Signal      Pin          Pin           Pin       Pin        Pin              Pin
 * -----------------------------------------------------------------------------------------
 * RST/Reset   RST          9             5         D9         RESET/ICSP-5     RST
 * SPI SS      SDA(SS)      10            53        D10        10               10
 * SPI MOSI    MOSI         11 / ICSP-4   51        D11        ICSP-4           16
 * SPI MISO    MISO         12 / ICSP-1   50        D12        ICSP-1           14
 * SPI SCK     SCK          13 / ICSP-3   52        D13        ICSP-3           15
 */

#include <SPI.h>
#include <MFRC522.h>
#include <Ethernet.h>
#include <iostream>
#include <string>
#include <sstream>

#define RST_PIN         9          // Configurable, see typical pin layout above
#define SS_PIN          10         // Configurable, see typical pin layout above

MFRC522 mfrc522(SS_PIN, RST_PIN);  // Create MFRC522 instance

// Enter a MAC address for your controller below.
// Newer Ethernet shields have a MAC address printed on a sticker on the shield
byte mac[] = {  0x98, 0x4F, 0xEE, 0x01, 0xCD, 0x27 };

//IPAddress server(192,168,1,103); // Our server
IPAddress server(127,0,0,1); // Chip server

// Initialize the Ethernet client library
// with the IP address and port of the server 
// that you want to connect to (port 80 is default for HTTP):
EthernetClient client;

void setup() {
  Serial.begin(9600);   // Initialize serial communications with the PC
  while (!Serial);    // Do nothing if no serial port is opened (added for Arduinos based on ATMEGA32U4)
  SPI.begin();      // Init SPI bus
  mfrc522.PCD_Init();   // Init MFRC522
//---------------ETHERNET INIT
  // start the Ethernet connection:
  if (Ethernet.begin(mac) == 0) {
    Serial.println("Failed to configure Ethernet using DHCP");
    // no point in carrying on, so do nothing forevermore:
    for(;;)
      ;
  }
  // give the Ethernet shield a second to initialize:
  delay(1000);
  Serial.println("connecting...");

// if you get a connection, report back via serial:
  if (client.connect(server, 3000)) {
    Serial.println("connected");
  } 
  else {
    // if you didn't get a connection to the server:
    Serial.println("connection failed");
  }

}

void loop() {
  // Look for new cards
  if ( ! mfrc522.PICC_IsNewCardPresent()) {
    return;
  }

  // Select one of the cards
  if ( ! mfrc522.PICC_ReadCardSerial()) {
    return;
  }

  char store_id[30];
  
  for (byte i = 0; i < mfrc522.uid.size; i++) {
    Serial.print(mfrc522.uid.uidByte[i]);
  } 
Serial.println();

				unsigned int hex_num;
          hex_num =  mfrc522.uid.uidByte[0] << 24;
          hex_num += mfrc522.uid.uidByte[1] << 16;
          hex_num += mfrc522.uid.uidByte[2] <<  8;
          hex_num += mfrc522.uid.uidByte[3];

         int  NFC_id=  (int)hex_num;

sprintf(store_id,"%d" ,NFC_id);

  Serial.print("Store id is: ");
  Serial.println(store_id);


//------------ETHERNET PART----------------

 if(!client.connected()) {
     Serial.println("Inside if !connected");
        if (client.connect(server, 3000)) {
          Serial.println("connected again");
        } 
        else {
          // if you didn't get a connection to the server:
          Serial.println("connection failed on retry");
        }
    }

  char req[80];
  strcpy(req, "GET /?uid=");
  strcat(req, store_id);
  strcat(req, "&lat=48.139867&long=11.560935&sensor='MUCHbf' HTTP/1.0");
    
          // Make a HTTP request:
          client.println(req);
          client.println();
          Serial.println("Request sent");
         Serial.println(req);

  
  // if there are incoming bytes available 
  // from the server, read them and print them:
  while (client.available()) {
    char c = client.read();
    Serial.print(c);
  }

}
