#include <ESP8266WiFi.h>
#include <PubSubClient.h>

// Wireless Settings
const char* ssid = "Bedlam-HA";
const char* password = "buggeroff!";
const char* wifihstnme = "esp8266switch01";

// MQTT Broker Settings
const char* mqtt_server = "192.168.1.222";
const char* mqtt_id = "pi";
const char* mqtt_pass = "raspberry";

// MQTT Topic
const char* mqtt_cmdtopic = "esp8266/switch1/cmd";
const char* mqtt_stattopic = "esp8266/switch1/stat";

WiFiClient espClient;
PubSubClient client(espClient);
int Pin0 = 0;
String switch1;
String strTopic;
String strPayload;

void setup_wifi() {

  delay(10);
  // We start by connecting to a WiFi network
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
  Serial.println("WiFi connected");
  WiFi.hostname(wifihstnme);
  delay(1000);
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
  Serial.println("Hostname: ");
  Serial.println(WiFi.hostname());
}

void callback(char* topic, byte* payload, unsigned int length) {
  payload[length] = '\0';
  strTopic = String((char*)topic);
  if(strTopic == mqtt_cmdtopic)
    {
    switch1 = String((char*)payload);
    if(switch1 == "ON")
      {
        Serial.println("ON");
        digitalWrite(Pin0, HIGH);
        // Publish back status and set to retain
        client.publish(mqtt_stattopic, "ON", true);
      }
    else
      {
        Serial.println("OFF");
        digitalWrite(Pin0, LOW);
        // Publish back status and set to retain
        client.publish(mqtt_stattopic, "OFF", true);
      }
    }
}
 
 
void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("Attempting to log into MQTT Broker...");
    Serial.println("Broker:");
    Serial.println(mqtt_server);
    Serial.println("SUB (Command) Topic:");
    Serial.println(mqtt_cmdtopic);
    Serial.println("PUB (Status) Topic:");
    Serial.println(mqtt_stattopic);
    // Attempt to connect
    if (client.connect("esp8266",mqtt_id,mqtt_pass)) {
      Serial.println("connected");
      // Once connected, publish an announcement...
      client.subscribe(mqtt_cmdtopic);
    } else {
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
  Serial.begin(115200);
  setup_wifi(); 
  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);

  pinMode(Pin0, OUTPUT);
  digitalWrite(Pin0, HIGH);
}
 
void loop()
{
  if (!client.connected()) {
    reconnect();
  }
  client.loop();
}
