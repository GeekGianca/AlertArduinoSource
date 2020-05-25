#include <SPI.h>
#include <MFRC522.h>
#include <SoftwareSerial.h>
#include <String.h>

#define RST_PIN 9
#define SS_PIN 10

SoftwareSerial sim900l(7, 8);//Sim900
MFRC522 mfrc522(SS_PIN, RST_PIN);//RFID
/*Var for Sim900 configuration*/
char objectToOut[100];
boolean receveidMessage = false;
boolean isSendSuccess = false;

char received = 0;
String message;
String string;
char stringChar[100];
char *decompose;
char *p;
char *i;
String number;
String data_1;
String data_2;
String date;

char cNumber[50],
     cData_1[50],
     cData_2[50],
     cDate[50],
     cTime[15],
     cDate_[15];

boolean endBool = false;
boolean messageBool = false;

int x = 0,
    z = 0,
    a = 0;
/*End vars*/

int LED_PIN = 2;
int STOP = 0;
int BZZR = 5;
int PULSE_BZZR = 0;

/*Ultrasonido*/
int TRIG = 4;
int ECO = 3;
int LED_RED = 6;
int LED_GRN = 5;
float DURT;
float DISTANCE;
float LENGTH = 58.2;
/*Fin*/

void setup() {
  pinMode(LED_PIN, OUTPUT);
  pinMode(TRIG, OUTPUT);
  pinMode(ECO, INPUT);
  pinMode(LED_RED, OUTPUT);
  pinMode(LED_GRN, OUTPUT);
  pinMode(BZZR, OUTPUT);
  digitalWrite(6, HIGH);
  //Init sim900
  sim900l.begin(9600);
  Serial.begin(9600);
  //Init RFID
  SPI.begin();
  mfrc522.PCD_Init();
  Serial.println("Turn on Module");
  Serial.println("Trying to connect");
  //Configure received only msg
  configureTextModeReceiver();
  delay(8000);
}

void loop() {
  String uuid = "";
  /*Read if message send*/
  if (sim900l.available() > 0) {
    readMessageReceived();
  }
  /*End Receiver Sim900*/

  if (mfrc522.PICC_IsNewCardPresent()) {
    if (mfrc522.PICC_ReadCardSerial()) {
      Serial.print("UUID: ");
      for (byte i = 0; i < mfrc522.uid.size; i++) {
        if (mfrc522.uid.uidByte[i] < 0x10) {
          uuid = uuid + "0";
        } else {
          uuid = uuid + "";
        }
        uuid = uuid + mfrc522.uid.uidByte[i];
        digitalWrite(LED_PIN, HIGH);
        digitalWrite(LED_PIN, HIGH);
        digitalWrite(LED_PIN, HIGH);
      }
      Serial.println(uuid);
      if (uuid.equals("19415918626") && !receveidMessage) {
        Serial.println(uuid);
        mfrc522.PICC_HaltA();
        STOP = 0;
        digitalWrite(LED_PIN, LOW);
        isSendSuccess = false;
      }

      if (receveidMessage) {
        if (uuid == objectToOut) {
          Serial.print(uuid);
          Serial.println();
          mfrc522.PICC_HaltA();
          STOP = 0;
          digitalWrite(LED_PIN, LOW);
          receveidMessage = false;
          strncpy(objectToOut, "", sizeof(objectToOut));
          isSendSuccess = false;
        }
      }
    }
  }

  /*Ultrasonido*/
  if (STOP == 0) {
    digitalWrite(BZZR, LOW);
    digitalWrite(TRIG, HIGH);
    digitalWrite(TRIG, LOW);
    DURT = pulseIn(ECO, HIGH);
    DISTANCE = DURT * 0.034 / 2;
    if (DISTANCE > 0) {
      STOP = 1;
      Serial.print("Distancia: ");
      Serial.println(DISTANCE);
    }
  } else {
    digitalWrite(BZZR, HIGH);
    digitalWrite(LED_PIN, HIGH);
    if(!isSendSuccess){
      Serial.println("Sending message");
      sendMessage();
      isSendSuccess = true;
    }
  }
  /*Fin*/
}

void readMessageReceived() {
  received = sim900l.read();
  string += received;
  if (received == '\n') {
    Serial.println("Sim 900 Incoming message");
    string.toCharArray(stringChar, 100);
    decompose = stringChar;
    p = strtok_r(decompose, "\"", &i);
    endBool = true;
    do {
      p = strtok_r(NULL, "\"", &i);
      x = x + 1;
      if (x == 1) {
        number = p;
      }
      if (x == 2) {
        data_1 = p;
      }
      if (x == 1) {
        data_2 = p;
      }
      if (x == 1) {
        date = p;
        endBool = false;
      }
    } while (endBool);
    x = 0;
    number.toCharArray(cNumber, 50);
    data_1.toCharArray(cData_1, 50);
    data_2.toCharArray(cData_2, 50);
    for (int k = 0; k < 8; k++) {
      date[k] = cDate[k];
    }
    for (int z = 0; z < 8; z++) {
      cTime[z] = cDate[z + 9];
    }
    Serial.print("Number incoming:");
    Serial.println(cNumber);
    if (strstr(cNumber, "3013321481")) {
      Serial.print("Number:\t");
      Serial.println(cNumber);
      Serial.print("Time:\t");
      Serial.println(cTime);
      Serial.print("Date:\t");
      Serial.println(date);
      messageBool = true;
    }
    if (messageBool) {
      a = a + 1;
      if (a == 2) {
        Serial.print("Message:\t");
        Serial.println(stringChar);
        strncpy(objectToOut, stringChar, sizeof(objectToOut));
        a = 0;
        messageBool = false;
        receveidMessage = true;
      }
    }
    string = "";
  }
}

void configureTextModeReceiver()
{
  sim900l.print("AT+CMGF=1\r");//AT text mode
  delay(200);
  sim900l.print("AT+CNMI=2,2,0,0,0\r");
  delay(200);
}

void sendMessage() {
  Serial.println("Sending message...");
  sim900l.print("AT+CMGF=1\r");//AT text mode
  delay(200);
  sim900l.print("AT+CMGS=\"3013321481\"");
  delay(200);
  sim900l.print("AT+CMGS=\"Ha salido un dispositivo del area de seguridad.\"");
  delay(200);
  sim900l.println((char)26);
  delay(200);
  sim900l.println();
  Serial.println("Message send");
}
