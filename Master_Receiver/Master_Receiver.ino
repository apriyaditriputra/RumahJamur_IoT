
//---------------------------------------- Include Library.
#include <WiFi.h>
#include <HTTPClient.h>
#include <SPI.h>
#include <LoRa.h>
//----------------------------------------

//---------------------------------------- LoRa Pin / GPIO configuration.
#define ss 5
#define rst 14
#define dio0 2
//----------------------------------------

//---------------------------------------- Variable declaration to hold incoming and outgoing data.
String Incoming = "";
String Message = "";            
//----------------------------------------

//---------------------------------------- LoRa data transmission configuration.
byte LocalAddress = 0x01;               //--> address of this device (Master Address).
byte Destination_ESP32_Slave_1 = 0x02;  //--> destination to send to Slave 1 (ESP32).
byte Destination_ESP32_Slave_2 = 0x03;  //--> destination to send to Slave 2 (ESP32).
//---------------------------------------- 

//---------------------------------------- Variable declaration for Millis/Timer.
unsigned long previousMillis_SendMSG = 0;
const long interval_SendMSG = 1000;
//---------------------------------------- 

// Variable declaration to count slaves.
byte Slv = 0;

//________________________________________________________________________________ Subroutines for sending data (LoRa Ra-02).
const char ssid[] = "Yadii";
const char password[] = "yadi1029";
String LoRaData;
String HOST_NAME = "http://monitoringiotjamur.my.id/koneksi.php?temperature="; // REPLACE WITH YOUR PC's IP ADDRESS
String tempQuery = "?temperature=31.0";

void sendMessage(String Outgoing, byte Destination) {
  LoRa.beginPacket();             //--> start packet
  LoRa.write(Destination);        //--> add destination address
  LoRa.write(LocalAddress);       //--> add sender address
  LoRa.write(Outgoing.length());  //--> add payload length
  LoRa.print(Outgoing);           //--> add payload
  LoRa.endPacket();               //--> finish packet and send it
}
//________________________________________________________________________________ 

//________________________________________________________________________________ Subroutines for receiving data (LoRa Ra-02).
void onReceive(int packetSize) {
  if (packetSize == 0) return;  //--> if there's no packet, return

  //---------------------------------------- read packet header bytes:
  int recipient = LoRa.read();        //--> recipient address
  byte sender = LoRa.read();          //--> sender address
  byte incomingLength = LoRa.read();  //--> incoming msg length
  //---------------------------------------- 

  // Clears Incoming variable data.
  Incoming = "";

  //---------------------------------------- Get all incoming data.
  while (LoRa.available()) {
    Incoming += (char)LoRa.read();
  }
  //---------------------------------------- 

  //---------------------------------------- Check length for error.
  if (incomingLength != Incoming.length()) {
    Serial.println();
    Serial.println("error: message length does not match length");
    return; //--> skip rest of function
  }
  //---------------------------------------- 

  //---------------------------------------- Checks whether the incoming data or message for this device.
  if (recipient != LocalAddress) {
    Serial.println();
    Serial.println("This message is not for me.");
    return; //--> skip rest of function
  }
  //---------------------------------------- 

  //---------------------------------------- if message is for this device, or broadcast, print details:
  Serial.println();
//  Serial.println("Received from: 0x" + String(sender, HEX));
  //Serial.println("Message length: " + String(incomingLength));
  Serial.println(Incoming);
  //Serial.println("RSSI: " + String(LoRa.packetRssi()));
  //Serial.println("Snr: " + String(LoRa.packetSnr()));
  //---------------------------------------- 

   HTTPClient http;

      String server = HOST_NAME + Incoming ;
      http.begin(server);
      http.addHeader("Content-Type", "application/x-www-form-urlencoded");
      int httpCode = http.GET();
      if (httpCode > 0) {
        if (httpCode == HTTP_CODE_OK) {
          String payload = http.getString();
          Serial.println(payload);
        } else {
          Serial.printf("HTTP GET... code: %d\n", httpCode);

        }
      } else {
        Serial.printf("HTTP GET... failed, error: %s\n", http.errorToString(httpCode).c_str());
      }
}
//________________________________________________________________________________ 

//________________________________________________________________________________ VOID SETUP
void setup() {
  // put your setup code here, to run once:

  Serial.begin(115200);

  //---------------------------------------- Settings and start Lora Ra-02.
  LoRa.setPins(ss, rst, dio0);

  Serial.println("Start LoRa init...");
  if (!LoRa.begin(433E6)) {             // initialize ratio at 915 or 433 MHz
    Serial.println("LoRa init failed. Check your connections.");
    while (true);                       // if failed, do nothing
  }
  Serial.println("LoRa init succeeded.");

   WiFi.begin(ssid, password);
  Serial.println("Connecting");     
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.print("Connected to WiFi network with IP Address: ");
  Serial.println(WiFi.localIP());

  //---------------------------------------- 
}
//________________________________________________________________________________ 

//________________________________________________________________________________ VOID LOOP
void loop() {
  // put your main code here, to run repeatedly:

  //---------------------------------------- Millis or Timer to send message / command data to slaves every 1 second (see interval_SendCMD variable).
  // Messages are sent every one second is alternately.
  // > Master sends message to Slave 1, delay 1 second.
  // > Master sends message to Slave 2, delay 1 second.
  
  unsigned long currentMillis_SendMSG = millis();
  
  if (currentMillis_SendMSG - previousMillis_SendMSG >= interval_SendMSG) {
    previousMillis_SendMSG = currentMillis_SendMSG;

    Slv++;
    if (Slv > 2) Slv = 1;

    Message = "SDS" + String(Slv);

    //::::::::::::::::: Condition for sending message / command data to Slave 1 (ESP32 Slave 1).
    if (Slv == 1) {
//      Serial.println();
//      Serial.print("Send message to ESP32 Slave " + String(Slv));
//      Serial.println(Message);
      sendMessage(Message, Destination_ESP32_Slave_1);
    }
    //:::::::::::::::::

    //::::::::::::::::: Condition for sending message / command data to Slave 2 (UNO Slave 2).
//    if (Slv == 2) {
//      Serial.println();
//      Serial.print("Send message to ESP32 Slave " + String(Slv));
//      Serial.println(" : " + Message);
//      sendMessage(Message, Destination_ESP32_Slave_2);
//    }
    //:::::::::::::::::
  }
  //----------------------------------------

  //---------------------------------------- parse for a packet, and call onReceive with the result:
  onReceive(LoRa.parsePacket());
  //----------------------------------------
}
//________________________________________________________________________________ 
//<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
