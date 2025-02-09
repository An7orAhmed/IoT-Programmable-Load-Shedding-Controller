#include <SoftwareSerial.h>
#include <LiquidCrystal.h>
#include <Robojax_AllegroACS_Current_Sensor.h>

#define rly1 A2
#define rly2 A3
#define rly3 A4
#define rly4 A5

#define load1 10
#define load2 11
#define load3 12
#define load4 13
#define load5 0
#define load6 1

Robojax_AllegroACS_Current_Sensor acs712(0, A0);
LiquidCrystal lcd(2, 3, 4, 5, 6, 7);
SoftwareSerial wifi(8, 9);

float VCC;
int ACvolt, ACcurrent;
bool serverMode;
long prevms;

void setup() {
  wifi.begin(9600);
  lcd.begin(20, 4);

  pinMode(rly1, OUTPUT);
  pinMode(rly2, OUTPUT);
  pinMode(rly3, OUTPUT);
  pinMode(rly4, OUTPUT);

  pinMode(load1, OUTPUT);
  pinMode(load2, OUTPUT);
  pinMode(load3, OUTPUT);
  pinMode(load4, OUTPUT);
  pinMode(load5, OUTPUT);
  pinMode(load6, OUTPUT);
}

void loop() {
  checkServer();
  measureAC();
  ACcurrent = acs712.getCurrentAverage(50) * 1000;
  ACcurrent -= 60;
  if (ACcurrent < 0) ACcurrent = 0;

  if (millis() - prevms >= 1000) {
    logger();
    showOnLCD();
    controlRelay();
    sendToServer();
    prevms = millis();
  }
}

void controlRelay() {
  if (serverMode) return;
  switch (ACcurrent) {
    case 0 ... 100:  // mA
      digitalWrite(rly1, 1);
      digitalWrite(rly2, 1);
      digitalWrite(rly3, 1);
      digitalWrite(rly4, 1);
      break;
    case 101 ... 200:  // mA
      digitalWrite(rly1, 0);
      digitalWrite(rly2, 1);
      digitalWrite(rly3, 1);
      digitalWrite(rly4, 1);
      break;
    case 201 ... 300:  // mA
      digitalWrite(rly1, 0);
      digitalWrite(rly2, 0);
      digitalWrite(rly3, 1);
      digitalWrite(rly4, 1);
      break;
    case 301 ... 400:  // mA
      digitalWrite(rly1, 0);
      digitalWrite(rly2, 0);
      digitalWrite(rly3, 0);
      digitalWrite(rly4, 1);
      break;
    case 401 ... 1000:  // mA
      digitalWrite(rly1, 0);
      digitalWrite(rly2, 0);
      digitalWrite(rly3, 0);
      digitalWrite(rly4, 0);
      break;
  }
}

void sendToServer() {
  static bool mode;
  String data = "WEB=0,";
  data += (String) "?v=" + ACvolt;
  data += (String) "&c=" + ACcurrent + "$";
  if (mode == 1) data = "WEB=1,$";
  wifi.println(data);
  mode = !mode;
}

void showOnLCD() {
  lcd.setCursor(0, 0);
  lcd.print((String) "AC Volt: " + ACvolt + " V  ");
  lcd.setCursor(0, 1);
  lcd.print((String) "AC Curr.: " + ACcurrent + " mA  ");
  lcd.setCursor(0, 2);
  lcd.print((String) "RLY1: " + digitalRead(rly1) + " | RLY2: " + digitalRead(rly2));
  lcd.setCursor(0, 3);
  lcd.print((String) "RLY3: " + digitalRead(rly3) + " | RLY4: " + digitalRead(rly4));
}

void logger() {
  Serial.print((String) "ACV: " + ACvolt + " | ");
  Serial.print((String) "AMP: " + ACcurrent + " | ");
  Serial.println();
}

void measureAC() {
  static byte sampleCount;
  static int maxVal;
  int val = analogRead(A1);
  if (val > maxVal) maxVal = val;
  sampleCount++;
  if (sampleCount == 200) {
    float voltage = (VCC * maxVal) / 1023.0;
    sampleCount = 0;
    maxVal = 0;
    VCC = readVcc();
    ACvolt = voltage * 43.4;
  }
}

float readVcc() {
  ADMUX = _BV(REFS0) | _BV(MUX3) | _BV(MUX2) | _BV(MUX1);
  delay(2);
  ADCSRA |= _BV(ADSC);
  while (bit_is_set(ADCSRA, ADSC)) {};

  uint8_t low = ADCL;
  uint8_t high = ADCH;
  long result = (high << 8) | low;

  return (float)(1125300L / result) / 1000;
}

void checkServer() {
  if (wifi.available()) {
    String cmd = wifi.readString();

    if (cmd.indexOf("sc=1") != -1) serverMode = 1;
    else if (cmd.indexOf("sc=0") != -1) serverMode = 0;

    if (cmd.indexOf("l=") != -1) {
      cmd.remove(0, 2);
      digitalWrite(load1, cmd[0] - 48);
      digitalWrite(load2, cmd[1] - 48);
      digitalWrite(load3, cmd[2] - 48);
      digitalWrite(load4, cmd[3] - 48);
      digitalWrite(load5, cmd[4] - 48);
      digitalWrite(load6, cmd[5] - 48);
    }

    if (serverMode == 1) {
      if (cmd.indexOf("r1=1") != -1) digitalWrite(rly1, 1);
      else if (cmd.indexOf("r1=0") != -1) digitalWrite(rly1, 0);

      if (cmd.indexOf("r2=1") != -1) digitalWrite(rly2, 1);
      else if (cmd.indexOf("r2=0") != -1) digitalWrite(rly2, 0);

      if (cmd.indexOf("r3=1") != -1) digitalWrite(rly3, 1);
      else if (cmd.indexOf("r3=0") != -1) digitalWrite(rly3, 0);

      if (cmd.indexOf("r4=1") != -1) digitalWrite(rly4, 1);
      else if (cmd.indexOf("r4=0") != -1) digitalWrite(rly4, 0);
    }
  }
}
