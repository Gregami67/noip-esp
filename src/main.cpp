#include <Arduino.h>
#include <WiFi.h>

/*

The purpose of this project is to create a noip configuration tool for esp32
This tool will allow the user to configure the esp32 to connect to a wifi network and update the noip dns record
It will use both cores to process the wifi and blink operations

*/

TaskHandle_t Task1;
TaskHandle_t Task2;

#define LEDPIN 2

RTC_DATA_ATTR bool isAPMode = false;

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
    digitalWrite(LEDPIN, HIGH);
    delay(delayTime);
    digitalWrite(LEDPIN, LOW);
    delay(delayTime);
  }
  return value;
}

void Task1Potato(void *pvParameters){
  while (1) {
    Serial.println(".");
    if (!isConfigured(isWifiConfigured, 1000)) continue;
    if (!isConfigured(isNoipConfigured, 250)) continue;
    vTaskDelay(5000 / portTICK_PERIOD_MS);
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
  //TODO: Update to single line if statement
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
  isAPMode = !isAPMode;
  if (isAPMode){
    Serial.println("AP Mode");
    setup_ap();
  } else {
    Serial.println("Station Mode");
    setup_station();
  }
}

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  pinMode(LEDPIN, OUTPUT);
  xTaskCreatePinnedToCore(
    Task1Potato,
    "Led Feedback",
    10000,
    NULL,
    1,
    &Task1,
    0
  );
  // xTaskCreatePinnedToCore(
  //   server_stuff,
  //   "Server Stuff",
  //   4096,
  //   NULL,
  //   0,
  //   &Task2,
  //   1
  // );
}

void loop() {

}