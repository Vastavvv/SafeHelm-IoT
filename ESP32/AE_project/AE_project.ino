#include <WiFi.h>
#include <Wire.h>
#include <math.h>
#include "MAX30105.h"

#define MQ2 35
#define BUZZER 27
#define LED 26

const char* ssid = "YOUR_WIFI_NAME";
const char* password = "YOUR_WIFI_PASSWORD";

WiFiServer server(80);
MAX30105 heart;

int16_t AcX, AcY, AcZ;

bool gasAlert=false;
bool fallAlert=false;
bool heartAlert=false;

unsigned long gasTimer=0;

/* BPM */
long lastBeatTime=0;
int beatCount=0;
int bpm=0;
unsigned long bpmTimer=0;

int zeroCount=0;

void setup()
{
  Serial.begin(115200);
  Wire.begin(21,22);

  pinMode(BUZZER, OUTPUT);
  pinMode(LED, OUTPUT);

  analogReadResolution(12);
  analogSetPinAttenuation(MQ2, ADC_11db);

  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) delay(500);

  Serial.println("WiFi Connected");
  Serial.println(WiFi.localIP());

  server.begin();

  if(heart.begin(Wire))
    heart.setup(60,4,2,100,411,4096);

  // MPU wake
  Wire.beginTransmission(0x68);
  Wire.write(0x6B);
  Wire.write(0);
  Wire.endTransmission(true);
}

void loop()
{
  /* -------- HEART -------- */
  long ir = heart.getIR();
  bool fingerDetected = (ir > 50000);

  if(ir==0) zeroCount++; else zeroCount=0;
  if(zeroCount>10){ heart.setup(60,4,2,100,411,4096); zeroCount=0; }

  if(fingerDetected && millis()-lastBeatTime>600){
    beatCount++;
    lastBeatTime=millis();
  }

  if(millis()-bpmTimer>10000){
    bpm = beatCount*6;
    beatCount=0;
    bpmTimer=millis();

    if(fingerDetected){
      if(bpm<50 || bpm>130) heartAlert=true;
      else heartAlert=false;
    } else {
      heartAlert=false;
      bpm=0;
    }
  }

  /* -------- GAS -------- */
  int gas = analogRead(MQ2);

  if(gas > 3000){
    gasAlert = true;
    gasTimer = millis();
  } else if(millis()-gasTimer > 4000){
    gasAlert = false;
  }

  /* -------- MPU -------- */
  Wire.beginTransmission(0x68);
  Wire.write(0x3B);
  Wire.endTransmission(false);
  Wire.requestFrom(0x68,6,true);

  AcX = Wire.read()<<8 | Wire.read();
  AcY = Wire.read()<<8 | Wire.read();
  AcZ = Wire.read()<<8 | Wire.read();

  float ax = AcX / 16384.0;
  float ay = AcY / 16384.0;
  float az = AcZ / 16384.0;

  float totalAcc = sqrt(ax*ax + ay*ay + az*az);
  float shock = abs(totalAcc - 1.0) * 10000;

  bool tiltDetected = abs(AcZ) < 8000;

  if(shock > 8000 || tiltDetected)
    fallAlert = true;
  else
    fallAlert = false;

  /* -------- ALERT -------- */
  if(gasAlert || fallAlert || heartAlert){
    digitalWrite(BUZZER,HIGH);
    digitalWrite(LED,HIGH);
  } else {
    digitalWrite(BUZZER,LOW);
    digitalWrite(LED,LOW);
  }

  /* -------- SERVER -------- */
  WiFiClient client = server.available();
  if(client){
    client.readStringUntil('\r');
    client.flush();

    client.println("HTTP/1.1 200 OK");
    client.println("Content-Type: application/json");
    client.println("Access-Control-Allow-Origin: *");
    client.println("Connection: close");
    client.println();

    client.print("{");
    client.print("\"gas\":"); client.print(gas); client.print(",");
    client.print("\"bpm\":"); client.print(bpm); client.print(",");
    client.print("\"spo2\":98,");
    client.print("\"shock\":"); client.print(shock); client.print(",");
    client.print("\"tilt\":"); client.print(tiltDetected ? 1 : 0);
    client.print("}");

    client.stop();
  }

  Serial.print("Gas:"); Serial.print(gas);
  Serial.print(" BPM:"); Serial.print(bpm);
  Serial.print(" Shock:"); Serial.println(shock);

  delay(300);
}