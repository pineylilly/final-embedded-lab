#include <SoftwareSerial.h>
#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <ArduinoJson.h>

// UART between STM32 and NodeMCU
EspSoftwareSerial::UART testSerial;

WiFiClient espClient;
PubSubClient client(espClient);
char msg[256];
char toSTMmsg[256];
char emptyMsg[2] = {' ', '\0'};
String receiveString;
String classifyString;

// Wifi and NETPIE Configuration
const char* ssid = "Jirayut's Galaxy S21 Ultra 5G";
const char* password = "new.47159";
const char* mqtt_server = "broker.netpie.io";
const int mqtt_port = 1883;
// Test Account
const char* mqtt_Client = "3981ac03-e013-4154-96c7-2b394d57505b"; // Client ID
const char* mqtt_username = "g8Lx31p1wtm6Q6npqsfztJ4Qed6WUprF";   // Token
const char* mqtt_password = "8M8o_4a7_-n4TBdpVR(a7#L!BX3nhewf";   // Secret

// Try to reconnect if disconnect
void reconnect() {
    while (!client.connected()) {
        Serial.print("Attempting MQTT connection…");
        if (client.connect(mqtt_Client, mqtt_username, mqtt_password)) {
            Serial.println("connected");
            // Subscribe change of state
            client.subscribe("@msg/state");
            // Subscribe response from requested data
            client.subscribe("@private/shadow/data/get/response");
        } else {
            Serial.print("failed, rc=");
            Serial.print(client.state());
            Serial.println("try again in 5 seconds");
            delay(5000);
        }
    }
}

// Send data to STM32
void UARTSend(int autoMode, int pwmFan, int pwmWater) {
  testSerial.write(autoMode);
  delay(50);
  testSerial.write(pwmFan);
  delay(50);
  testSerial.write(pwmWater);
  delay(50);
}

// Wait for message callback
void callback(char* topic, byte* payload, unsigned int length) {
    Serial.print("Message arrived [");
    Serial.print(topic);
    Serial.print("] ");
    String message;
    for (int i = 0; i < length; i++) {
        message = message + (char)payload[i];
    }
    Serial.println(message);
    if(String(topic) == "@msg/state" || String(topic) == "@private/shadow/data/get/response") {
        // Create json parser
        StaticJsonDocument<200> doc;
        // string to char[]
        message.toCharArray(toSTMmsg, (message.length() + 1));
        // Parsing json
        deserializeJson(doc, toSTMmsg);
        //Serial.println((int)doc["data"]["autoMode"]);
        // classifyString = "{\"data\": {\"autoMode\" : " + String(doc["data"]["autoMode"]) + ", \"pwmFan\" : " + String(doc["data"]["pwmFan"]) + ", \"pwmWater\" : " + String(doc["data"]["pwmWater"]) + "}}";
        // Serial.println(classifyString);
        // classifyString.toCharArray(toSTMmsg, (classifyString.length() + 1));
        //Serial.println((int)doc["data"]["autoMode"]);
        UARTSend((int)doc["data"]["autoMode"], (int)doc["data"]["pwmFan"], (int)doc["data"]["pwmWater"]);
    }
}

// Publish (Send) data to NETPIE
void sendData(String data) {
  if (!client.connected()) {
      reconnect();
  }
  client.loop();
  Serial.println(data);
  data.toCharArray(msg, (data.length() + 1));
  client.publish("@shadow/data/update", msg);
}

// Request data from NETPIE
void requestData() {
if (!client.connected()) {
      reconnect();
  }
  client.loop();
  Serial.println("request data");
  client.publish("@shadow/data/get", " ");
  Serial.println("waiting request data");
}

void setup() {
  testSerial.begin(9600, EspSoftwareSerial::SWSERIAL_8N1, D7, D8, false, 95, 11);
  Serial.begin(115200);

  // Try to connect to WiFi
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
  client.setServer(mqtt_server, mqtt_port);
  client.setCallback(callback);

}

void loop() {
  if (!client.connected()) {
    reconnect();
  }
  client.loop();
  while (testSerial.available() > 0) {
    char c = testSerial.read();
    //Serial.print(c);
    if (c == '\0' && receiveString == "") continue;
    if (c != '\0') {
      receiveString += c;
    }
    else {
      if (receiveString == "request") {
        requestData();
      }
      else {
        sendData(receiveString);
      }
      receiveString = "";
    }
    yield();
  }
}