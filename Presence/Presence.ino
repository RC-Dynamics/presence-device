#include <ESP8266WiFi.h> 
#include <PubSubClient.h> // Library for MQTT
#include <SPI.h>
#include <MFRC522.h>

#define TOP_SUB "ChamadaRFID/UFPE/CIn/Sala_1_SUB"
#define TOP_PUB "ChamadaRFID/UFPE/CIn/Sala_1_PUB"
#define clientID "UFPE/CIn/Sala_1"
#define ALIVE "ChamadaRFID/UFPE/CIn/Sala_1/Alive"

const char* mqtt_server = "iot.eclipse.org";


// NEED TO CONFIGURE WIFI NETWORK
const char* ssid = "RoboBoy";
const char* password = "lucas123";

WiFiClient espClient;
PubSubClient MQTT(espClient);

#define pin0 0
#define pin1 2
#define pin2 4


#define SS_PIN D0
#define RST_PIN D1
MFRC522 mfrc522(SS_PIN, RST_PIN);   // Create MFRC522 instance.
String card;

// ============= Prototypes =============
void subscribed(char* topic, byte* payload, unsigned int length);
void setupWifi();
void mqttReconnect();
char readRFID();
// =======================================



void setup()
{
  Serial.begin(115200);
  pinMode(pin0, OUTPUT);
  pinMode(pin1, OUTPUT);
  pinMode(pin2, OUTPUT);
// Connecting to WiFi
  turnOnLed(2); 
  setupWifi();
// Starting MQTT
  MQTT.setServer(mqtt_server, 1883);
  MQTT.setCallback(subscribed); // Receive Function
// Init RFID Sensor
  SPI.begin();
  delay(100);
  mfrc522.PCD_Init();   // Inicia MFRC522
  turnOnLed(0);  
    
}

void loop()
{
    if(WiFi.status() != WL_CONNECTED){
      turnOnLed(2);
      setupWifi();
      turnOnLed(0);
    }
    if (!MQTT.connected()) {
      turnOnLed(2);
      mqttReconnect();
      turnOnLed(0);
    }
    

    MQTT.loop();
    if(readRFID()){
      turnOnLed(3);
      MQTT.publish(TOP_PUB, card.c_str());
      Serial.println("Published...");
      delay(1000);
      turnOnLed(1);
    }

}
// ============ FUCNTIONS =============
// ***************** CallBack for MQTT *****************
void subscribed(char* topic, byte* payload, unsigned int length) {
  turnOnLed(1);
  Serial.print("Message arrived [ ");
  Serial.print(topic);
  Serial.print(" ]: ");
  for (int i = 0; i < length; i++) {
    Serial.print((char)payload[i]);
  }
  Serial.println();
  if(payload[0] == 't')
    turnOnLed(0);
  //Here to control de LED
}
// ***************** Connectiong to WiFi *****************
void setupWifi() {
  delay(10);
  // We start by connecting to a WiFi network
  Serial.print("Connecting to ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.print("WiFi at IP:   ");
  Serial.println(WiFi.localIP());
}

// ***************** MQTT RECONNECT *****************
void mqttReconnect() {
  // Loop until we're reconnected
  while (!MQTT.connected()) {
    Serial.print("Attempting MQTT connection... ");
    turnOnLed(2);
    // Create a random client ID

    // Attempt to connect
    if (MQTT.connect(clientID)) {
      Serial.println("Connected");
      // Once connected, publish an announcement...
      MQTT.publish(ALIVE, clientID);
      // ... and resubscribe
      MQTT.subscribe(TOP_SUB); 
    } else {
      Serial.print("Failed, rc= ");
      Serial.print(MQTT.state());
      Serial.println(" trying again in 2 seconds");
      // Wait 5 seconds before retrying
      delay(2500);
    }
    while (WiFi.status() != WL_CONNECTED) {
      delay(500);
      Serial.print(".");
    }
    Serial.println();
  }
}

// ***************** READ SENSOR RFID ***************** 
char readRFID(){
    if ( ! mfrc522.PICC_IsNewCardPresent()) 
    {
      card.remove(0, card.length());
      return 0;
    }
    // Select one of the cards
    if ( ! mfrc522.PICC_ReadCardSerial()) 
    {
      card.remove(0, card.length());
      return 0;
    }
    Serial.print("RFID read:  ");
    for (byte i = 0; i < mfrc522.uid.size; i++) 
    {
      Serial.print(mfrc522.uid.uidByte[i] < 0x10 ? " 0" : " ");
      Serial.print(mfrc522.uid.uidByte[i], HEX);
      card.concat(String(mfrc522.uid.uidByte[i], HEX));
    }
    Serial.println();
    return 1;
}

void turnOnLed(int color){
  // 0 -> Green
  // 1 -> Blue
  // 2 -> Red

    digitalWrite(pin0, (color == 0 || color == 3)? LOW: HIGH);
    digitalWrite(pin1, (color == 1 || color == 3)? LOW: HIGH);
    digitalWrite(pin2, color == 2? LOW: HIGH);    

}
