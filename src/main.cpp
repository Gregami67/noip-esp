#include <Arduino.h>
#include <WiFi.h>

/*

The purpose of this project is to create a noip configuration tool for esp32
This tool will allow the user to configure the esp32 to connect to a wifi network and update the noip dns record
It will use both cores to process the wifi and blink operations

*/

TaskHandle_t Task1;
TaskHandle_t Task2;

const uint8_t ledPin = LED_BUILTIN;

RTC_DATA_ATTR bool isAPMode = true;

bool volatile isWifiConfigured = false;
bool volatile isNoipConfigured = false;

const char* default_ap_ssid = "ssid";
const char* default_ap_password = "password";

const char* default_ssid = "ssid";
const char* default_password = "password";

RTC_DATA_ATTR char ap_ssid[32] = "";
RTC_DATA_ATTR char ap_password[64] = "";

RTC_DATA_ATTR char ssid[32] = "";
RTC_DATA_ATTR char password[64] = "";

// RTC_DATA_ATTR char noip_user[32] = "";
// RTC_DATA_ATTR char noip_password[32] = "";
// RTC_DATA_ATTR char noip_host[32] = "";


bool isConfigured(bool value, uint16_t delayTime){
  if (!value) {
    digitalWrite(ledPin, HIGH);
    delay(delayTime);
    digitalWrite(ledPin, LOW);
    delay(delayTime);
  }
  return value;
}

void led_feedback(void *pvParameters){
  for(;;){
    if (!isConfigured(isWifiConfigured, 1000)) continue;
    if (!isConfigured(isNoipConfigured, 250)) continue;
    delay(5000);
  }
}

//TODO: Add only one connection to the wifi network
void setup_ap(){
  if (strlen(ap_ssid) > 0 && strlen(ap_password) > 0){
    WiFi.softAP(ap_ssid, ap_password);
  } else {
    WiFi.softAP(default_ap_ssid, default_ap_password);
  }
  vTaskDelay(1000 / portTICK_PERIOD_MS);
  if (WiFi.softAPIP() != IPAddress(0, 0, 0, 0)){
    Serial.println("AP IP Address: " + WiFi.softAPIP().toString());
    isWifiConfigured = true;
  } else {
    isWifiConfigured = false;
  }
}

void setup_station(){
  WiFi.begin(default_ssid, default_password);
  isAPMode = false;
}

void server_stuff(void *pvParameters){
  if (isAPMode){
    setup_ap();
    Serial.println("AP Mode");
  } else {
    setup_station();
    Serial.println("Station Mode");
  }
}

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  xTaskCreatePinnedToCore(
    led_feedback,
    "Led Feedback",
    1024,
    NULL,
    0,
    &Task1,
    0
  );
  xTaskCreatePinnedToCore(
    led_feedback,
    "Server Stuff",
    4096,
    NULL,
    0,
    &Task2,
    1
  );
}

void loop() {

}