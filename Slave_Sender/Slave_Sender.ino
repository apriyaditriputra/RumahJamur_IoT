
//>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>> Test Slave (1 or 2) ESP32 Lora Ra-02
//---------------------------------------- Include Library.
#include <SPI.h>
#include <LoRa.h>
#include "DHT.h"
#include <Wire.h> 
#include <LiquidCrystal_I2C.h>
LiquidCrystal_I2C lcd(0x27, 20, 4);

//----------------------------------------

//---------------------------------------- Defines the DHT11 Pin and the DHT type.
#define DHTPIN      27
#define DHTTYPE     DHT22
//----------------------------------------

//---------------------------------------- Defines LED Pins.
#define LED_1_Pin   27
#define LED_2_Pin   25
//----------------------------------------

//---------------------------------------- LoRa Pin / GPIO configuration.
#define ss 5
#define rst 14
#define dio0 2
//----------------------------------------

// Initializes the DHT sensor (DHT11).
DHT dht11(33, DHTTYPE);
DHT dht22(26, DHT22);
DHT dht33(27, DHT22);
DHT dht44(25, DHT22);
//---------------------------------------- Variable declaration to hold incoming and outgoing data.
String Incoming = "";
String Message = "";
//----------------------------------------

//---------------------------------------- LoRa data transmission configuration.
////////////////////////////////////////////////////////////////////////////
// PLEASE UNCOMMENT AND SELECT ONE OF THE "LocalAddress" VARIABLES BELOW. //
////////////////////////////////////////////////////////////////////////////

byte LocalAddress = 0x02;       //--> address of this device (Slave 1).
//byte LocalAddress = 0x03;       //--> address of this device (Slave 2).

byte Destination_Master = 0x01; //--> destination to send to Master (ESP32).
//----------------------------------------

//---------------------------------------- Variable declarations for temperature and humidity values.
float h = 0.0;
float t = 0.0;
float h2 = 0.0;
float t2 = 0.0;
float h3 = 0.0;
float t3 = 0.0;
float h4 = 0.0;
float t4 = 0.0;
byte LED_1_State;
byte LED_2_State;
//----------------------------------------

//---------------------------------------- Millis / Timer to update temperature and humidity values from DHT11 sensor.
unsigned long previousMillis_UpdateDHT11 = 0;
const long interval_UpdateDHT11 = 2000;
//----------------------------------------
int x = 1;
int a = 0;
int b = 0;
int c = 0;
int d = 0;
int sum;

int cond;
int timee=0;
//________________________________________________________________________________ Subroutines for sending data (LoRa Ra-02).
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
  Serial.println("Received from: 0x" + String(sender, HEX));
  //Serial.println("Message length: " + String(incomingLength));
  Serial.println("Message: " + Incoming);
  //Serial.println("RSSI: " + String(LoRa.packetRssi()));
  //Serial.println("Snr: " + String(LoRa.packetSnr()));
  //----------------------------------------

  // Calls the Processing_incoming_data() subroutine.
  Processing_incoming_data();
}
//________________________________________________________________________________

//________________________________________________________________________________ Subroutines to process data from incoming messages, then send messages to the Master.
void Processing_incoming_data() {
  //---------------------------------------- Conditions for sending messages to Master.
  /////////////////////////////////////////////////////////////////////////////////////////
  // PLEASE UNCOMMENT THE LINE OF CODE BELOW IF THIS CODE OR THIS DEVICE IS FOR SLAVE 1. //
  /////////////////////////////////////////////////////////////////////////////////////////

  if (Incoming == "SDS1") {


    Message = "";
    Message = String(t) + ";" + String(h) + ";" + String(t2) + ";" + String(h2) + ";" + String(t3) + ";" + String(h3) + ";" + String(t4) + ";" + String(h4)+ ";" + String(cond);

    Serial.println();
    Serial.println("Send message to Master");
    Serial.print("Message: ");
    Serial.println(Message);

    sendMessage(Message, Destination_Master);
  }

}
//________________________________________________________________________________

//________________________________________________________________________________ VOID SETUP
void setup() {
  // put your setup code here, to run once:

  Serial.begin(115200);

  pinMode(LED_1_Pin, OUTPUT);
  pinMode(LED_2_Pin, OUTPUT);

  pinMode(15, OUTPUT);
  delay(100);

  digitalWrite(LED_1_Pin, HIGH);
  digitalWrite(LED_2_Pin, LOW);

  dht11.begin();
  dht22.begin();
  dht33.begin();
  dht44.begin();
  lcd.begin();
  lcd.backlight();

  //---------------------------------------- Settings and start Lora Ra-02.
  LoRa.setPins(ss, rst, dio0);

  Serial.println();
  Serial.println("Start LoRa init...");
  if (!LoRa.begin(433E6)) {             // initialize ratio at 915 or 433 MHz
    Serial.println("LoRa init failed. Check your connections.");
    while (true);                       // if failed, do nothing
  }
  Serial.println("LoRa init succeeded.");
  //----------------------------------------
}
//________________________________________________________________________________

//________________________________________________________________________________ VOID LOOP
void loop() {
  // put your main code here, to run repeatedly:
//  
  Serial.print(h);
  Serial.print("||");
  Serial.print(t);
  Serial.print("||");
  Serial.print(h2);
  Serial.print("||");
  Serial.print(t2);
  Serial.print("||");
  Serial.print(h3);
  Serial.print("||");
  Serial.print(t3);
  Serial.print("||");
  Serial.print(h4);
  Serial.print("||");
  Serial.print(t4);
  Serial.print("||");
//    Serial.print(h5);
//  Serial.print("||");
//  Serial.print(t5);
//  Serial.print("||");
//    Serial.print(h6);
//  Serial.print("||");
//  Serial.print(t6);
//  Serial.print("||");
  Serial.println(sum);


//  delay(2000);

  //---------------------------------------- Millis / Timer to update the temperature and humidity values ​​from the DHT11 sensor every 2 seconds (see the variable interval_UpdateDHT11).
  unsigned long currentMillis_UpdateDHT11 = millis();

  if (currentMillis_UpdateDHT11 - previousMillis_UpdateDHT11 >= interval_UpdateDHT11) {
    previousMillis_UpdateDHT11 = currentMillis_UpdateDHT11;

    // Reading temperature or humidity takes about 250 milliseconds!
    // Sensor readings may also be up to 2 seconds 'old' (its a very slow sensor)
    h = dht11.readHumidity();
    h2 = dht22.readHumidity();
    h3 = dht33.readHumidity();
    h4 = dht44.readHumidity();
// h5 = dht55.readHumidity();
//  h6 = dht66.readHumidity();
    // Read temperature as Celsius (the default)
    t = dht11.readTemperature();
    t2 = dht22.readTemperature();
    t3 = dht33.readTemperature();
    t4 = dht44.readTemperature();
//t5 = dht55.readTemperature();
//t6 = dht66.readTemperature();
//    lcd.clear();
//
  // Tampilkan data dari sensor pada baris yang berbeda
  lcd.setCursor(0, 0);
  lcd.print("DHT1: ");
  lcd.print(t);
  lcd.print("C ");
  lcd.print(h);
  lcd.print("%");

  lcd.setCursor(0, 1);
  lcd.print("DHT2: ");
  lcd.print(t2);
  lcd.print("C ");
  lcd.print(h2);
  lcd.print("%");

  lcd.setCursor(0, 2);
  lcd.print("DHT3: ");
  lcd.print(t3);
  lcd.print("C ");
  lcd.print(h3);
  lcd.print("%");

  lcd.setCursor(0, 3);
  lcd.print("DHT4: ");
  lcd.print(t4);
  lcd.print("C ");
  lcd.print(h4);
  lcd.print("%");

//
    if (h <= 87 ) {
      a = 1;
    }
    else {
      a = 0;
    }
    if (h2 <= 87 ) {
      b = 1;
    }
    else {
      b = 0;
    }
    if (h3 <= 87 ) {
      c = 1;
    }
    else {
      c = 0;
    }
    if (h4 <= 87) {
      d = 1;
    }
    else {
      d = 0;
    }
//        if (h5 <= 80) {
//     e = 1;
//    }
//    else {
//      e = 0;
//    }
//
//    if (h6 <= 80) {
//      d = 1;
//    }
//    else {
//      d = 0;
//    }


    sum = a + b + c + d;
    if (sum >= 3) {
      if(timee<100){
      cond=1;
      digitalWrite(15, HIGH);
      delay(5000);
      digitalWrite(15, LOW);
      delay(5000);


      }
    }
    else {
      digitalWrite(15, LOW);
      cond=0;
      timee=0;
    }
   timee++;
    // Read temperature as Fahrenheit (isFahrenheit = true)
    //float f = dht.readTemperature(true);

    // Check if any reads failed and exit early (to try again).
    if (isnan(h) || isnan(t)) {
      Serial.println(F("Failed to read from DHT sensor!"));
      h = 0;
      t = 0.0;
    }
  }
  //----------------------------------------

  //---------------------------------------- parse for a packet, and call onReceive with the result:
  onReceive(LoRa.parsePacket());
  //----------------------------------------
}
//________________________________________________________________________________
//<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
