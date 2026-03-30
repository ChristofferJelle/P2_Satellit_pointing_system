
void InitESP32_NOW() {
  WiFi.mode(WIFI_STA);

  //initatite ESP_now for comunication without router.
  if (esp_now_init() != ESP_OK) {
    Serial.println("Error initializing ESP-NOW");
    return;
  }
}

//dont really need to be a funktion since its one line if code but its here ig in the case we want to uncomment the error handler part.
void SendDataToMaster() {
  // Send message via ESP-NOW
  esp_err_t result = esp_now_send(broadcastAddress, (uint8_t*)&setpoint, sizeof(setpoint));

  /*
  //this was "too loud" in the serial moniter so i commented it
  if (result == ESP_OK) {
    Serial.print("Sent with success to ");
    AddressOfPeer(broadcastAddress);
    Serial.println();
  } else {
    Serial.print("Error sending the data to ");

    AddressOfPeer(broadcastAddress);
    Serial.println();
  }
  */
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

void AddressOfPeer(uint8_t peerInfoMAC[]) {
  for (int i = 0; i < 6; i++) {
    if (peerInfoMAC[i] < 16) { Serial.print("0"); }  // zero pad
    Serial.print(peerInfoMAC[i], HEX);
    if (i < 5) { Serial.print(":"); }
  }
}