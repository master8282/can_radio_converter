#include <SPI.h>
#include <mcp2515.h>

struct can_frame canMsg;
MCP2515 mcp2515(10);

void setup() {
  pinMode(A0, OUTPUT); // ground
  pinMode(A5, OUTPUT); // volume up
  pinMode(A4, OUTPUT); // volume down
  pinMode(6, OUTPUT);  // next
  pinMode(7, OUTPUT);  // prev
  pinMode(A2, OUTPUT); // menu
  pinMode(A1, OUTPUT); // bring up
  digitalWrite(A0, HIGH);
  digitalWrite(A5, HIGH);
  digitalWrite(A4, HIGH);
  digitalWrite(A2, HIGH);
  digitalWrite(A1, HIGH);
  digitalWrite(6, HIGH);
  digitalWrite(7, HIGH);

  // Serial.begin(115200);

  SPI.begin();
  mcp2515.reset();
  mcp2515.setBitrate(CAN_83K3BPS); // 83,3k for old chrysler internal CAN-B
  mcp2515.setNormalMode();

  // Serial.println("Initialization done");
}

void loop() {
  if (mcp2515.readMessage(&canMsg) == MCP2515::ERROR_OK) {
    //  Serial.println(canMsg.can_id);
    if (canMsg.can_id == 928) {
      if (canMsg.data[0] > 0) {
        switch (canMsg.data[0]) {
        case 2:
          //      Serial.println("Volume Up");
          digitalWrite(A5, LOW);
          break;
        case 4:
          //      Serial.println("Volume Down");
          digitalWrite(A4, LOW);
          break;
        case 8:
          //      Serial.println("Next");
          digitalWrite(6, LOW);
          break;
        case 16:
          //      Serial.println("Prev");
          digitalWrite(7, LOW);
          break;
        }
      }

      if (canMsg.data[1] > 0) {
        switch (canMsg.data[1]) {
        case 1:
          //      Serial.println("Menu");
          digitalWrite(A0, LOW);
          digitalWrite(A2, LOW);
          delay(100); // special delay to preven false workouts
          break;
        case 2:
          //      Serial.println("Bring Up");
          digitalWrite(A0, LOW);
          digitalWrite(A1, LOW);
          delay(100); // special delay to preven false workouts
          break;
        }
      }

      if (canMsg.data[0] == 0 && canMsg.data[0] == 0) {
        delay(45); // 40 is minimum delay for pioneer else doesn't feel
                   // clicking.
        digitalWrite(A0, HIGH);
        digitalWrite(A2, HIGH);
        digitalWrite(A1, HIGH);
        digitalWrite(A5, HIGH);
        digitalWrite(A4, HIGH);
        digitalWrite(6, HIGH);
        digitalWrite(7, HIGH);
      }
    }
  }
}
