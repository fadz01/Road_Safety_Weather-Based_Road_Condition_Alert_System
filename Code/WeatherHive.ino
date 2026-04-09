#define BLYNK_TEMPLATE_ID "TMPL65L-cJL1V"  // Replace with your actual template ID
#define BLYNK_TEMPLATE_NAME "WeatherHive"  // Replace with your actual template Name
#define BLYNK_AUTH_TOKEN "xfSZidF4KuA-VXtgHu1K3Vxy1kjh47Ya"

#include <WiFi.h>
#include <BlynkSimpleEsp32.h>
#include <DHT.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>

// Constants for Sensors
#define DHTPIN 4       // Pin for DHT22
#define DHTTYPE DHT22
#define LDRPIN 32      // GPIO32 for LDR Sensor
#define RAINPIN 33     // GPIO33

// DHT22 Sensor
DHT dht(DHIPIN, DHTTYPE);

// LCD
LiquidCrystal_I2C lcd(0x27, 16, 2); // I2C Address for LCD

// Blynk Auth Token 
char auth[] = "xfSZidF4KuA-VXtgHu1K3Vxy1kjh47Ya"; // Replace with your actual Blynk Auth Token

// WiFi Credentials
char ssid[] = "Deeana";        // Replace with your WiFi SSID
char ssid[] = "dwaterlaw";     // Replace with your WiFi Password

// Variables for Sensors
float temperature, humidity;
int lightLevel, rainLevel;
String visibilityAlert = "";
String generalAlert = "";

void connectWiFi() {
  WiFi.begin(ssid, pass);
  Serial.print("Connecting to WiFi");

  int attempts = 0; // Track connection attempts
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.print(".");
    attempts++;

    // Restart ESP32 if it takes too long to connect
    if (attempts > 20) {
      Serial.println("\nFailed to connect to WiFi. Restarting...");
      ESP.restart();
    }
  }

  Serial.println("\nConnected to WiFi successfully!");
  Serial.print("IP Address: ");
  Serial.println(WiFi.localIP());
}

void setup() {
  // Initialize Serial Monitor
  Serial.begin(115200);

  // Initialize DHT Sensor
  dht.begin();

  // Initialize LCD
  lcd.init();
  lcd.backlight();

  // Connect to WiFi
  connectWiFi();

  // Initialize Blynk
  Blynk.begin(auth, ssid, pass);

  // Setup LCD Display
  lcd.setCursor(0, 0);
  lcd.print("WeatherHive Init");
  delay(2000);
}

void loop() {
  // Check WiFi Connection and Reconnect if Disconnected
  if (WiFi.status() != WL_CONNECTED) {
    Serial.println("WiFi lost! Reconnecting...");
    connectWiFi();
  }

// Run Blynk
Blynk.run();

// Read Sensor Data
temperature = dht.readTemperature();
humidity = dht.readHumidity();
lightLevel = analogRead(LDRPIN);
rainLevel = analogRead(RAINPIN);

// Determine Visibility Levels
checkVisibility();

// Determine Rain Conditions
checkRainCondition();

// Display Data on LCD
lcd.clear();
lcd.setCursor(0, 0);
 lcd.print(visibilityAlert);  
//lcd.print("C Hum;");
//lcd.print(humidity);
lcd.setCursor(0, 1);
lcd.print(generalAlert);     // Show general alert message

// Send Data to Blynk App
Blynk.virtualWrite(V0, temperature);
Blynk.virtualWrite(V1, humidity);
Blynk.virtualWrite(V2, lightLevel);
Blynk.virtualWrite(V3, rainLevel);
Blynk.virtualWrite(V4, generalAlert);
Blynk.virtualWrite(V5, visibilityAlert);

// Wait before the next iteration
delay(2000);  // 2-second update interval
}

void checkVisibility() {
    // Determine visibility based on LDR sensor value
    if (lightLevel < 1500) {
        visibilityAlert = "Bright: Safe";           // Bright light = good visibility
    } else if (lightLevel < 3000) {
        visibilityAlert = "Low Visibility: Caution"; // Reduced visibility
    } else {
        visibilityAlert = "Dark: Use Lights";       // Very low visibility, use headlights
    }

    // Print to Serial Monitor for debugging
    Serial.print("LDR Value: ");
    Serial.print(lightLevel);
    Serial.print(", Visibility: ");
    Serial.println(visibilityAlert);
}

void checkRainCondition() {
    // Determine rain intensity based on sensor value
    if (rainLevel >= 4095) {
        generalAlert = "No Rain";                 // 4095 = No Rain
    } else if (rainLevel >= 3095) {
        generalAlert = "Light Rain Detected";     // 3095 = Light Rain
    } else {
        generalAlert = "Heavy Rain Detected";     // Below 3095 = Heavy Rain
    }

    // Print to Serial Monitor for debugging
    Serial.print("Rain Level: ");
    Serial.print(rainLevel);
    Serial.print(", General Alert: ");
    Serial.println(generalAlert);
}



