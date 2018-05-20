/****************************************
 * Include Libraries
 ****************************************/
#include <WiFi.h>
#include <PubSubClient.h>

/****************************************
 * Define Constants
 ****************************************/
#define WIFISSID "Put_your_wifi_name_here" // Put your WifiSSID here
#define PASSWORD "Put_your_wifi_password_here" // Put your wifi password here
#define TOKEN "Put_your_Ubidots_TOKEN_here" // Put your Ubidots' TOKEN
#define MQTT_CLIENT_NAME "Put_your_MQTT_client_name_here" // MQTT client Name, please enter your own 8-12 alphanumeric character ASCII string; 
                                           //it should be a random and unique ascii string and different from all other devices

#define VARIABLE_LABEL_SUBSCRIBE "relay" // Assing the variable label
#define DEVICE_LABEL "esp32" // Assig the device label

#define RELAY 16 // Set the GPIO16 as RELAY

char mqttBroker[]  = "things.ubidots.com";
char payload[100];
char topic[150];
char topicSubscribe[100];

/****************************************
 * Auxiliar Functions
 ****************************************/
WiFiClient ubidots;
PubSubClient client(ubidots);

void callback(char* topic, byte* payload, unsigned int length) {
  char p[length + 1];
  memcpy(p, payload, length);
  p[length] = NULL;
  String message(p);
  if (message == "0") {
    digitalWrite(RELAY, LOW);
  } else {
    digitalWrite(RELAY, HIGH);
  }
  
  Serial.write(payload, length);
  Serial.println();
}

void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.println("Attempting MQTT connection...");
    
    // Attemp to connect
    if (client.connect(MQTT_CLIENT_NAME, TOKEN, "")) {
      Serial.println("Connected");
      client.subscribe(topicSubscribe);
    } else {
      Serial.print("Failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 2 seconds");
      // Wait 2 seconds before retrying
      delay(2000);
    }
  }
}

/****************************************
 * Main Functions
 ****************************************/
void setup() {
  Serial.begin(115200);
  WiFi.begin(WIFISSID, PASSWORD);
  // Assign the pin as OUTPUT 
  pinMode(RELAY, OUTPUT);

  Serial.println();
  Serial.print("Wait for WiFi...");
  
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(500);
  }
  
  Serial.println("");
  Serial.println("WiFi Connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
  client.setServer(mqttBroker, 1883);
  client.setCallback(callback);

  sprintf(topicSubscribe, "/v1.6/devices/%s/%s/lv", DEVICE_LABEL, VARIABLE_LABEL_SUBSCRIBE);
  
  client.subscribe(topicSubscribe);
}

void loop() {
  if (!client.connected()) {
    reconnect();
    client.subscribe(topicSubscribe);   
  }
  client.loop();
}
