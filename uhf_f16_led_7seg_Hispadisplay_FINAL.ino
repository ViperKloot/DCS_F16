#define DCSBIOS_DEFAULT_SERIAL

#include "LedControl.h"
//LedControl(DIN,CLK,L/CS,1-8Max72**)
LedControl lc = LedControl(12, 11, 10, 1);
LedControl lc2 = LedControl(9, 8, 7, 1);

#include "DcsBios.h"
int pushbtn = 0;
int uhf1 = 0;
int uhf2 = 0;
int uhf3 = 0;
int uhf4 = 0;
int uhf5 = 0;
int uhf6 = 0;
int uhf7 = 0;
int uhf8 = 0;
int uhfsw = 0;
int uhfmo = 0;

DcsBios::Switch2Pos uhfTestBtn("UHF_TEST_BTN", 13);


void onUhfFuncKnbChange(unsigned int newValue) {
  uhfsw = newValue;
}
DcsBios::IntegerBuffer uhfFuncKnbBuffer(0x4448, 0xc000, 14, onUhfFuncKnbChange);

void onUhfModeKnbChange(unsigned int newValue) {
  uhfmo = newValue;
}
DcsBios::IntegerBuffer uhfModeKnbBuffer(0x444a, 0x0003, 0, onUhfModeKnbChange);

void onUhfFreq100KnbChange(unsigned int newValue) {
  uhf1 = newValue + 1;
}
DcsBios::IntegerBuffer uhfFreq100KnbBuffer(0x4434, 0x6000, 13, onUhfFreq100KnbChange);

void onUhfFreq10KnbChange(unsigned int newValue) {
  uhf2 = newValue;
}
DcsBios::IntegerBuffer uhfFreq10KnbBuffer(0x4448, 0x000f, 0, onUhfFreq10KnbChange);

void onUhfFreq1KnbChange(unsigned int newValue) {
  uhf3 = newValue;
}
DcsBios::IntegerBuffer uhfFreq1KnbBuffer(0x4448, 0x00f0, 4, onUhfFreq1KnbChange);

void onUhfFreq01KnbChange(unsigned int newValue) {
  uhf4 = newValue;
}
DcsBios::IntegerBuffer uhfFreq01KnbBuffer(0x4448, 0x0f00, 8, onUhfFreq01KnbChange);

void onUhfFreq0025KnbChange(unsigned int newValue) {
  int freq = newValue * 25;
  int digit1 = freq % 10;
  int digit2 = freq / 10;
  uhf5 = digit2;
  uhf6 = digit1;
}
DcsBios::IntegerBuffer uhfFreq0025KnbBuffer(0x4448, 0x3000, 12, onUhfFreq0025KnbChange);

void onUhfChanKnbChange(unsigned int newValue) {
  int chn = newValue + 1;
  int digit1 = chn % 10;
  int digit2 = chn / 10;
  uhf7 = digit2;
  uhf8 = digit1;
}
DcsBios::IntegerBuffer uhfChanKnbBuffer(0x4434, 0x1f00, 8, onUhfChanKnbChange);

void onUhfTestBtnChange(unsigned int newValue) {
  pushbtn = (newValue);
}
DcsBios::IntegerBuffer uhfTestBtnBuffer(0x444a, 0x0008, 3, onUhfTestBtnChange);


void setup() {
  DcsBios::setup();
  pinMode(13, INPUT);
  lc.shutdown(0, false);
  lc2.shutdown(0, false);
  /* Set the brightness to a medium values */
  lc.setIntensity(0, 3);
  lc2.setIntensity(0, 3);
  /* and clear the display */
  lc.clearDisplay(0);
  lc2.clearDisplay(0);
}

void loop() {
  if (uhfsw == 0)
  {
    lc.setChar(0, 0, ' ', false);
    lc.setChar(0, 1, ' ', false);
    lc.setChar(0, 2, ' ', false);
    lc.setChar(0, 3, ' ', false);
    lc.setChar(0, 4, ' ', false);
    lc.setChar(0, 5, ' ', false);
    lc2.setChar(0, 4, ' ', false);
    lc2.setChar(0, 5, ' ', false);
  }
  else
  {
    if (pushbtn == 0)
    {
      lc.setDigit(0, 0, uhf1, false);
      lc.setDigit(0, 1, uhf2, false);
      lc.setDigit(0, 2, uhf3, true);
      lc.setDigit(0, 3, uhf4, false);
      lc.setDigit(0, 4, uhf5, false);
      lc.setDigit(0, 5, uhf6, false);
      if ((uhfmo == 0) || (uhfmo == 2))  
      {
        lc2.setChar(0, 4, ' ', false);
        lc2.setChar(0, 5, ' ', false);
      }
      else
      {
        lc2.setDigit(0, 4, uhf7, false);
        lc2.setDigit(0, 5, uhf8, false);
      }
    }
    else
    {
      lc.setDigit(0, 0, 8, false);
      lc.setDigit(0, 1, 8, false);
      lc.setDigit(0, 2, 8, true);
      lc.setDigit(0, 3, 8, false);
      lc.setDigit(0, 4, 8, false);
      lc.setDigit(0, 5, 8, false);
      lc2.setDigit(0, 4, 8, false);
      lc2.setDigit(0, 5, 8, false);
    }
  }
  DcsBios::loop();
}
