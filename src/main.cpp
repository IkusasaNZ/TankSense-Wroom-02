#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <WiFiManager.h> 
#include <ESP8266HTTPClient.h>
#include <WiFiClientSecure.h>

#define TRIG_PIN 4  // GPIO4
#define ECHO_PIN 5  // GPIO5

long duration;
float distance;

const char* ssid = "George";
const char* password = "QUINTINLIDDLE";

const String webhookUrl = "https://gzdwafxrglwhmpyprvmq.supabase.co/functions/v1/webhook-receiver";
const String authKey = "Bearer eyJhbGciOiJIUzI1NiIsInR5cCI6IkpXVCJ9.eyJpc3MiOiJzdXBhYmFzZSIsInJlZiI6Imd6ZHdhZnhyZ2x3aG1weXBydm1xIiwicm9sZSI6ImFub24iLCJpYXQiOjE3NTM5NDQxNzIsImV4cCI6MjA2OTUyMDE3Mn0.lacdnoIL8MRFtKBy-x7dxO2EDHHOooeogoAcgDisScw";  // Use "Bearer ..." or just the token depending on your API
const String deviceSerial = "ABC123";  // Change to your device's serial number

void initWiFi() {
  WiFi.begin(ssid, password);
  Serial.print("Connecting to WiFi");

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("\nConnected to WiFi");
} 

void sendTankSenseData() {

  if (WiFi.status() == WL_CONNECTED) {
    float randomValue = random(0, 22) / 10.0;  // 0.0 to 2.1 (21 inclusive) with 1 decimal

    Serial.println("-----");
    Serial.print("Generated random value: ");
    Serial.println(randomValue, 1);
    Serial.print("Sending to webhook: ");
    Serial.println(webhookUrl);

    WiFiClientSecure client;
    client.setInsecure();  // ⚠️ Disables certificate validation (easier for testing)

    HTTPClient http;
    WiFiClient wifiClient;
    http.begin(client, webhookUrl);
    http.addHeader("Content-Type", "application/json");
    http.addHeader("Authorization", authKey);

    // String payload = "{\"value\": " + String(randomValue, 1) + "}";
    String payload = "{\"Serial\": \"" + deviceSerial + "\", \"Distance\": " + String(randomValue, 1) + "}";

    int httpCode = http.POST(payload);
    String response = http.getString();

    Serial.print("HTTP Status: ");
    Serial.println(httpCode);
    Serial.print("Response: ");
    Serial.println(response);

    http.end();
  } else {
    Serial.println("WiFi disconnected. Reconnecting...");
    WiFi.begin(ssid, password);
  }

} 

void initwifiManager() {
  // Initialize WiFiManager
  WiFiManager wifiManager;
  // Reset settings - for testing
  // wifiManager.resetSettings();
  
  // Set custom parameters
  wifiManager.setTimeout(120); // Set timeout to 120 seconds
  
  // Start the WiFi manager
  if (!wifiManager.autoConnect("TankSenseAP")) {
    Serial.println("Failed to connect and hit timeout");
    delay(3000);
    ESP.restart(); // Restart if connection fails
  }
  Serial.println("Connected to WiFi!");
}


// Subroutine to read and print distance
void readDistance() {

  digitalWrite(4, LOW);
  delayMicroseconds(2);
  digitalWrite(4, HIGH);
  delayMicroseconds(10);
  digitalWrite(4, LOW);

  long duration = pulseIn(5, HIGH, 30000);
  float distance = duration * 0.0343 / 2;

  if (duration == 0) {
    Serial.println("No echo (timeout)");
  } else {
    Serial.print("Distance: ");
    Serial.print(distance);
    Serial.println(" cm");
  }
}
// Subroutine to initialize ultrasonic sensor pins
void initUltrasonic() {
  pinMode(4, OUTPUT); // TRIG
  pinMode(5, INPUT);  // ECHO
}


void setup() {
  Serial.begin(115200);
  delay(1000);

  // initwifiManager();
  // initUltrasonic();
  initWiFi();

}


void loop() {

  sendTankSenseData(); // Send data to webhook
  // readDistance(); // Read and print distance
  delay(30000); // Wait 5 minutes between measurements
  
}


