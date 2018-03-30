/*
 *  File Name: SonOff_Relay_Firmware.ino
 *  
 *  Application: HomeAssistant SonOff Switch
 *  
 *  Description: This code is for the SonOff ESP8266 enabled smart
 *  relay for 120 - 240V applications.  This is the custom firmware to use
 *  with the Homeassistant system via MQTT packages.  This revision 1.1 comes equipe
 *  with the ability to connect to GPIO14 and use it as an override switch for local
 *  control.
 *  
 *  Author: M. Sperry - http://www.youtube.com/misperry
 *  Date: 03/29/2018
 *  Revision: 1.1
 *  
 *  
 */
 
#include <ESP8266WiFi.h>
#include <PubSubClient.h>

// Update these with values suitable for your network.
const char* ssid = "";
const char* password = "";
const char* mqtt_server = "";
#define mqtt_user "" //enter your MQTT username
#define mqtt_password "" //enter your password

WiFiClient espClient;
PubSubClient client(espClient);
int SwitchedPin = 12, LEDPin = 13, Override = 14, OverrideMem = 0;
bool HassSwitch = false;
String switch1;
String strTopic;
String strPayload;

void setup_wifi() {
 Serial.begin(115200);
  delay(2000);

  // We start by connecting to a WiFi network
 
  Serial.println();
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);
  
  WiFi.begin(ssid, password);
  
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
 
  Serial.println("");
  Serial.println("WiFi connected");  
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());

  pinMode(LEDPin,OUTPUT);
  digitalWrite(LEDPin, HIGH);

  
}

void callback(char* topic, byte* payload, unsigned int length) {
  payload[length] = '\0';
  strTopic = String((char*)topic);
  if(strTopic == "ha/switch1")
    {
    switch1 = String((char*)payload);
    if(switch1 == "ON")
      {
        Serial.println("ON");
        digitalWrite(SwitchedPin, HIGH);
        digitalWrite(LEDPin, LOW);
        HassSwitch = true;
      }
    else
      {
        Serial.println("OFF");
        digitalWrite(SwitchedPin, LOW);
        digitalWrite(LEDPin, HIGH);
        HassSwitch = false;
      }
    }
}
 
 
void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Attempt to connect
    if (client.connect("arduinoClient", mqtt_user, mqtt_password)) {
      Serial.println("connected");
      // Once connected, publish an announcement...
      client.subscribe("ha/#");
    } 
    else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}
 
void setup()
{
  setup_wifi(); 
  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);

  pinMode(SwitchedPin, OUTPUT);
  digitalWrite(SwitchedPin, LOW);
}
 
void loop()
{
  if (!client.connected()) {
    reconnect();
  }
  if(digitalRead(Override) != OverrideMem && !HassSwitch)
  {
    if(digitalRead(Override))
    {
      Serial.println("ON");
      digitalWrite(SwitchedPin, HIGH);
      digitalWrite(LEDPin, LOW);
      OverrideMem = 1;
    }
    else
    {
      Serial.println("OFF");
      digitalWrite(SwitchedPin, LOW);
      digitalWrite(LEDPin, HIGH);
      OverrideMem = 0;
    }
  }
  client.loop();
}
