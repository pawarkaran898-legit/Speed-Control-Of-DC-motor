#include <Wire.h>

#define USE_LCD false    // set true if you still have the second Arduino/LCD

const int trigPin = 8;
const int echoPin = 9;
const int ledG = 4, ledY = 5, ledR = 6;
const int buzzer = 10;

const int SAFE_T = 30;    // >30 cm = SAFE
const int WARN_T = 15;    // 15-30 cm = CAUTION; <15 = DANGER

// simple smoothing
float filt = 0;
bool initialized = false;

long readDistanceCm(){
  digitalWrite(trigPin, LOW); delayMicroseconds(3);
  digitalWrite(trigPin, HIGH); delayMicroseconds(10);
  digitalWrite(trigPin, LOW);
  long dur = pulseIn(echoPin, HIGH, 30000); // timeout ~30ms (~5m)
  if(dur == 0) return 400; // no echo -> far
  return dur / 58;       // us -> cm
}

void setOutputs(int d){
  // zone logic + buzzer pattern
  if(d > SAFE_T){
    digitalWrite(ledG, HIGH); digitalWrite(ledY, LOW); digitalWrite(ledR, LOW);
    noTone(buzzer);
  } else if(d >= WARN_T){
    digitalWrite(ledG, LOW); digitalWrite(ledY, HIGH); digitalWrite(ledR, LOW);
    // short beep every ~500ms
    static unsigned long t0=0; unsigned long t=millis();
    if(t - t0 < 80) tone(buzzer, 2000); else noTone(buzzer);
    if(t - t0 > 500) t0 = t;
  } else {
    digitalWrite(ledG, LOW); digitalWrite(ledY, LOW); digitalWrite(ledR, HIGH);
    // fast beeps
    static unsigned long t0=0; unsigned long t=millis();
    if((t/100)%2==0) tone(buzzer, 2500); else noTone(buzzer);
  }
}

void sendToLCD(int d){
  if(!USE_LCD) return;
  const char* zone = (d > SAFE_T) ? "SAFE" : (d >= WARN_T) ? "CAUTION" : "DANGER";
  char msg[33];
  // Line1: "ADAS STATUS   " (pad to 16)
  // Line2: "d=xxxcm  ZONE " or show zone text
  snprintf(msg, sizeof(msg), "ADAS STATUS     d=%3dcm %-8s", d, zone);
  Wire.beginTransmission(8);
  Wire.write(msg);
  Wire.endTransmission();
}

void setup(){
  if(USE_LCD) Wire.begin(); // only start I2C if using LCD slave
  pinMode(trigPin, OUTPUT); pinMode(echoPin, INPUT);
  pinMode(ledG, OUTPUT); pinMode(ledY, OUTPUT); pinMode(ledR, OUTPUT);
  pinMode(buzzer, OUTPUT);
  Serial.begin(9600);        // <<-- for Python plotting use 9600 (or change Python)
}

void loop(){
  int raw = (int)readDistanceCm();

  // low-pass filter to reduce flicker
  if(!initialized){ filt = raw; initialized = true; }
  filt = 0.7f * filt + 0.3f * raw;
  int df = (int)(filt + 0.5f);

  setOutputs(df);
  sendToLCD(df);

  // compute alert state for serial
  int alertState = 0;
  if(df > SAFE_T) alertState = 0;
  else if(df >= WARN_T) alertState = 1;
  else alertState = 2;

  // CSV output for Python: timestamp_ms,raw_cm,filtered_cm,alert
  unsigned long t_ms = millis();
  Serial.print(t_ms);
  Serial.print(',');
  Serial.print(raw);
  Serial.print(',');
  Serial.print(df);
  Serial.print(',');
  Serial.println(alertState);

  delay(120); // update ~8 Hz
}
