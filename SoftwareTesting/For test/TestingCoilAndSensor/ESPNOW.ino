//initialise lillygo screen
void InitDisplay() {
  tft.init();
  tft.setRotation(1);  // landscape mode
  tft.fillScreen(TFT_BLACK);
  tft.setTextColor(TFT_WHITE);
  tft.setTextSize(2);  // scale text
}

//draws the screen using library funktions based on the VT100 standard thing
void DrawDisplay() {
  // Erase previous numbers by drawing a filled rectangle in background color
  tft.fillRect(spX, spY, spW, spH, TFT_BLACK);

  tft.setCursor(0, 0);
  tft.print("Setpoint: ");
  tft.print(setpoint, 1);  // 1 decimal place

  tft.fillRect(akX, akY, akW, akH, TFT_BLACK);
  tft.setCursor(0, 30);
  tft.print("Aktuel vinkel: ");
  if (kalAngleZ < 0) {
    tft.print(kalAngleZ, 1);  // 1 decimal place
  } else {
    tft.print("+");
    tft.print(kalAngleZ, 1);  // 1 decimal place
  }
  tft.fillRect(PIDX, PIDY, PIDW, PIDH, TFT_BLACK);
  tft.setCursor(0, 60);
  tft.print("PID output: ");
  tft.print((int)Output, 1);  // 1 decimal place
}

//initialise everything that has to do with setting up ESPNOW library
void InitESP32_NOW() {
  //initialise wifi mode
  WiFi.mode(WIFI_STA);

  //initatite ESP_now with library's own funktion for comunication without router.
  if (esp_now_init() != ESP_OK) {
    Serial.println("Error initializing ESP-NOW");
  }
    //Register that i want to recive to cb (call back). Basically says "everytime i send data u just need to run this too"
    esp_now_register_recv_cb(esp_now_recv_cb_t(OnDataRecv));
}

// Callback when data is received
void OnDataRecv(const uint8_t* mac, const uint8_t* incomingData, int len) {
  memcpy(&setpoint, incomingData, sizeof(setpoint));
  Serial.println(setpoint);
}

//registers peers with the data it needs to send data
void registerPeers() {

  // Register peer
  for (int i = 0; i < 6; i++) {
    peerInfo.peer_addr[i] = broadcastAddress[i];
  }
  peerInfo.channel = 0;
  peerInfo.encrypt = false;

  Serial.println("Peer MAC:");
  AddressOfPeer(broadcastAddress);

  if (esp_now_add_peer(&peerInfo) == ESP_OK) {
    Serial.println("Peer added successfully");
  } else {
    Serial.print("Failed to add peer Error code: ");
    Serial.println(esp_now_add_peer(&peerInfo));
  }
}

//dont really need to be a funktion since its one line if code but its here ig in the case we want to uncomment the error handler part.
void SendDataToMaster(double sendingAngle) {
  // Send message via ESP-NOW
  //Serial.println(sendingAngle);
  esp_err_t result = esp_now_send(broadcastAddress, (uint8_t*)&sendingAngle, sizeof(sendingAngle));
  // Serial.println(result);
  //this funktion just uses a funktion to get the mac address and then returns it....and checks if it actually got it
}

uint8_t* readMacAddress() {
  static uint8_t baseMac[6];
  esp_err_t ret = esp_wifi_get_mac(WIFI_IF_STA, baseMac);
  if (ret == ESP_OK) {
    return baseMac;
  } else {
    Serial.println("Failed to read MAC address");
    return nullptr;
  }
}

void AddressOfPeer(uint8_t peerInfoMAC[]) {
  for (int i = 0; i < 6; i++) {
    if (peerInfoMAC[i] < 16) { Serial.print("0"); }  // zero pad
    Serial.print(peerInfoMAC[i], HEX);
    if (i < 5) { Serial.print(":"); }
  }
}