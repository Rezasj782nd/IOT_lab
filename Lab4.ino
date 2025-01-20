//Github: https://github.com/Rezasj782nd
//email: sahraeianrza@gmail.com


#include <DHT.h>
#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <simpleRPC.h>
#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <WiFiClient.h>



// Replace with your network credentials
const char* ssid = "SJ78";
const char* password = "sahraeian7881";

// ThingsBoard MQTT Broker information
const char* mqtt_server = "192.168.0.101";             // For ThingsBoard Cloud
const char* mqtt_user = "0vcigsmvxx4r8j0ct408";      // Device access token
const char* mqtt_topic = "v1/devices/me/telemetry";  // Default telemetry topic

//api
String serverName = "https://192.168.1.4/api/v1/0vcigsmvxx4r8j0ct408/rpc";




// DHT Sensor settings
uint8_t DHTPin = D14;
#define DHTTYPE DHT11      // Define the DHT sensor type
DHT dht(DHTPin, DHTTYPE);  // Initialize DHT sensor

// LED Pin settings
#define GREEN_LED_PIN 14   // GPIO5 (D5)
#define YELLOW_LED_PIN 12  // GPIO4 (D6)
#define RED_LED_PIN 13     // GPIO0 (D7)

WiFiClient espClient;
PubSubClient client(espClient);
HTTPClient http;


void setup() {
  Serial.begin(115200);
  delay(10);
  //api test
  http.begin(espClient, serverName.c_str());


  // Connect to Wi-Fi
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi...");
  }
  Serial.println("Connected to WiFi");

  // Initialize MQTT client
  client.setServer(mqtt_server, 1883);
  client.setCallback(on_res);

  // Initialize DHT
  delay(100);
  pinMode(DHTPin, INPUT);
  dht.begin();
  delay(100);

  // Initialize LED pins
  pinMode(GREEN_LED_PIN, OUTPUT);
  pinMode(YELLOW_LED_PIN, OUTPUT);
  pinMode(RED_LED_PIN, OUTPUT);
}

void loop() {
  if (!client.connected()) {
    reconnect();
  }
  client.loop();


  // api

 

  // Read temperature and humidity from DHT22
  float temperature =  dht.readTemperature();  // Celsius
  float humidity =  dht.readTemperature();


    if (isnan(temperature) || isnan(humidity)) {
    Serial.println("Failed to read from DHT sensor!");
  }
  else {
    // Prepare the telemetry data
    String payload = "{\"temperature\": " + String(temperature) + ", \"humidity\": " + String(humidity) + "}";

    // Publish the data to ThingsBoard
    client.publish(mqtt_topic, payload.c_str());

    String payload2 =  "{\"method\": \"getcolor\" , \"params\" : { \"temp\":" + String(temperature) +"}}";
    client.publish("v1/devices/me/rpc/request/1", payload2.c_str());

    // Print to Serial Monitor
    Serial.print("Temperature: ");
    Serial.print(temperature);
    Serial.print(" °C  ");
    Serial.print("Humidity: ");
    Serial.print(humidity);
    Serial.println(" %");

    // Control LEDs based on temperature threshold
  }
  



  delay(5000);  // Send data every 5 seconds
}

// Reconnect to ThingsBoard MQTT server if the connection is lost
void reconnect() {
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    if (client.connect("ArduinoClient", mqtt_user, NULL)) {
      Serial.println("connected");
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      delay(5000);
    }
  }
}

// Control LEDs based on temperature threshold


void on_res(const char* topic, unsigned char* payload, unsigned int length) {
 int i;
  char json[length +1];
  strncpy (json, (char*)payload, length);
  json[length] = '\0';
  String data = String(json);
  sscanf(data.c_str(), "%d", &i);
  // StaticJsonBuffer<200> jsonBuffer;
  // JsonObject& data = JsonBuffer.parseObject((char*)json);


  controlLEDs(i);
}

void controlLEDs(int color) {
  // Define temperature thresholds (for example)
  const float greenThreshold = 25.0;   // Temperature below this will turn Green LED
  const float yellowThreshold = 35.0;  // Temperature between green and yellow will turn Yellow LED
  const float redThreshold = 35.0;     // Temperature above this will turn Red LED

  // Turn off all LEDs initially
  digitalWrite(GREEN_LED_PIN, LOW);
  digitalWrite(YELLOW_LED_PIN, LOW);
  digitalWrite(RED_LED_PIN, LOW);

  // Control LEDs based on temperature
  if (color == 1) {
    digitalWrite(GREEN_LED_PIN, HIGH);  // Green LED ON
  }
  if (color == 2) {
    digitalWrite(GREEN_LED_PIN, HIGH);
    digitalWrite(YELLOW_LED_PIN, HIGH);  // Yellow LED ON
  }
  if (color == 3) {
    digitalWrite(GREEN_LED_PIN, HIGH);
    digitalWrite(YELLOW_LED_PIN, HIGH);
    digitalWrite(RED_LED_PIN, HIGH);  // Red LED ON
  }
}







// void controlLEDs(float temperature) {
//   // Define temperature thresholds (for example)
//   const float greenThreshold = 25.0;   // Temperature below this will turn Green LED
//   const float yellowThreshold = 35.0;  // Temperature between green and yellow will turn Yellow LED
//   const float redThreshold = 35.0;     // Temperature above this will turn Red LED

//   // Turn off all LEDs initially
//   digitalWrite(GREEN_LED_PIN, LOW);
//   digitalWrite(YELLOW_LED_PIN, LOW);
//   digitalWrite(RED_LED_PIN, LOW);

//   // Control LEDs based on temperature
//   if (temperature < greenThreshold) {
//     digitalWrite(GREEN_LED_PIN, HIGH);  // Green LED ON
//   }
//   if (temperature >= greenThreshold && temperature < redThreshold) {
//     digitalWrite(GREEN_LED_PIN, HIGH);
//     digitalWrite(YELLOW_LED_PIN, HIGH);  // Yellow LED ON
//   }
//   if (temperature >= redThreshold) {
//     digitalWrite(GREEN_LED_PIN, HIGH);
//     digitalWrite(YELLOW_LED_PIN, HIGH);
//     digitalWrite(RED_LED_PIN, HIGH);  // Red LED ON
//   }
// }


