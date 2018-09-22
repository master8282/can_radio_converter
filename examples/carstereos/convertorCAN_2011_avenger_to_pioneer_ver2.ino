#include <SPI.h>
#include <mcp2515.h>

struct can_frame x190;
struct can_frame x416;
struct can_frame canMsg;

MCP2515 mcp2515(10);

unsigned long inter416, inter190, wait, ebrake;
bool volup = false;
bool voldown = false;
bool next = false;
bool prev = false;
bool menu = false;
bool bring = false;
  
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

  x190.can_id  = 0x190; //emulation of the response of original 
  x190.can_dlc = 7;     //stereo for instrumental cluster.
  x190.data[0] = 0x1D;
  x190.data[1] = 0x0D;
  x190.data[2] = 0x35;
  x190.data[3] = 0x31;
  x190.data[4] = 0x00;
  x190.data[5] = 0x00;
  x190.data[6] = 0x07;

  x416.can_id  = 0x416; //emulation of the response of original stereo
  x416.can_dlc = 8;     // for another device like airbags, BCM etc.
  x416.data[0] = 0xFD;  // without these responces many DTC about lost
  x416.data[1] = 0x1C;  // communication with the stereo. 
  x416.data[2] = 0x3F;
  x416.data[3] = 0xFF;
  x416.data[4] = 0xFF;
  x416.data[5] = 0xFF;
  x416.data[6] = 0xFF;
  x416.data[7] = 0xFF;
  
//  while (!Serial);      // Left if it needs debugging
//  Serial.begin(115200);
  SPI.begin();
  
  mcp2515.reset();
  mcp2515.setBitrate(CAN_83K3BPS);
  mcp2515.setNormalMode();
  
}

void purge() {
  volup = false;
  voldown = false;
  next = false;
  prev = false;
  menu = false;
  bring = false;
  digitalWrite(A0, HIGH);
  digitalWrite(A2, HIGH);
  digitalWrite(A1, HIGH);
  digitalWrite(A5, HIGH);
  digitalWrite(A4, HIGH);
  digitalWrite(6, HIGH);
  digitalWrite(7, HIGH);
}

void loop() {
  unsigned long current_time = millis();

  if (mcp2515.readMessage(&canMsg) == MCP2515::ERROR_OK) {
 
    if (canMsg.can_id == 928) {
      ebrake = current_time;

      if (canMsg.data[0] > 0) {
        wait = 45 + current_time; // 45 optimal sticking time for buttons
        switch (canMsg.data[0]) { // for pioneer aftamarket stereo.
        case 2:
          volup = true;
          break;
        case 4:
          voldown = true;
          break;
        case 8:
          next = true;
          break;
        case 16:
          prev = true;
          break;
        }
      }

      if (canMsg.data[1] > 0) {
        wait = 150 + current_time; //exstended time for "call" and "menu".
        switch (canMsg.data[1]) {
        case 1:
          menu = true;
          break;
        case 2:
          bring = true;
          break;
        }
      }

      if (canMsg.data[0] == 0 && canMsg.data[1] == 0 && current_time >= wait) {        
        purge();  
      }
    }
  }

  if (ebrake + 1000 < current_time) { // if connection with CAN-B gets lost more than 1 sec.
    purge(); 
 
  }

  if (volup == true) {
    digitalWrite(A5, LOW);
  }

  if (voldown == true) {
    digitalWrite(A4, LOW);
  }

  if (next == true) {
    digitalWrite(6, LOW);
  }

  if (prev == true) {
    digitalWrite(7, LOW);
  }

  if (menu == true) {
    digitalWrite(A0, LOW);
    digitalWrite(A2, LOW);
  }

  if (bring == true) {
    digitalWrite(A0, LOW);
    digitalWrite(A1, LOW);
  }

  if (current_time - inter416 > 450) {   
  mcp2515.sendMessage(&x416);
  inter416 = current_time;  
  }

  if (current_time - inter190 > 450) {   
  mcp2515.sendMessage(&x190);
  inter190 = current_time;   
  } 
}
