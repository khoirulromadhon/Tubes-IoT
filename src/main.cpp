#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <PubSubClient.h>

#define triggerPin D6
#define echoPin D5
#define buzzer  D3

const char *ssid = "JTI-POLINEMA";            // sesuaikan dengan username wifi
const char *password = "jtifast!";        // sesuaikan dengan password wifi
const char *mqtt_server = "broker.hivemq.com"; // isikan server broker

WiFiClient espClient;
PubSubClient client(espClient);

long now = millis();
long lastMeasure = 0;
String macAddr = "";

void setup_wifi()
{
  delay(10);
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.print("WiFi connected - ESP IP address: ");
  Serial.println(WiFi.localIP());
  macAddr = WiFi.macAddress();
  Serial.println(macAddr);
}

void reconnect()
{
  while (!client.connected())
  {
    Serial.print("Attempting MQTT connection...");
    if (client.connect(macAddr.c_str()))
    {
      Serial.println("connected");
    }
    else
    {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      delay(5000);
    }
  }
}


void setup() {
   Serial.begin (115200);
   pinMode(triggerPin, OUTPUT);
   pinMode(echoPin, INPUT);
   pinMode(buzzer, OUTPUT);
   pinMode(BUILTIN_LED, OUTPUT);
   Serial.println("Mqtt Node-RED");
   setup_wifi();
   client.setServer(mqtt_server, 1883);
}

void loop() {
   long duration, jarak;
   digitalWrite(triggerPin, LOW);
   delayMicroseconds(2);
   digitalWrite(triggerPin, HIGH);
   delayMicroseconds(10);
   digitalWrite(triggerPin, LOW);
   duration = pulseIn(echoPin, HIGH);
   jarak = duration * 0.034 / 2;
   // Serial.print(jarak);
   // Serial.println(" cm");
   // delay(2000);

   if (jarak <= 200) {
      Serial.print(jarak);
      Serial.println(" cm");
      Serial.println("Buzzer on");
      // digitalWrite(buzzer, LOW);
      delay(2000); 
   }
   if (jarak > 200) {
      Serial.print(jarak);
      Serial.println(" cm");
      Serial.println("Buzzer off");
      // digitalWrite(buzzer, HIGH);
      delay(2000); 
   }

   if (!client.connected()){
      reconnect();
   }
   if (!client.loop()){
      client.connect(macAddr.c_str());
   }

   now = millis();

   if (now - lastMeasure > 5000){
      lastMeasure = now;
      
      jarak = duration * 0.034 / 2;
      static char jrk[7];

      dtostrf(jarak, 4, 2, jrk);

      Serial.println(jrk);

      client.publish("1941720102/room/jrk", jrk);
   }
}