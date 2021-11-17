/* This health wearable project code was 
  written by Kupoluyi Jesutofunmi Philip for 
  Mr. Yomi Babalola for research purposes 
  on 1 September, 2021. 
    
 * It uses the ThingSpeak cloud platform
 * The project device uses: 
 * ESP32 as its microcontroller unit, 
 * DHT11 as its temperature and humidity sensor,   
 * Pulse sensor,
 * AD8232 as its ECG sensor.

*/
#define USE_ARDUINO_INTERRUPTS true
//Libraries needed 
#include <PulseSensorPlayground.h>
#include <Wire.h>
#include <DHT.h>
#include <DHT_U.h>
#include <Adafruit_Sensor.h>
#include <WiFi.h>
#include "EEPROM.h"
#include "intro.h"
#include "secrets.h"
#include "ThingSpeak.h" // always include last

//Pin definition
#define DHTPIN 13
#define ECG_OUT 39
#define P_OUT 36
#define BUZZ 32
#define LO_P 12
#define LO_N 14
#define Heart_LED 2

//LED pin definition
#define PWR_LED 26
#define UPLOAD_LED 33
#define READ_LED 25
#define CONN_LED 27

//EEPROM size definition
#define EEPROM_SIZE 64

//DHT definition
#define DHTTYPE    DHT11 
DHT_Unified dht(DHTPIN, DHTTYPE);

//Wifi Credentials definition
char ssid[] = SECRET_SSID;   // your network SSID (name) 
char pass[] = SECRET_PASS;   // your network password
int keyIndex = 0;            // your network key Index number
WiFiClient  client;

//ThingSpeak Channel Definition
unsigned long myChannelNumber = SECRET_CH_ID;
const char * myWriteAPIKey = SECRET_WRITE_APIKEY;

//Sensor variable definition
int temp;
int pulse;
int ecg;
int addr = 2;
int pulse_addr = 1;
int dht_addr = 0;
int Threshold = 2000;

PulseSensorPlayground pulseSensor;

//One-time setup code
void setup() {
  pinMode(PWR_LED, OUTPUT);
  pinMode(CONN_LED, OUTPUT);
  pinMode(UPLOAD_LED, OUTPUT);
  pinMode(READ_LED, OUTPUT);
  pinMode(BUZZ, OUTPUT);
  pinMode(LO_P , INPUT);
  pinMode(LO_N , INPUT);

  pulseSensor.analogInput(P_OUT);
  pulseSensor.setThreshold(Threshold);
  pulseSensor.blinkOnPulse(Heart_LED);
  if (pulseSensor.begin()) {
    Serial.println("We created a pulseSensor Object !");  //This prints one time at Arduino power-up,  or on Arduino reset.  
  }
  if (!EEPROM.begin(EEPROM_SIZE))
  {
    Serial.println("failed to initialise EEPROM"); delay(1000000);
  }
Serial.begin(115200);  //Initialize serial
  while (!Serial) {
    ; // wait for serial port to connect.
  }
 
  dht.begin();
  sensor_t sensor;
  WiFi.mode(WIFI_STA);   
  ThingSpeak.begin(client);  // Initialize ThingSpeak
  intro();
  LED_intro();
}

//Main Code
void loop() {
// Connect or reconnect to WiFi
  if(WiFi.status() != WL_CONNECTED){
    Serial.print("Attempting to connect to SSID: ");
    Serial.println(SECRET_SSID);
    while(WiFi.status() != WL_CONNECTED){
      WiFi.begin(ssid, pass);  // Connect to WPA/WPA2 network.
      Serial.print(".");
      digitalWrite(UPLOAD_LED, LOW);
      digitalWrite(READ_LED, LOW);
      digitalWrite(CONN_LED, LOW);
      delay(2500); 
      digitalWrite(CONN_LED, HIGH);
      digitalWrite(UPLOAD_LED, LOW);
      digitalWrite(READ_LED, LOW);
      delay(1000);    
    } 
    Serial.println("\nConnected.");
    digitalWrite(CONN_LED, HIGH);
    digitalWrite(UPLOAD_LED, LOW);
    digitalWrite(READ_LED, LOW);
  }
 if(digitalRead(LO_P)!=1 && digitalRead(LO_N)!=1 ){
  digitalWrite(READ_LED, HIGH);
  delay(1000);
  digitalWrite(READ_LED, LOW);
  delay(250);
   
  ECG();
  Temp();
  Pulse();
for (int i = 2; i < EEPROM_SIZE; i++)
    { 
      Serial.println(int(EEPROM.read(i)));
      ThingSpeak.setField(1, byte(EEPROM.read(i)) );
      ThingSpeak.setField(2, byte(EEPROM.read(1)) );
      ThingSpeak.setField(3, byte(EEPROM.read(0)) );
      //Upload all
int h = ThingSpeak.writeFields(myChannelNumber, myWriteAPIKey);
  if(h == 200){
    Serial.println("Health Wearable update successful.");
    digitalWrite(UPLOAD_LED, HIGH);
    delay(2000);
    digitalWrite(UPLOAD_LED, LOW);
    delay(500);
  }
  
  else{
    Serial.println("Problem updating channel. HTTP error code " + String(h));
    digitalWrite(UPLOAD_LED, HIGH);
    delay(500);
    digitalWrite(UPLOAD_LED, LOW);
    delay(500);
    digitalWrite(UPLOAD_LED, HIGH);
    delay(500);
    digitalWrite(UPLOAD_LED, LOW);
    delay(500);
    digitalWrite(UPLOAD_LED, HIGH);
    delay(500);
    digitalWrite(UPLOAD_LED, LOW);
    delay(500);
  } 
       //Refresh time
  delay(20000);
    }

 }
 else{
  readErr();
  }
}
void ECG(){
  delay(1000);
  while(addr != EEPROM_SIZE){
  ecg = analogRead(P_OUT) / 16; 
  Serial.println(ecg);
  EEPROM.write(addr, ecg);
  addr = addr + 1;
  digitalWrite(UPLOAD_LED, LOW);
  digitalWrite(READ_LED, HIGH);
  delay(250);
  digitalWrite(UPLOAD_LED, LOW);
  digitalWrite(READ_LED, LOW);
  delay(250);
  }
  if (addr == EEPROM_SIZE)
  {
    Serial.println("Done Reading!");
    addr = 2;
    EEPROM.commit();
    digitalWrite(UPLOAD_LED, LOW);
    digitalWrite(READ_LED, HIGH);
    }
    
  
  }

//Temperature Calculation 
void Temp(){
  sensors_event_t event;
  dht.temperature().getEvent(&event);
  if (isnan(event.temperature)) {
    Serial.println(F("Error reading temperature!"));
  }
  else {
int temp = event.temperature;
EEPROM.write(0, temp);
delay(100);
EEPROM.commit();
  }
  }

//pulse calculation 
void Pulse(){
pulse = pulseSensor.getBeatsPerMinute();
ThingSpeak.setField(2, pulse);
EEPROM.write(1, pulse);
delay(100);
EEPROM.commit();
 }
 void LED_intro(){
  // LED and buzzer intro
digitalWrite(PWR_LED, HIGH);
digitalWrite(UPLOAD_LED, HIGH);
digitalWrite(CONN_LED, HIGH);
digitalWrite(READ_LED, HIGH);
digitalWrite(BUZZ, HIGH);
delay(500);
digitalWrite(PWR_LED, HIGH);
digitalWrite(UPLOAD_LED, LOW);
digitalWrite(CONN_LED, LOW);
digitalWrite(READ_LED, LOW);
delay(500);
digitalWrite(PWR_LED, LOW);
digitalWrite(UPLOAD_LED, HIGH);
digitalWrite(CONN_LED, LOW);
digitalWrite(READ_LED, LOW);
delay(500);
digitalWrite(PWR_LED, LOW);
digitalWrite(UPLOAD_LED, LOW);
digitalWrite(CONN_LED, HIGH);
digitalWrite(READ_LED, LOW);
delay(500);
digitalWrite(PWR_LED, LOW);
digitalWrite(UPLOAD_LED, LOW);
digitalWrite(CONN_LED, LOW);
digitalWrite(READ_LED, HIGH);
delay(500);
digitalWrite(PWR_LED, LOW);
digitalWrite(UPLOAD_LED, LOW);
digitalWrite(CONN_LED, LOW);
digitalWrite(READ_LED, LOW);
delay(500);
digitalWrite(PWR_LED, HIGH);
digitalWrite(UPLOAD_LED, HIGH);
digitalWrite(CONN_LED, HIGH);
digitalWrite(READ_LED, HIGH);
digitalWrite(BUZZ, HIGH);
delay(1000);
digitalWrite(BUZZ, LOW);
  }
 void readErr(){
  digitalWrite(READ_LED, HIGH);
  delay(250);
  digitalWrite(READ_LED, LOW);
  delay(250);
  }
