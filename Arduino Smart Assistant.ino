

#include <SoftwareSerial.h>
#include <VoiceRecognitionV3.h>

/**
  Connection
  Arduino    VoiceRecognitionModule
   2   ------->     TX
   3   ------->     RX
*/
VR myVR(2, 3);   // 2:RX 3:TX, you can choose your favourite pins.

uint8_t records[7]; // save record
uint8_t buf[64];

/* define L298N motor drive control pins */
int Light = A0;    // IN1
int Fan = A1;   // IN2


#define light_ON    (0)
#define light_OFF   (1)
#define Fan_ON    (2)
#define Fan_OFF   (3)


void printSignature(uint8_t *buf, int len)
{
  int i;
  for (i = 0; i < len; i++) {
    if (buf[i] > 0x19 && buf[i] < 0x7F) {
      Serial.write(buf[i]);
    }
    else {
      Serial.print("[");
      Serial.print(buf[i], HEX);
      Serial.print("]");
    }
  }
}


void printVR(uint8_t *buf)
{
  Serial.println("VR Index\tGroup\tRecordNum\tSignature");

  Serial.print(buf[2], DEC);
  Serial.print("\t\t");

  if (buf[0] == 0xFF) {
    Serial.print("NONE");
  }
  else if (buf[0] & 0x80) {
    Serial.print("UG ");
    Serial.print(buf[0] & (~0x80), DEC);
  }
  else {
    Serial.print("SG ");
    Serial.print(buf[0], DEC);
  }
  Serial.print("\t");

  Serial.print(buf[1], DEC);
  Serial.print("\t\t");
  if (buf[3] > 0) {
    printSignature(buf + 4, buf[3]);
  }
  else {
    Serial.print("NONE");
  }
  Serial.println("\r\n");
}

void setup()
{
  /** initialize */
  myVR.begin(9600);

  Serial.begin(115200);
  Serial.println("Voice Control start");

  /* initialize motor control pins as output */
  pinMode(Light, OUTPUT);
  pinMode(Fan, OUTPUT);

  digitalWrite(Light, HIGH);
  digitalWrite(Fan, HIGH);

  if (myVR.clear() == 0) {
    Serial.println("Recognizer cleared.");
  } else {
    Serial.println("Not find VoiceRecognitionModule.");
    Serial.println("Please check connection and restart Arduino.");
    while (1);
  }

  if (myVR.load((uint8_t)light_ON) >= 0) {
    Serial.println("Light ON loaded");
  }

  if (myVR.load((uint8_t)light_OFF) >= 0) {
    Serial.println("Light OFF Loaded");
  }
  if (myVR.load((uint8_t)Fan_ON) >= 0) {
    Serial.println("Fan ON loaded");
  }

  if (myVR.load((uint8_t)Fan_OFF) >= 0) {
    Serial.println("Fan OFF loaded");
  }

}

void loop()
{

  int ret;
  ret = myVR.recognize(buf, 50);
  if (ret > 0) {
    switch (buf[1]) {
      case light_ON:
        digitalWrite(Light, LOW);
        break;

      case light_OFF:
        digitalWrite(Light, HIGH);
        break;

      case Fan_ON:
        digitalWrite(Fan, LOW);
        break;

      case Fan_OFF:
        digitalWrite(Fan, HIGH);
        break;

      default:
        Serial.println("Record function undefined");
        break;
    }
    /** voice recognized */
    printVR(buf);
  }
}
