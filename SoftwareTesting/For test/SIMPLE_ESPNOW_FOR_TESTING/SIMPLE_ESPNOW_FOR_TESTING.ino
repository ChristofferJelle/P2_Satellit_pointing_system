#include <esp_now.h>
#include <WiFi.h>
#include <esp_wifi.h>
#include <string.h>

//adress of the mainESP32 in sending data to
uint8_t broadcastAddress[] = { 0xf0, 0x9e, 0x9e, 0x1f, 0x0a, 0xa0 };

esp_now_peer_info_t peerInfo;

bool ConnectedToMaster = false;
double setpoint, kalAngleZ;

uint32_t timer;
int sampleTime = 60000;  // målt til 2289, worked ish at 5000
int lastTime = timer - sampleTime;
int sampleTime2 = 100;  // målt til 2289, worked ish at 5000
int lastTimeSample = timer - sampleTime2;
unsigned long now;

bool GONOW = false;
void setup() {
  Serial.begin(115200);

  InitESP32_NOW();

  registerPeers();
  //outgoingStruct.id = WiFi.macAddress();
  now = millis();
  timer = millis();
}

void loop() {
  if (Serial.available() > 0) {
    // Read the incoming data as an integer

    setpoint = Serial.parseInt();
    SendDataToMaster(setpoint);
  }
  now = millis();
  unsigned long timeChange = (now - lastTime);
  unsigned long timeChange2 = (now - lastTimeSample);
  if (timeChange2 >= sampleTime2 && (timeChange >= sampleTime) && GONOW) {
    Serial.println(kalAngleZ);
    GONOW = false;
    lastTimeSample = now;
  }
}