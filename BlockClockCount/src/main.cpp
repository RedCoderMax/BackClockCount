#include <Arduino.h>
#include <Wire.h>
#include <RTClib.h>
#include <WiFi.h>
#include <NTPClient.h>
#include <WiFiUdp.h>

#define SDA_PIN 21
#define SCL_PIN 22

RTC_DS3231 rtc; // Use the DS3231 RTC module

// WiFi settings
const char *ssid = "JioFiber-4G";
const char *password = "YourPassword";  // Replace with the actual password

WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, "pool.ntp.org", 0);

// Offset for Hyderabad, India (Indian Standard Time - IST) in seconds
const long timezoneOffset = 5 * 60 * 60 + 30 * 60;  // UTC+5:30

void printDateTime(const char* label, DateTime time);
void setupESP32AndRTC();

void setup() {
  Serial.begin(115200);

  // Call the setupESP32AndRTC function
  setupESP32AndRTC();
}

void loop() {
  printDateTime("NTP Time: ", timeClient.getEpochTime() + timezoneOffset); // Adjust NTP time with offset
  delay(100);
}

void printDateTime(const char* label, DateTime time) {
  Serial.print(label);
  Serial.print(time.year(), DEC);
  Serial.print('/');
  Serial.print(time.month(), DEC);
  Serial.print('/');
  Serial.print(time.day(), DEC);
  Serial.print(" ");
  Serial.print(time.hour(), DEC);
  Serial.print(':');
  Serial.print(time.minute(), DEC);
  Serial.print(':');
  Serial.println(time.second(), DEC);
}

void setupESP32AndRTC() {
  Wire.begin(SDA_PIN, SCL_PIN);

  // Connect to Wi-Fi
  WiFi.begin(ssid, password);
  Serial.print("Connecting to ");
  Serial.print(ssid);

  int attempts = 0;
  while (WiFi.status() != WL_CONNECTED && attempts < 20) {
    delay(1000);
    Serial.print(".");
    attempts++;
  }

  if (WiFi.status() == WL_CONNECTED) {
    Serial.println("\nWiFi connected!");

    // Initialize the RTC
    if (!rtc.begin()) {
      Serial.println("Couldn't find RTC");
      while (1);
    }

    // Check if the RTC lost power and if so, set the time
    if (rtc.lostPower()) {
      Serial.println("RTC lost power, let's set the time!");
      rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
    }

    // Get time from NTP server
    timeClient.begin();
    timeClient.update();
    DateTime now = timeClient.getEpochTime();

    // Set the RTC with the obtained time
    rtc.adjust(now);

    Serial.println("RTC time set from NTP!");
  } else {
    Serial.println("\nFailed to connect to WiFi!");
  }
}