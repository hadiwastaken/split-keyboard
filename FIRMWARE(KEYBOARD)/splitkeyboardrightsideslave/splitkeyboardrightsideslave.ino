#include <esp_now.h>
#include <WiFi.h>

const int rowPins[] = {18,19,21,22};
const int colPins[] = {23,25,26,27,32};

uint8_t leftMacAddress[] = {0xFF,0xFF,0xFF,0xFF,0xFF,0xFF}; 

typedef struct struct_message {
  bool keyState[4][5];
} struct_message;

struct_message myData;
esp_now_peer_info_t peerInfo;


void setup() {
  WiFi.mode(WIFI_STA);
  if (esp_now_init() != ESP_OK) return;

  memcpy(peerInfo.peer_addr, leftMacAddress, 6);
  peerInfo.channel = 0;
  peerInfo.encrypt = false;
  esp_now_add_peer(&peerInfo);

  for(int i=0; i<4; i++) pinMode(rowPins[i], INPUT_PULLUP);
  for(int i=0; i<5; i++){
    pinMode(colPins[i], OUTPUT);
    digitalWrite(colPins[i], HIGH);
  }
}


void loop() {
  bool changed = false;
  for (int col = 0;col < 5; col++) {
    digitalWrite(colPins[col], LOW);
    for (int row = 0;row < 4; row++) {
      if(row == 2 && col == 4) continue;
      if(row == 3 && col != 4) continue;

      bool currentState = (digitalRead(rowPins[row]) == LOW);
      if (myData.keyState[row][col] != currentState) {
        myData.keyState[row][col] = currentState;
        changed = true;
      }
    }
    digitalWrite(colPins[col], HIGH);
  }
  if (changed) {
    esp_now_send(leftMacAddress, (uint8_t *) &myData, sizeof(myData));
  }
  delay(10);
}
