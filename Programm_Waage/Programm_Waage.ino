
float i;
int servoUrsprung=0;
int tasterstatusOben=0;
int tasterstatusUnten=0;
int tasterOben=13;
int tasterUnten=12;
int GSM1 = 10;
int in1 = 9;
int in2 = 8;
int toleranz = 10; //nicht finaler Wert
int gewichtleicht = 100; //nicht finaler Wert
int gewichtschwer = 200; //nicht finaler Wert
int gewichtmittel = 300; //nicht finaler Wert
int gesuchtleicht = 0; //Durch Änderung von 0 zu 1 wird festgelegt, welches Gewicht gesucht wird
int gesuchtmittel = 1;
int gesuchtschwer = 0;
#include <LiquidCrystal.h>
#include <Servo.h>
#include <HX711_ADC.h>
#include <EEPROM.h>
#include <Stepper.h>;
const int HX711_dout = 4; //mcu > HX711 dout pin
const int HX711_sck = 5; //mcu > HX711 sck pin
Servo servoblau;
Servo servo;
LiquidCrystal lcd(12, 11, 5, 4, 3, 2);
Stepper MaschineMatthias (200,1,2,3,4);

HX711_ADC LoadCell(HX711_dout, HX711_sck);

int eepromAdress = 0;
unsigned long t = 0;
void calibrate() {
  Serial.println("***");
  Serial.println("Start calibration:");
  Serial.println("It is assumed that the mcu was started with no load applied to the load cell.");
  Serial.println("Now, place your known mass on the loadcell,");
  Serial.println("then send the weight of this mass (i.e. 100.0) from serial monitor.");
  float m = 0;
  boolean f = 0;
  while (f == 0) {
    LoadCell.update();
    if (Serial.available() > 0) {
      m = Serial.parseFloat();
      if (m != 0) {
        Serial.print("Known mass is: ");
        Serial.println(m);
        f = 1;
      }
      else {
        Serial.println("Invalid value");
      }
    }
  }
  float c = LoadCell.getData() / m;
  LoadCell.setCalFactor(c);
  Serial.print("Calculated calibration value is: ");
  Serial.print(c);
  Serial.println(", use this in your project sketch");
  f = 0;
  Serial.print("Save this value to EEPROM adress ");
  Serial.print(eepromAdress);
  Serial.println("? y/n");
  while (f == 0) {
    if (Serial.available() > 0) {
      char inByte = Serial.read();
      if (inByte == 'y') {
        #if defined(ESP8266) 
        EEPROM.begin(512);
        #endif
        EEPROM.put(eepromAdress, c);
        #if defined(ESP8266)
        EEPROM.commit();
        #endif
        EEPROM.get(eepromAdress, c);
        Serial.print("Value ");
        Serial.print(c);
        Serial.print(" saved to EEPROM address: ");
        Serial.println(eepromAdress);
        f = 1;
      }
      else if (inByte == 'n') {
        Serial.println("Value not saved to EEPROM");
        f = 1;
      }
    }
  }
  Serial.println("End calibration");
  Serial.println("For manual edit, send 'c' from serial monitor");
  Serial.println("***");
}
void Seperator(){
MaschineMatthias.setSpeed (60); //nicht finaler Wert
MaschineMatthias.step (50);
}
void aufzug(){
tasterstatusOben=digitalRead(tasterOben);
tasterstatusUnten=digitalRead(tasterUnten);

if (tasterstatusOben == HIGH){
  //Warten, dann nach unten fahren
digitalWrite(in1, LOW);  // Motor hält an
digitalWrite(in2, LOW);
digitalWrite(in1, HIGH);  // Motor beginnt zu rotieren
digitalWrite(in2, LOW);

}

if (tasterstatusUnten == HIGH){
digitalWrite(in1, LOW);  // Motor hält an
digitalWrite(in2, LOW);
servo.write(servoUrsprung+90);
  delay (2000);
servo.write(servoUrsprung);
  delay (500);
digitalWrite(in1, LOW);  //Motor beginnt zu rotieren
digitalWrite(in2, HIGH);
}
}
void setupAufzug() {
Serial.begin(9600);
servo.attach(11);
lcd.begin(16, 2);
pinMode(tasterOben, INPUT);
pinMode(tasterUnten, INPUT);
pinMode(GSM1, OUTPUT);
pinMode(in1, OUTPUT);
pinMode(in2, OUTPUT);
}
void changeSavedCalFactor() {
  float c = LoadCell.getCalFactor();
  boolean f = 0;
  Serial.println("***");
  Serial.print("Current value is: ");
  Serial.println(c);
  Serial.println("Now, send the new value from serial monitor, i.e. 696.0");
  while (f == 0) {
    if (Serial.available() > 0) {
      c = Serial.parseFloat();
      if (c != 0) {
        Serial.print("New calibration value is: ");
        Serial.println(c);
        LoadCell.setCalFactor(c);
        f = 1;
      }
      else {
        Serial.println("Invalid value, exit");
        return;
      }
    }
  }
  f = 0;
  Serial.print("Save this value to EEPROM adress ");
  Serial.print(eepromAdress);
  Serial.println("? y/n");
  while (f == 0) {
    if (Serial.available() > 0) {
      char inByte = Serial.read();
      if (inByte == 'y') {
        #if defined(ESP8266)
        EEPROM.begin(512);
        #endif
        EEPROM.put(eepromAdress, c);
        #if defined(ESP8266)
        EEPROM.commit();
        #endif
        EEPROM.get(eepromAdress, c);
        Serial.print("Value ");
        Serial.print(c);
        Serial.print(" saved to EEPROM address: ");
        Serial.println(eepromAdress);
        f = 1;
      }
      else if (inByte == 'n') {
        Serial.println("Value not saved to EEPROM");
        f = 1;
      }
    }
  }
  Serial.println("End change calibration value");
  Serial.println("***");
}
void setup() {
  setupAufzug();
  if (gesuchtleicht = 1){
  gesuchtleicht = 90;
  }
  if (gesuchtmittel = 1){
  gesuchtmittel = 90;
  }
  if (gesuchtschwer = 1){
  gesuchtschwer = 90;
  }
  if (gesuchtleicht = 0){
  gesuchtleicht = -90;
  }
  if (gesuchtmittel = 0){
  gesuchtmittel = -90;
  }
  if (gesuchtschwer = 0){
  gesuchtschwer = -90;
  }
  Serial.begin(9600); delay(10);
  Serial.println();
  Serial.println("Starting...");
  LoadCell.begin();
  long stabilisingtime = 2000; // tare preciscion can be improved by adding a few seconds of stabilising time
  LoadCell.start(stabilisingtime);
  if (LoadCell.getTareTimeoutFlag()) {
    Serial.println("Tare timeout, check MCU>HX711 wiring and pin designations");
  }
  else {
    LoadCell.setCalFactor(1.0); // user set calibration value (float)
    Serial.println("Startup + tare is complete");
  }
  while (!LoadCell.update());
  calibrate();
}
void loop() {
  aufzug();
  //update() should be called at least as often as HX711 sample rate; >10Hz@10SPS, >80Hz@80SPS
  //longer delay in sketch will reduce effective sample rate (be carefull with delay() in the loop)
  LoadCell.update();
  //get smoothed value from the data set
  Seperator ();
  if (millis() > t + 250) {
    float i = LoadCell.getData();
    Serial.print("Load_cell output val: ");
    Serial.println(i);
    t = millis();
}
    Seperator();
   if (i < gewichtleicht + toleranz); {
     servoblau.write(gesuchtleicht);
        servoblau.write(0);
}
    if (i < gewichtmittel + toleranz); {
      servoblau.write(gesuchtmittel);
        servoblau.write(0);
}
      if (i < gewichtschwer + toleranz); {
      servoblau.write(gesuchtschwer);
        servoblau.write(0);
}
        servoblau.write(-90);
        servoblau.write(0);
lcd.clear();
lcd.setCursor(0, 0);
lcd.print(i);
  }

 
