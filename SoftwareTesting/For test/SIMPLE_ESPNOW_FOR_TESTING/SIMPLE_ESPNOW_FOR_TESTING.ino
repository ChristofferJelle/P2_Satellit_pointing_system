#include <esp_now.h>
#include <WiFi.h>
#include <esp_wifi.h>
#include <string.h>

//adress of the mainESP32 in sending data to
uint8_t broadcastAddress[] = { 0xf0, 0x9e, 0x9e, 0x1f, 0x0a, 0xa0 };

esp_now_peer_info_t peerInfo;

bool ConnectedToMaster = false;
double setpoint;

struct incomingStuct {
  double kalAngleZ;
  double OutputPID;
};
incomingStuct incomingStuct1;

uint32_t timer;
int sampleTime = 10;
int lastTime = timer - sampleTime;
unsigned long now;
void setup() {
  Serial.begin(115200);

  InitESP32_NOW();

  registerPeers();
  //outgoingStruct.id = WiFi.macAddress();
  now = millis();
  timer = millis();
}

//if its not connected to the master/main esp32 then keep trying to ping with it's outgoing struct, which includes its own mac address as an id so it can be added as a peer, aka be connected
//it gets conncted when the master returns the 'R' command, which sets ConnectedToMaster to false in the OnDataRecv() callback funktion
void loop() {
  if (Serial.available() > 0) {
    // Read the incoming data as an integer

    setpoint = Serial.parseInt();
    SendDataToMaster(setpoint);
  }
  now = millis();
  unsigned long timeChange = (now - lastTime);
  if (timeChange >= sampleTime) {
    Serial.print("Angle: ");
    Serial.print(incomingStuct1.kalAngleZ);
    Serial.print("\t");
        Serial.print("PID: ");
    Serial.print(incomingStuct1.OutputPID);
    Serial.print("\t");
    Serial.println();
    incomingStuct1.kalAngleZ = 0;
    incomingStuct1.OutputPID = 0;
    lastTime = now;
  }
}