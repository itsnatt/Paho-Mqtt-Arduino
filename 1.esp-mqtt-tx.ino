#include <Wire.h>
#include <Adafruit_MPU6050.h>
#include <Adafruit_Sensor.h>
#include <WiFi.h>
#include <PubSubClient.h>

const char *ssid = "Wokwi-GUEST";
const char *password = "";
const char *mqtt_server = "0.tcp.ap.ngrok.io";
const int mqtt_port = 18016;  // Sesuaikan dengan port broker MQTT Anda

const int soil1Pin = 34;
const int soil2Pin = 35;
const int rainPin = 15;
const int batteryPin = 2;

Adafruit_MPU6050 mpu;

WiFiClient espClient;
PubSubClient client(espClient);


void setup_wifi() {
  delay(10);
  Serial.println("Connecting to WiFi");
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting...");
  }
  Serial.println("Connected to WiFi");
}

void reconnect() {
  while (!client.connected()) {
    Serial.println("Attempting MQTT connection...");
    if (client.connect("ESP32Client")) {
      Serial.println("Connected to MQTT server");
    } else {
      Serial.print("Failed to connect, rc=");
      Serial.print(client.state());
      Serial.println(" Retrying in 5 seconds...");
      delay(5000);
    }
  }
}

void setup(void) {
  Serial.begin(115200);

  while (!mpu.begin()) {
    Serial.println("MPU6050 not connected!");
    delay(1000);
  }
  Serial.println("MPU6050 ready!");

  setup_wifi();
  client.setServer(mqtt_server, mqtt_port);
}

void loop() {
  if (!client.connected()) {
    reconnect();
  }
  client.loop();
    sensors_event_t a, g, temp;
  mpu.getEvent(&a, &g, &temp);

  int soil1Value = map(analogRead(soil1Pin), 0, 4095, 1, 100);
  int soil2Value = map(analogRead(soil2Pin), 0, 4095, 1, 100);
  boolean rainValue = digitalRead(rainPin);
  boolean batteryValue = digitalRead(batteryPin);
  int x = int(a.acceleration.x);
  int y = int(a.acceleration.y);
  int z = int(a.acceleration.z);
  int node_id = 3844;

  // Kirim data ke MQTT server dengan menyertakan ID node pada topik umum
  client.publish("/sensor/node_id", String(node_id).c_str());
  client.publish("/sensor/soil1", String(soil1Value).c_str());
  client.publish("/sensor/soil2", String(soil2Value).c_str());
  client.publish("/sensor/rain", String(rainValue).c_str());
  client.publish("/sensor/battery", String(batteryValue).c_str());
  client.publish("/sensor/acceleration_x", String(x).c_str());
  client.publish("/sensor/acceleration_y", String(y).c_str());
  client.publish("/sensor/acceleration_z", String(z).c_str());

    Serial.print("Soil Moisture 1: ");
  Serial.println(soil1Value);
  Serial.print("Soil Moisture 2: ");
  Serial.println(soil2Value);
  Serial.print("Rain Status: ");
  Serial.println(rainValue);
  Serial.print("Battery Status: ");
  Serial.println(batteryValue);
  Serial.print("Acceleration: X=");
  Serial.print(x);
  Serial.print(" m/s^2, Y=");
  Serial.print(y);
  Serial.print(" m/s^2, Z=");
  Serial.print(z);
  Serial.println(" m/s^2");

  delay(5000); // Delay 5 detik
}
