/* DCS F-16 CHAFF / FLARE DISPENSER
   DCS-BIOS FORK VERSION ! don't use HUB VERSION!
   HARDWARE: 2 X HDSP2533 LED DISPLAY CONNECTED TO 2 X 74HC595 SHIFT REGISTERS (saves Pins on Arduino Mega)
   1 ARDUINO MEGA (don't ask me for a pinout diagram as i don't have it anymore, Panel is closed and sealed with Hotglue)
   https://www.youtube.com/watch?v=ULrL2KjXRA8
*/

#define DCSBIOS_DEFAULT_SERIAL
#include "DcsBios.h"

//HDSP Display2 for chaff and flares
const int RST   = A0; // HDSP-2533 reset
const int WR    = A1; // HDSP-2533 write
const int CE    = 10; // HDSP-2533 chip enable
const int AD0   = A4; // HDSP-2533 adress bit 0
const int AD1   = A3; // HDSP-2533 adrres bit 1
const int AD2   = A2; // HDSP-2533 adress bit 2

//HDSP Display1 for amount01 and amount02
const int RST1   = A5; // HDSP-2533 reset
const int WR1    = A6; // HDSP-2533 write
const int CE1    = 11; // HDSP-2533 chip enable
const int AD01   = A7; // HDSP-2533 adress bit 0
const int AD11   = A8; // HDSP-2533 adress bit 1
const int AD21   = A9; // HDSP-2533 adress bit 2

//  Shift Register2 setup chaff and flares
const int DS    = 2; //shiftregister datapin  (data)
const int ST_CP = 3; //shiftregister storageclock(latch)
const int SH_CP = 4; //shiftregister shiftclock(data polling)

//  Shift Register1 setup amount01 and amount02
const int DS1    = 5; //shiftregister datapin  (data)
const int ST_CP1 = 6; //shiftregister storageclock(latch)
const int SH_CP1 = 7; //shiftregister shiftclock(data polling)

char* flare = "    ";
char* chaff = "    ";
char* am01 = "    ";
char* am02 = "    ";

// DCS-Bios Output for Chaff/Flares
void onCmdsChAmountChange(char* newValue) {
  chaff = newValue;
}
DcsBios::StringBuffer<4> cmdsChAmountBuffer(0x45a8, onCmdsChAmountChange);

void onCmdsFlAmountChange(char* newValue) {
  flare = newValue;
}
DcsBios::StringBuffer<4> cmdsFlAmountBuffer(0x45ac, onCmdsFlAmountChange);

void onCmdsO1AmountChange(char* newValue) {
  am01 = newValue;
}
DcsBios::StringBuffer<4> cmdsO1AmountBuffer(0x45a0, onCmdsO1AmountChange);

void onCmdsO2AmountChange(char* newValue) {
  am02 = newValue;
}
DcsBios::StringBuffer<4> cmdsO2AmountBuffer(0x45a4, onCmdsO2AmountChange);

// Rotary Encoders and Switches
DcsBios::Switch2Pos cmdsPwrSourcheSw("CMDS_PWR_SOURCHE_SW", 30);
DcsBios::Switch2Pos cmdsJmrSourcheSw("CMDS_JMR_SOURCHE_SW", 31);
DcsBios::Switch2Pos cmdsMwsSourcheSw("CMDS_MWS_SOURCHE_SW", 32);
DcsBios::Switch2Pos cmdsJettSw("CMDS_JETT_SW", 33);
DcsBios::Switch2Pos cmds01ExpCatSw("CMDS_01_EXP_CAT_SW", 34);
DcsBios::Switch2Pos cmds02ExpCatSw("CMDS_02_EXP_CAT_SW", 35);
DcsBios::Switch2Pos cmdsChExpCatSw("CMDS_CH_EXP_CAT_SW", 36);
DcsBios::Switch2Pos cmdsFlExpCatSw("CMDS_FL_EXP_CAT_SW", 37);
const byte cmdsModeKnbPins[11] = {38, 39, 40, 41, 42, 43};
DcsBios::SwitchMultiPos cmdsModeKnb("CMDS_MODE_KNB", cmdsModeKnbPins, 11);
const byte cmdsProgKnbPins[9] = {44, 45, 46, 47, 48};
DcsBios::SwitchMultiPos cmdsProgKnb("CMDS_PROG_KNB", cmdsProgKnbPins, 9);


void setup() {
  DcsBios::setup();
  pinMode(RST    , OUTPUT); //reset pin, init on startup
  pinMode(WR     , OUTPUT);
  pinMode(CE     , OUTPUT);
  pinMode(AD0    , OUTPUT); // data register selection // these 4 pins allow one to select a data register.
  pinMode(AD1    , OUTPUT); // data register selection
  pinMode(AD2    , OUTPUT); // data register selection
  pinMode(DS, OUTPUT); //shift register serial in
  pinMode(ST_CP, OUTPUT); //shift register storage pin(latch)
  pinMode(SH_CP, OUTPUT); //shift register clock pin(clock)
  digitalWrite(CE, HIGH); // chip enable, go low before wr. go high after wr
  digitalWrite(WR, HIGH); // write enable, go low to start write, go high when done.
  resetDisplay();
  writeDisplay(" VIPER  "); // test display2 , can be deleted

  pinMode(RST1    , OUTPUT); //reset pin, init on startup
  pinMode(WR1     , OUTPUT);
  pinMode(CE1     , OUTPUT);
  pinMode(AD01    , OUTPUT); // data register selection // these 4 pins allow one to select a data register.
  pinMode(AD11    , OUTPUT); // data register selection
  pinMode(AD21    , OUTPUT); // data register selection
  pinMode(DS1, OUTPUT); //shift register serial in
  pinMode(ST_CP1, OUTPUT); //shift register storage pin(latch)
  pinMode(SH_CP1, OUTPUT); //shift register clock pin(clock)
  digitalWrite(CE1, HIGH); // chip enable, go low before wr. go high after wr
  digitalWrite(WR1, HIGH); // write enable, go low to start write, go high when done.
  resetDisplay1();
  writeDisplay1(" F-16 C "); // test display1 , can be deleted
  delay(2000); // can be deleted
}

void resetDisplay()
{
  digitalWrite(RST, LOW);
  delayMicroseconds(1);
  digitalWrite(RST, HIGH);
  delayMicroseconds(150);
}

void resetDisplay1()
{
  digitalWrite(RST1, LOW);
  delayMicroseconds(1);
  digitalWrite(RST1, HIGH);
  delayMicroseconds(150);
}

void writeDisplay(char *input)
{
  for (int i = 0; i < 8; i++)
  {
    // below bit(AD0,1,2) is setting the correct display character index(i)
    digitalWrite(AD0, (1 & i) != 0 ? HIGH : LOW); // character address because there are 8 alphanumeric digits
    digitalWrite(AD1, (2 & i) != 0 ? HIGH : LOW); // 000 is digit on far left. 111 is digit on far right
    digitalWrite(AD2, (4 & i) != 0 ? HIGH : LOW);
    delay(1);
    digitalWrite(ST_CP, LOW);
    delay(1);
    shiftOut(DS, SH_CP, MSBFIRST, input[i]);
    delay(1);
    digitalWrite(ST_CP, HIGH);
    delay(1);
    digitalWrite(CE, LOW);
    delay(1);
    digitalWrite(WR, LOW);
    delay(1);
    digitalWrite(WR, HIGH);
    delay(1);
    digitalWrite(CE, HIGH);
    delay(1);
  }
}

void writeDisplay1(char *input1)
{
  for (int i1 = 0; i1 < 8; i1++)
  {
    // below bit(AD0,1,2) is setting the correct display character index(i)
    digitalWrite(AD01, (1 & i1) != 0 ? HIGH : LOW); // character address because there are 8 alphanumeric digits
    digitalWrite(AD11, (2 & i1) != 0 ? HIGH : LOW); // 000 is digit on far left. 111 is digit on far right
    digitalWrite(AD21, (4 & i1) != 0 ? HIGH : LOW);
    delay(1);
    digitalWrite(ST_CP1, LOW);
    delay(1);
    shiftOut(DS1, SH_CP1, MSBFIRST, input1[i1]);
    delay(1);
    digitalWrite(ST_CP1, HIGH);
    delay(1);
    digitalWrite(CE1, LOW);
    delay(1);
    digitalWrite(WR1, LOW);
    delay(1);
    digitalWrite(WR1, HIGH);
    delay(1);
    digitalWrite(CE1, HIGH);
    delay(1);
  }
}

void loop() {
  char * output = (char *) malloc(1 + sizeof(char*) * (strlen(chaff) + strlen(flare)));
  strcpy(output, chaff);
  strcat(output, flare);
  writeDisplay(output);
  delay(5);
  delete[] output; // delete data stored

  char * output1 = (char *) malloc(1 + sizeof(char*) * (strlen(am01) + strlen(am02)));
  strcpy(output1, am01);
  strcat(output1, am02);
  writeDisplay1(output1);
  delay(5);
  delete[] output1; // delete data stored
  DcsBios::loop();
}
