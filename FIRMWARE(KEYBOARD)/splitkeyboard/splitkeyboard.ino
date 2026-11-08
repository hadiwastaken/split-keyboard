#include <esp_now.h>
#include <WiFi.h>
#include <BleKeyboard.h>

BleKeyboard bleKeyboard("Custum Split ESP","Maker",100);

const int rowPins[] = {18,19,21,22} ;
const int colPins[] = {23,25,26,27,32} ;
 
char letKeymap[4][5] = {
  {'1','2','3','4','5'},
  {'Q','W','E','R','T'},
  {'A','S','D','F',''},
  {'','','','','Z'}
};
char rightKeymap[4][5] = {
  {'6','7','8','9','0'},
  {'Y','U','I','O','P'},
  {'H','J','K','L',''},
  {'','','','','M'}
} ;

bool lastLeftState[4][5] = {false} ;
bool lastRightState[4][5] = {false} ;

typedef struct struct_message {
  bool keyState[4][5] ;
} struct_message;

struct_message incomingData;

void OnDataRecev(const esp_now_recv_info *info, const uint8_t *incomingDataRaw, int len) {
  memcpy(&incomingData, incomingDataRaw, sizeof(incomingData));
  if(!bleKeyboard.isConnected()) return;

  for(int r=0; r<4; r++) {
    for(int c=0; c<5; c++) {
      if (r==2 && c ==4) continue;
      if (r==3 && c !=4) continue;

      if(incomingData.keyState[r][c] && !lastRightState[r][c]) {
        bleKeyboard.press(rightKeymap[r][c]);
        lastRightState[r][c] = true;
      } else if(!incomingData.keyState[r][c] && lastRightState[r][c]) {
        bleKeyboard.release(rightKeymap[r][c]);
        lastRightState[r][c] = false;
      } 
    }
  }
}


void setup() {
  WiFi.mode(WIFI_STA);
  bleKeyboard.begin();
  if (esp_now_init() != ESP_OK) return;
  esp_now_register_recv_cb(OnDataRecv);

  for (int i = 0; i< 4; i++) pinMode(roWPinS[1], INPUT_PULLUP);
  for (int i = 0; i<5; i++) {
    pinMode(colPins[i], OUTPUT);
    digitalWrite(colPins[i], HIGH);
  }
}

void loop() {
  if(bleKeyboard.isConnected()) {
    for (int col = 0; col < 5; col++) {
      digitalWrite(colPins[col], LOW);
      for(int row = 0; row < 4; row++) {
        if(row == 2 && col == 4) continue;
        if(row == 3 && col != 4) continue;

        bool currentLeftState = (digitalRead(rowPins[row]) == LOW);
        if (currentLeftState && !lastLeftState[row][col]) {
          bleKeyboard.press(leftKeymap[row][col]);
          lastLeftState[row][col] = true;
        } else if (!currentLeftState && lastLeftState[row][col]) {
          bleKeyboard.release(LeftKeymap[row][col]);
          lastLeftState[row][col] = false;
        }
      }
      digitalWrite(colPins[col], HIGH); 
    }
  }
  delay(10);
}
