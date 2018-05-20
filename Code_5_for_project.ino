/*************************************************************************************************

 * This Example sends harcoded data to Ubidots using a ESP32. The code sends a distance value 

 * between a device and its opposite endpoint to Ubidots, then the value will be managed in 

 * Ubidots to calculate the volume of a tank with the characteristics of your tank.

 * 

 * This example is given AS IT IS without any warranty.

 * 

 * Made by María Carlina Hernandez.

 *************************************************************************************************/



/****************************************

 * Include Libraries

 ****************************************/

#include <WiFi.h>

#include <PubSubClient.h>

#include <DHT.h>





/****************************************

 * Define Constants

 ****************************************/

namespace {

    const char * WIFISSID = "Assign_your_wifi_SSID_here"; // Put your WifiSSID here

    const char * PASSWORD = "Assign_your_wifi_SSID_here"; // Put your wifi password here

    const char * TOKEN = "Assign_your_Ubidots_token_here"; // Put your Ubidots' TOKEN

    const char * MQTT_CLIENT_NAME = "Assign_MQTT_client_here"; // MQTT client Name, please enter your own 8-12 alphanumeric character ASCII string; 

    const char * VARIABLE_LABEL_1 = "distance"; // Assing the variable label

    const char * VARIABLE_LABEL_2 = "humidity"; // Assing the variable label

    const char * VARIABLE_LABEL_3 = "temperature"; // Assing the variable label

    const char * DEVICE_LABEL = "esp32"; // Assig the device label

    const char * MQTT_BROKER = "things.ubidots.com";

    const int DHTPIN = 33; // Pin where is connected the DHT11

    const int DHTTYPE = DHT11; // Type of DHT

    const int trigPin = 16; // Triger pin of the HC-SR04

    const int echoPin = 17; // Echo pin of the HC-SR04  

}



/* Sensor's declarations */

long duration;

float distance;

/* Space to store the request */

char payload[300];

char topic[150];

/* Space to store values to send */

char str_sensor[10];

char str_TempSensor[10];

char str_HumSensor[10];



/****************************************

 * Auxiliar Functions

 ****************************************/

WiFiClient ubidots;

PubSubClient client(ubidots);

DHT dht(DHTPIN, DHTTYPE);



void callback(char * topic, byte * payload, unsigned int length) {

    char p[length + 1];

    memcpy(p, payload, length);

    p[length] = NULL;

    String message(p);

    Serial.write(payload, length);

    Serial.println(topic);

}



void reconnect() {

    // Loop until we're reconnected

    while (!client.connected()) {

        Serial.println("Attempting MQTT connection...");



        // Attemp to connect

        if (client.connect(MQTT_CLIENT_NAME, TOKEN, "")) {

            Serial.println("Connected");

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

 * Sensor Functions

 ****************************************/

float readDistance() {

    digitalWrite(trigPin, HIGH);

    delayMicroseconds(10);

    digitalWrite(trigPin, LOW);

    duration = (pulseIn(echoPin, HIGH));

    distance = float(duration / 29 / 2);

    return distance;

}



/****************************************

 * Main Functions

 ****************************************/

void setup() {

    Serial.begin(115200);

    WiFi.begin(WIFISSID, PASSWORD);



    /* Initializing the DHT11 */

    dht.begin();



    /* Assign the PINS as INPUT/OUTPUT */

    pinMode(trigPin, OUTPUT);

    pinMode(echoPin, INPUT);



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

    client.setServer(MQTT_BROKER, 1883);

    client.setCallback(callback);

}



void loop() {

    if (!client.connected()) {

        reconnect();

    }



    /* Reading temperature and humidity */

    float humidity = dht.readHumidity();

    float temperature = dht.readTemperature();



    /* call the funtion readDistance() */

    distance = readDistance();



    /* 4 is mininum width, 2 is precision; float value is copied onto str_sensor*/

    dtostrf(distance, 4, 2, str_sensor);

    dtostrf(humidity, 4, 2, str_HumSensor);

    dtostrf(temperature, 4, 2, str_TempSensor);



    /* Building the Ubidots request */

    sprintf(topic, "%s%s", "/v1.6/devices/", DEVICE_LABEL);

    sprintf(payload, "%s", ""); // Cleans the payload

    sprintf(payload, "{\"%s\": %s,", VARIABLE_LABEL_1, str_sensor); // Adds the variable label

    sprintf(payload, "%s\"%s\": %s,", payload, VARIABLE_LABEL_2, str_HumSensor); // Adds the variable label

    sprintf(payload, "%s\"%s\": %s}", payload, VARIABLE_LABEL_3, str_TempSensor); // Adds the variable label



    //sprintf(payload, "%s {\"value\": %s}}", payload, str_sensor); 



    /* Print the sensor reading to the Serial Monitor */

    Serial.println("Publishing values to Ubidots Cloud");

    Serial.print("Distance = ");

    Serial.println(distance);

    Serial.print("Humidity = ");

    Serial.println(humidity);

    Serial.print("Temperature = ");

    Serial.println(temperature);



    /* Publish the request to Ubidots */

    client.publish(topic, payload);

    client.loop();

    delay(1000);

}
