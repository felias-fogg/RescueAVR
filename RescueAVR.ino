// -*- mode: c++; tab-width: 4; indent-tabs-mode: nil -*-

//  Title:        RescueAVR

#define VERSION  "2.0"

/*
  based on Jeff Keyzer's HVRescue_Shield and manekinen's Fusebit Doctor.

  It uses the hardware dsigned by manekinen but the software is completely
  new, using ideas und snippets from  Jeff Keyzer's sketch HVRescue_Shield.
  In addition, the program can also be run on a native Arduino, provided
  12V are supplied and the target chip is connected to the right Arduino
  pins.

  The program decides on its own whether it runs in Arduino or
  Fusebit-Doctor mode. This decision is based on the CPU frequency.
  With F_CPU <= 8000000, it is assumed that the software runs on the Fusebit-Doctor
  board. Otherwise it must be an Arduino board. This can be overriden with a define
  below.
  
  Copyright 2013 by Bernhard Nebel and parts are copyrighted by Jeff Keyzer.
 
  This program is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

/* Uncomment one of the following two lines, if you do not want to
   leave the decision on which board we are executed to the 
   the CPU frequency, i.e. F_CPU <= 8000000 -> FBD_MODE, otherwise ARDUNIO_MODE
*/
// #define FBD_MODE
// #define ARDUINO_MODE

#if !defined(FBD_MODE) && !defined(ARDUINO_MODE)
#if F_CPU <= 8000000L 
   #define FBD_MODE
#endif
#endif

#include <avr/pgmspace.h>

// communication speed 
#define  BAUD         19200    // Serial port rate at which to talk to PC


// The names of all MCUs known to us
char at90s2313[] PROGMEM = "AT90S2313";
char at90s2333[] PROGMEM = "AT90S2333";
char at90s2343[] PROGMEM = "AT90S2343";
char at90s4433[] PROGMEM = "AT90S4433";
char at90s4434[] PROGMEM = "AT90S4434";
char at90s8515[] PROGMEM = "AT90S8515";
char at90s8535[] PROGMEM = "AT90S8535";
char attiny11[] PROGMEM = "ATtiny11";
char attiny12[] PROGMEM = "ATtiny12";
char attiny13[] PROGMEM = "ATtiny13";
char attiny15[] PROGMEM = "ATtiny15";
char attiny2313[] PROGMEM = "ATtiny2313";
char attiny4313[] PROGMEM = "ATtiny4313";
char attiny24[] PROGMEM = "ATtiny24";
char attiny44[] PROGMEM = "ATtiny44";
char attiny84[] PROGMEM = "ATtiny84";
char attiny25[] PROGMEM = "ATtiny25";
char attiny45[] PROGMEM = "ATtiny45";
char attiny85[] PROGMEM = "ATtiny85";
char attiny26[] PROGMEM = "ATtiny26";
char attiny261[] PROGMEM = "ATtiny261";
char attiny461[] PROGMEM = "ATtiny461";
char attiny861[] PROGMEM = "ATtiny861";
char attiny28[] PROGMEM = "ATtiny28";
char attiny48[] PROGMEM = "ATtiny48";
char attiny88[] PROGMEM = "ATtiny88";
char atmega8515[] PROGMEM = "ATmega8515";
char atmega8535[] PROGMEM = "ATmega8535";
char atmega8[] PROGMEM = "ATmega8";
char atmega8hva[] PROGMEM = "ATmega8HVA";
char atmega16[] PROGMEM = "ATmega16";
char atmega16hva[] PROGMEM = "ATmega16HVA";
char atmega16hvb[] PROGMEM = "ATmega16HVB";
char atmega32[] PROGMEM = "ATmega32";
char atmega32c1[] PROGMEM = "ATmega32C1";
char atmega32hvb[] PROGMEM = "ATmega32HVB";
char atmega64[] PROGMEM = "ATmega64";
char atmega64c1[] PROGMEM = "ATmega64c1";
char atmega64hev2[] PROGMEM = "ATmega64HEV2";
char atmega64m1[] PROGMEM = "ATmega64m1";
char atmega128[] PROGMEM = "ATmega128";
char atmega128rfa1[] PROGMEM = "ATmega128RFA1";
char atmega162[] PROGMEM = "ATmega162";
char atmega48[] PROGMEM = "ATmega48";
char atmega48p[] PROGMEM = "ATmega48P";
char atmega88[] PROGMEM = "ATmega88";
char atmega88p[] PROGMEM = "ATmega88P";
char atmega168[] PROGMEM = "ATmega168";
char atmega168p[] PROGMEM = "ATmega168P";
char atmega328[] PROGMEM = "ATmega328";
char atmega164a[] PROGMEM = "ATmega164A";
char atmega164p[] PROGMEM = "ATmega164P";
char atmega324a[] PROGMEM = "ATmega324A";
char atmega324p[] PROGMEM = "ATmega324P";
char atmega324pa[] PROGMEM = "ATmega324PA";
char atmega644[] PROGMEM = "ATmega644";
char atmega644p[] PROGMEM = "ATmega644P";
char atmega644rfr2[] PROGMEM = "ATmega644RFR2";
char atmega1284[] PROGMEM = "ATmega1284";
char atmega1284p[] PROGMEM = "ATmega1284P";
char atmega1284rfr2[] PROGMEM = "ATmega1284RFR2";
char atmega328p[] PROGMEM = "ATmega328P";

// table with signatures
// 1st word signature (w/o 1E), 2nd word: MSB number of fuses (or 0 if we do not know),
// 2nd word LSB: low fuse, 3rd word MSB: high fuse, 3rd word LSB: extended fuse,
// 4th word string address. 

#define MCU_NUM 62
uint16_t mcu_types[MCU_NUM][4] PROGMEM =
  { 
    { 0x9101, 0x01FF, 0x0000, (uint16_t)at90s2313 },
    { 0x9105, 0x01FF, 0x0000, (uint16_t)at90s2333 },
    { 0x9103, 0x01FF, 0x0000, (uint16_t)at90s2343 },
    { 0x9203, 0x01FF, 0x0000, (uint16_t)at90s4433 },
    { 0x9202, 0x01FF, 0x0000, (uint16_t)at90s4434 },
    { 0x9301, 0x01FF, 0x0000, (uint16_t)at90s8515 },
    { 0x9303, 0x01FF, 0x0000, (uint16_t)at90s8535 },
    { 0x9004, 0x01FF, 0x0000, (uint16_t)attiny11 },
    { 0x9005, 0x01FF, 0x0000, (uint16_t)attiny12 },
    { 0x9007, 0x026A, 0xFF00, (uint16_t)attiny13 },
    { 0x9006, 0x01FF, 0x0000, (uint16_t)attiny15 },
    { 0x910A, 0x0364, 0xDFFF, (uint16_t)attiny2313 },
    { 0x920D, 0x0364, 0xDFFF, (uint16_t)attiny4313 },
    { 0x910B, 0x0362, 0xDFFF, (uint16_t)attiny24 },
    { 0x9207, 0x0362, 0xDFFF, (uint16_t)attiny44 },
    { 0x930C, 0x0362, 0xDFFF, (uint16_t)attiny84 },
    { 0x9108, 0x0362, 0xDFFF, (uint16_t)attiny25 },
    { 0x9206, 0x0362, 0xDFFF, (uint16_t)attiny45 },
    { 0x930B, 0x0362, 0xDFFF, (uint16_t)attiny85 },
    { 0x9109, 0x0241, 0xF700, (uint16_t)attiny26 },
    { 0x910C, 0x0362, 0xDFFF, (uint16_t)attiny261 },
    { 0x9208, 0x0362, 0xDFFF, (uint16_t)attiny461 },
    { 0x930D, 0x0362, 0xDFFF, (uint16_t)attiny861 },
    { 0x9107, 0x01FF, 0x0000, (uint16_t)attiny28 },
    { 0x9209, 0x036E, 0xDFFF, (uint16_t)attiny48 },
    { 0x9311, 0x036E, 0xDFFF, (uint16_t)attiny88 },
    { 0x9306, 0x02C1, 0xD900, (uint16_t)atmega8515 },
    { 0x9308, 0x02C1, 0xD900, (uint16_t)atmega8535 },
    { 0x9307, 0x02E1, 0xD900, (uint16_t)atmega8 },
    { 0x9310, 0x01DF, 0x0000, (uint16_t)atmega8hva },
    { 0x9403, 0x02E1, 0x9900, (uint16_t)atmega16 },
    { 0x940c, 0x01DF, 0x0000, (uint16_t)atmega16hva },
    { 0x940d, 0x02DE, 0xE900, (uint16_t)atmega16hvb },
    { 0x9502, 0x02E1, 0x9900, (uint16_t)atmega32 },
    { 0x9586, 0x0341, 0xD9F9, (uint16_t)atmega32c1 },
    { 0x9510, 0x02DE, 0xE900, (uint16_t)atmega32hvb },
    { 0x9602, 0x03C1, 0x99FF, (uint16_t)atmega64 },
    { 0x9686, 0x0341, 0xD9F9, (uint16_t)atmega64c1 },
    { 0x9610, 0x02D6, 0xF900, (uint16_t)atmega64hev2 },
    { 0x9684, 0x0341, 0xD9F9, (uint16_t)atmega64m1 },
    { 0x9702, 0x03C1, 0x99FD, (uint16_t)atmega128 },
    { 0x9701, 0x03C1, 0x99FD, (uint16_t)atmega128rfa1 },
    { 0x9404, 0x0362, 0x99FF, (uint16_t)atmega162 },
    { 0x9205, 0x0362, 0xDFFF, (uint16_t)atmega48 },
    { 0x920A, 0x0362, 0xDFFF, (uint16_t)atmega48p },
    { 0x930A, 0x0362, 0xDFFF, (uint16_t)atmega88 },
    { 0x930F, 0x0362, 0xDFF9, (uint16_t)atmega88p },
    { 0x9406, 0x0362, 0xDFF9, (uint16_t)atmega168 },
    { 0x940B, 0x0362, 0xDFFF, (uint16_t)atmega168p },
    { 0x9514, 0x0362, 0xD9FF, (uint16_t)atmega328 },
    { 0x940F, 0x0342, 0x99FF, (uint16_t)atmega164a },
    { 0x940A, 0x0342, 0x99FF, (uint16_t)atmega164p },
    { 0x9515, 0x0342, 0x99FF, (uint16_t)atmega324a },
    { 0x9508, 0x0342, 0x99FF, (uint16_t)atmega324p },
    { 0x9511, 0x0342, 0x99FF, (uint16_t)atmega324pa },
    { 0x9609, 0x0362, 0x99FF, (uint16_t)atmega644 },
    { 0x960A, 0x0362, 0x99FF, (uint16_t)atmega644p },
    { 0x9602, 0x0000, 0x0000, (uint16_t)atmega644rfr2 },
    { 0x9706, 0x0342, 0x99FF, (uint16_t)atmega1284 },
    { 0x9705, 0x0342, 0x99FF, (uint16_t)atmega1284p },
    { 0x9703, 0x0000, 0x0000, (uint16_t)atmega1284rfr2 },
    { 0x950F, 0x0362, 0xD9FF, (uint16_t)atmega328p },
  };


#ifdef FBD_MODE
/* pin assignment for the FBD board */
/************************************/
#define  VCC      5 
#define  RDY     14 
#define  OE       2 
#define  WR       4 
#define  BS1     15 
#define  BS2      6 
#define  XA0     16 
#define  ORIGXA1 17 
#define  RST      7 
#define  XTAL1    3 
#define  ORIGPAGEL 19
#define  ECJUMPER 18
// HVSP pins
#define  SCI    RDY // note that the direction must be changed!
#define  SDO    BS1
#define  SII    XA0
#define  SDI    ORIGXA1
// special values to control the VCC line
#define  VCCOFF   HIGH
#define  VCCON    LOW
#else
/* pin assignment for Arduino board */
/************************************/
#define  VCC       12
#define  RST       13

#define  RDY       A4 
#define  OE        A3 
#define  WR        A2 
#define  BS1       A1
#define  BS2       A5
#define  XA0       10
#define  ORIGXA1   11
#define  XTAL1     A0
#define  ORIGPAGEL BS1 // is actually not used and tied to GND
// HVSP pins
#define  SCI       A5
#define  SDO       A2
#define  SII       A1
#define  SDI       A0
// special values to control the VCC line
#define  VCCOFF    LOW
#define  VCCON     HIGH

#endif

// These pin assignments change depending on which chip is being programmed
byte PAGEL = ORIGPAGEL; // ATtiny2313: PAGEL = BS1
byte XA1 = ORIGXA1; // ATtiny2313: XA1 = BS2


// Serial instructions for HVSP mode
// Based on the ATtiny85 datasheet Table 20-16 pg. 163-165.
// These instructions don't contain the necessary zero padding, which is added later.

// LFUSE
#define HVSP_READ_LFUSE_DATA     B00000100  // For the commands we are interested in
#define HVSP_READ_LFUSE_INSTR1   B01001100  // only the 1st instruction contains a fixed data packet.
#define HVSP_READ_LFUSE_INSTR2   B01101000  // Instructions 2-3 have data = all zeros.
#define HVSP_READ_LFUSE_INSTR3   B01101100

#define HVSP_WRITE_LFUSE_DATA    B01000000  // For the write instructions, the data contents
#define HVSP_WRITE_LFUSE_INSTR1  B01001100  // for instruction 2 are the desired fuse bits.
#define HVSP_WRITE_LFUSE_INSTR2  B00101100  // Instructions 3-4 have data = all zeros.
#define HVSP_WRITE_LFUSE_INSTR3  B01100100
#define HVSP_WRITE_LFUSE_INSTR4  B01101100

// HFUSE
#define HVSP_READ_HFUSE_DATA     B00000100
#define HVSP_READ_HFUSE_INSTR1   B01001100
#define HVSP_READ_HFUSE_INSTR2   B01111010
#define HVSP_READ_HFUSE_INSTR3   B01111110

#define HVSP_WRITE_HFUSE_DATA    B01000000
#define HVSP_WRITE_HFUSE_INSTR1  B01001100
#define HVSP_WRITE_HFUSE_INSTR2  B00101100
#define HVSP_WRITE_HFUSE_INSTR3  B01110100
#define HVSP_WRITE_HFUSE_INSTR4  B01111100

// EFUSE
// Note: not all ATtiny's have an EFUSE
#define HVSP_READ_EFUSE_DATA     B00000100  
#define HVSP_READ_EFUSE_INSTR1   B01001100
#define HVSP_READ_EFUSE_INSTR2   B01101010
#define HVSP_READ_EFUSE_INSTR3   B01101110

#define HVSP_WRITE_EFUSE_DATA    B01000000
#define HVSP_WRITE_EFUSE_INSTR1  B01001100
#define HVSP_WRITE_EFUSE_INSTR2  B00101100
#define HVSP_WRITE_EFUSE_INSTR3  B01100110
#define HVSP_WRITE_EFUSE_INSTR4  B01101110

// LOCK
#define HVSP_READ_LOCK_DATA      B00000100
#define HVSP_READ_LOCK_INSTR1    B01001100
#define HVSP_READ_LOCK_INSTR2    B01111000
#define HVSP_READ_LOCK_INSTR3    B01101100

#define HVSP_WRITE_LOCK_DATA     B00100000
#define HVSP_WRITE_LOCK_INSTR1   B01001100
#define HVSP_WRITE_LOCK_INSTR2   B00101100
#define HVSP_WRITE_LOCK_INSTR3   B01100100
#define HVSP_WRITE_LOCK_INSTR4   B01101100

// Signature read
#define HVSP_READ_SIG_DATA       B00001000
#define HVSP_READ_SIG_INSTR1     B01001100
#define HVSP_READ_SIG_INSTR2     B00001100
#define HVSP_READ_SIG_INSTR3     B01101000
#define HVSP_READ_SIG_INSTR4     B01101100

// Erase chip
#define HVSP_ERASE_CHIP_DATA     B10000000
#define HVSP_ERASE_CHIP_INSTR1   B01001100
#define HVSP_ERASE_CHIP_INSTR2   B01100100
#define HVSP_ERASE_CHIP_INSTR3   B01101100


// Internal definitions
enum { ATMEGA, TINY2313, HVSP };
enum { LFUSE_SEL, HFUSE_SEL, EFUSE_SEL, LOCK_SEL };
enum { RED, GREEN };

// Global variables
byte mcu_mode;  // programming mode
byte mcu_fuses = 0; // number of fuses for the actual chip
boolean interactive_mode = true; // interaction over terminal
int mcu_index = -1; // index into mcu_types array - see above
unsigned long mcu_signature = 0; // signature as read from chip
boolean ec_allowed = true; // whether chip erase is allowed - only relevant for FBD


void setup() { // run once, when the sketch starts
  char modec = ' ';
  
  // Set up control lines for HV parallel programming
  
  setData(0);
  setDataDirection(INPUT);

#ifdef FBD_MODE
  pinMode(ECJUMPER, INPUT_PULLUP); // the jumper that allows chip erase
#endif
  pinMode(VCC, OUTPUT);
  pinMode(RDY, INPUT);
  pinMode(OE, OUTPUT);
  pinMode(WR, OUTPUT);
  pinMode(BS1, OUTPUT);
  pinMode(XA0, OUTPUT);
  pinMode(XA1, OUTPUT);
  pinMode(PAGEL, OUTPUT);
  pinMode(RST, OUTPUT);  // 12V Reset signal
  pinMode(BS2, OUTPUT);
  pinMode(XTAL1, OUTPUT);

#ifdef FBD_MODE
  if (digitalRead(0) == LOW)
  interactive_mode = false; // no RS232 connection!
#endif

  // Initialize output pins as needed
  digitalWrite(RST, LOW);  // Turn off 12V 

  digitalWrite(VCC, VCCOFF); // Turn off VCC

  Serial.begin(BAUD);  // Open serial port
  Serial.println();
  Serial.println();
  Serial.print(F("RescueAVR Version "));
  Serial.print(VERSION);
  if (interactive_mode) Serial.print(F(" / interactive, "));
  else Serial.print(F(" / non-interactive, "));
#ifdef FBD_MODE
  Serial.println(F("running on Fusebit-Doctor Board"));
#else
  Serial.println(F("running on Arduino"));
#endif
  Serial.println();

  for (mcu_mode = ATMEGA; mcu_mode <= HVSP; mcu_mode++) {
    enterHVProgMode(mcu_mode);
    mcu_signature = readSig(mcu_mode);
    leaveHVProgMode();
    // Serial.print(F("DEBUG: SIG="));
    // Serial.println(mcu_signature,HEX);
    if ((mcu_signature>>16) == 0x1E) break;
  }
  if (mcu_mode > HVSP) {
    Serial.println(F("No chip found!"));
#ifndef FBD_MODE
    Serial.println(F("Check wiring and try again."));
    Serial.println();
    while (true) ;
#endif
    Serial.println(F("Insert chip and reset or give details."));
    if (!interactive_mode) {
      showLed(true,false,3000); // 3 secs of red
      while (true) { };
    }
    while (Serial.available()) Serial.read();
    while (true) {
      Serial.println();
      Serial.println(F("Choose from:"));
      Serial.println(F("  P - HVPP"));
      Serial.println(F("  T - HVPP for Tiny"));
      Serial.println(F("  S - HVSP"));
      Serial.println(F("  X - exit"));
      Serial.print(F("Choice: "));
      while (!Serial.available())  { };
      modec = toupper(Serial.read());
      Serial.println(modec);
      while (Serial.available()) Serial.read();
      if (modec == 'P' || modec == 'T' || modec == 'S' || modec == 'X') break;
      Serial.print("'");
      Serial.print(modec);
      Serial.println(F("' is not valid choice."));
    }
    switch (modec) {
    case 'P': mcu_mode = ATMEGA; break;
    case 'T': mcu_mode = TINY2313; break;
    case 'S': mcu_mode = HVSP; break;
    default: while (true) { };
    }
  }
  Serial.print(F("Signature: "));
  Serial.println(mcu_signature,HEX);
  mcu_index = searchMCU(mcu_signature);
  Serial.print(F("MCU name:  "));
  if (mcu_index >= 0) {
    if ((pgm_read_word(&(mcu_types[mcu_index][1]))>>8) == false) {
      // unfortunately, we do not have enough info about this chip
      printMCUName(mcu_index);
      Serial.print(F(" / "));
      if (!interactive_mode) {
        showLed(false,true,1000);
      } 
      mcu_index = -1;
    }
  }
  if (mcu_index < 0 ) {
    Serial.println(F("Unknown"));
    if (!interactive_mode) {
      showLed(true,false,3000); // 3 secs of red
      while (true) { };
    } else {
      mcu_fuses = 0;
      while (Serial.available()) Serial.read();
      while (mcu_fuses < 1 || mcu_fuses > 3) {
        Serial.print(F("# of fuse bytes (1-3): "));
        while (!Serial.available())  { };
        mcu_fuses = Serial.read();
        Serial.println((char)mcu_fuses);
        while (Serial.available()) Serial.read();
        mcu_fuses = mcu_fuses - '0';
      }
    }
  } else {
    printMCUName(mcu_index);
    Serial.println();
    mcu_fuses = (pgm_read_word(&(mcu_types[mcu_index][1]))>>8);
    if (!interactive_mode) showLed(false,true,3000); // 3 secs of green
  }
}

void loop() {  // run over and over again
  
  byte hfuse, lfuse, efuse, lock;  // fuse and lock values
  byte dhfuse, dlfuse, defuse;  // default values
  char action = ' ';
  
  if (mcu_index >= 0) {
    dlfuse = pgm_read_word(&(mcu_types[mcu_index][1]))&0xFF;
    dhfuse = pgm_read_word(&(mcu_types[mcu_index][2]))>>8;
    defuse = pgm_read_word(&(mcu_types[mcu_index][2]))&0xFF;
  }

  enterHVProgMode(mcu_mode);
  
  lfuse = readFuse(mcu_mode,LFUSE_SEL);
  if (mcu_fuses > 1) hfuse = readFuse(mcu_mode,HFUSE_SEL);
  if (mcu_fuses > 2) efuse = readFuse(mcu_mode,EFUSE_SEL);
  lock = readFuse(mcu_mode,LOCK_SEL);
  leaveHVProgMode();

  printFuses("Current ",mcu_fuses,lfuse,hfuse,efuse);
  if (mcu_index >= 0) printFuses("Default ",mcu_fuses,dlfuse,dhfuse,defuse);

  printLock(lock);
 

  if (interactive_mode) {
    while (true) {
      while (Serial.available()) Serial.read();
      Serial.println();
      Serial.println(F("Choose:"));
      if (mcu_index >= 0) Serial.println(F("  R - Try to resurrect chip by all means"));
      Serial.println(F("  E - Erase chip"));
      if (mcu_index >= 0) Serial.println(F("  D - Burn default fuse values"));
      Serial.println(F("  L - Change low fuse"));
      if (mcu_fuses > 1) Serial.println(F("  H - Change high fuse"));
      if (mcu_fuses > 2) Serial.println(F("  X - Change extended fuse"));
      Serial.println(F("  K - Change lock byte"));
      Serial.print(F("Action: "));
      
      while (!Serial.available()) { };
      action = toupper(Serial.read());
      Serial.println(action);
      
      if (action == 'E' || action == 'D' || action == 'L' ||
          action == 'H' || action == 'X' || action == 'K' || action == 'R') break;
      if (action >= ' ') {
        Serial.print("'");
        Serial.print(action);
        Serial.println("' is not a valid command char.");
      }
    }
  
    enterHVProgMode(mcu_mode);
    switch (action) {
    case 'R':
      if (mcu_index >= 0) resurrection(dlfuse, dhfuse, defuse);
      break;
    case 'E': 
#ifdef FBD_MODE
      ec_allowed = (digitalRead(ECJUMPER) == LOW);
#endif
      if (ec_allowed) {
        Serial.print(F("\nErasing chip ..."));
        eraseChip(mcu_mode); 
        verifyOneByte(LOCK_SEL, 0xFF, "erasing chip");
      }
      else Serial.println(F("'Erase chip' is not allowed"));
      break;
    case 'D': 
      if (mcu_index >= 0) {
        Serial.print(F("\nSetting fuses ... "));
        burnFuse(mcu_mode, dlfuse, LFUSE_SEL);
        if (mcu_fuses > 1) burnFuse(mcu_mode, dhfuse, HFUSE_SEL);
        if (mcu_fuses > 2) burnFuse(mcu_mode, defuse, EFUSE_SEL);
        showLed(true,true,600);
        Serial.println(F("done"));
        delay(100);
        Serial.print(F("Verifying ... "));
        lfuse = readFuse(mcu_mode,LFUSE_SEL);
        if (mcu_fuses > 1) hfuse = readFuse(mcu_mode,HFUSE_SEL);
        if (mcu_fuses > 2) efuse = readFuse(mcu_mode,EFUSE_SEL);
        showLed(true,true,600);
        if (verifyFuses(mcu_fuses,
                        0xFF,0xFF,
                        lfuse,dlfuse,
                        hfuse,dhfuse,
                        efuse,defuse)) {
          Serial.println(F("done: Sucessfully set all fuses to default values"));
        } else {
          Serial.println(F("done: FAILED TO SET FUSES\x07"));
        }
      }
      break;
    case 'L': setAndVerifyOneByte(LFUSE_SEL, "setting low fuse");
      break;
    case 'H': 
      if (mcu_fuses > 1) {
        setAndVerifyOneByte(HFUSE_SEL, "setting high fuse");
      }
      break;
    case 'X':
      if (mcu_fuses > 2) {
        setAndVerifyOneByte(EFUSE_SEL, "setting extended fuse");
      }
      break;
    case 'K':
      setAndVerifyOneByte(LOCK_SEL, "setting lock byte");
      break;
    }
    Serial.println();
    leaveHVProgMode();
  } else { // non-interactive mode
    resurrection(dlfuse, dhfuse, defuse);
    Serial.println(F("Bye, bye ..."));
    while (true) { };
  }
}

void resurrection(byte dlf, byte dhf, byte def) {
  byte lf, hf, ef, lk;
  
  Serial.println(F("Start HV programming mode..."));
  enterHVProgMode(mcu_mode);
  showLed(true,true,400);
  
  // try to reset all locks
  Serial.println(F("Reset lock byte - if possible..."));
  burnFuse(mcu_mode, 0xFF, LOCK_SEL);
  showLed(true,false,200);
  showLed(false,true,200);
  
  // remove lock by erasing chip - if necessary and allowed
  lk = readFuse(mcu_mode, LOCK_SEL);
#ifdef FBD_MODE
  ec_allowed = (digitalRead(ECJUMPER) == LOW);
#endif
  if (lk != 0xFF && ec_allowed) {
    Serial.println(F("Lock byte could not be changed. Erase chip ..."));
    eraseChip(mcu_mode); 
    showLed(true,true,400);
  }
  // try to reset fuses
  Serial.println(F("Set fuses to default values ..."));
  burnFuse(mcu_mode, dlf, LFUSE_SEL);
  if (mcu_fuses > 1) burnFuse(mcu_mode, dhf, HFUSE_SEL);
  if (mcu_fuses > 2) burnFuse(mcu_mode, def, EFUSE_SEL);
  showLed(true,true,400);
  
  // see whether successful
  lk = readFuse(mcu_mode, LOCK_SEL);
  lf = readFuse(mcu_mode,LFUSE_SEL);
  if (mcu_fuses > 1) hf = readFuse(mcu_mode,HFUSE_SEL);
  if (mcu_fuses > 2) ef = readFuse(mcu_mode,EFUSE_SEL);
  
  leaveHVProgMode();
  // if successful blink green, otherwise blink red
  Serial.println(F("Verify fuse settings..."));
  showLed(true,false,100);
  showLed(false,true,100);
  showLed(true,false,100);
  showLed(false,true,100);
  showLed(true,false,100);
  showLed(false,true,100);
  if (verifyFuses(mcu_fuses,
                  lk,0xFF,
                  lf,dlf,
                  hf,dhf,
                  ef,def)) {
    Serial.println(F("-> Resurrection was successful!"));
    ledBlink(GREEN,5);
  } else {
    Serial.println(F("-> Resurrection FAILED!"));
    if (!ec_allowed) 
      Serial.println(F("Try again with 'erase chip' jumper set"));
    ledBlink(RED,5);
  }
}


void setAndVerifyOneByte(int SEL, char* mess) {
  int toburn;

  while (Serial.available()) Serial.read();
  Serial.print(F("New "));
  Serial.print(mess);
  Serial.print(F(" : "));
  toburn = askByte();
  if (toburn >= 0x00) {
    Serial.print(F("\nSetting "));
    Serial.print(mess);
    Serial.print(F(" ... "));
    burnFuse(mcu_mode, toburn, SEL);
    verifyOneByte(SEL, toburn, mess);
  }
}

void verifyOneByte(int SEL, byte desired, char* mess) {
  byte newval;

  showLed(true,true,600);
  Serial.println(F(" done"));
  delay(200);
  Serial.print(F("Verifying ... "));
  newval = readFuse(mcu_mode,SEL);
  showLed(true,true,600);
  if (newval == desired) {
    Serial.print(F("done: Successful in "));
  } else {
    Serial.print(F("done: FAILED in \x07"));
  }
  Serial.println(mess);
}
  


int searchMCU(unsigned long sig) {
  if ((sig>>16) != 0x1E) return -1;
  for (int i=0; i < MCU_NUM; i++) {
    if ((sig&0xFFFF) == pgm_read_word(&(mcu_types[i][0])))
      return i;
  }
  return -1;
}

void printMCUName(int ix) {
  char buf[15];
  if (ix < 0 || ix >= MCU_NUM) return;
  strcpy_P(buf, (PGM_P)pgm_read_word(&(mcu_types[ix][3])));
  Serial.print(buf);
}

boolean verifyFuses(int num, byte k1, byte k2, byte l1, byte l2, byte h1, byte h2, byte e1, byte e2) {
  boolean correct;
  correct = (k1 == k2);
  correct = (correct && (l1 == l2));
  if (num > 1) correct = (correct && (h1 == h2));
  if (num > 2) correct = (correct && (e1 == e2));
  return correct;
}

unsigned long readSig(int mode) {
  if (mode == HVSP) return readHVSPSig();
  else return readHVPPSig(mode);
}

byte readFuse(int mode, int selection) {
  if (mode == HVSP) return readHVSPFuse(selection);
  else return readHVPPFuse(mode,selection);
}

void burnFuse(int mode, byte val, int selection) {
  if (mode == HVSP) burnHVSPFuse(val,selection);
  else burnHVPPFuse(mode,val,selection);
}

void eraseChip(int mode) {
  if (mode == HVSP) eraseHVSP();
  else eraseHVPP(mode);  
}

void enterHVProgMode(int mode) {
  
  // Initialize pins to enter programming mode
  if (mode == TINY2313) {
    PAGEL = BS1;
    XA1 = BS2;
  } else {
    PAGEL = ORIGPAGEL;
    XA1 = ORIGXA1;
  }
  
  setData(0);
  setDataDirection(INPUT);
  digitalWrite(PAGEL, LOW);
  digitalWrite(XA1, LOW);
  digitalWrite(XA0, LOW);
  digitalWrite(BS1, LOW);
  digitalWrite(BS2, LOW);
  digitalWrite(WR, LOW);  // ATtiny2313 needs this to be low to enter programming mode, ATmega doesn't care
  digitalWrite(OE, LOW);
  delay(100); // to let it settle, since otherwise Vcc may be already high due to XA1 high
 
  if(mode == HVSP) {
    pinMode(SCI,OUTPUT);
    pinMode(SDI,OUTPUT);
    pinMode(SII,OUTPUT);
    pinMode(SDO,OUTPUT);
    digitalWrite(SDI, LOW);  // set necessary pin values to enter programming mode
    digitalWrite(SII, LOW);
    digitalWrite(SDO, LOW);  // needs to be low to enter programming mode
    digitalWrite(SCI, LOW);
  }
  
  // Enter programming mode
  digitalWrite(VCC, VCCON);  // Apply VCC to start programming process
  delayMicroseconds(80);
  digitalWrite(RST, HIGH);   // Apply 12V to !RESET 
  
  if(mode == HVSP) {
    // reset SDO after short delay, longer leads to logic contention because target sets SDO high after entering programming mode
    delayMicroseconds(1);   // datasheet says 10us, 1us is needed to avoid drive contention on SDO 
    pinMode(SDO, INPUT);    // set to input to avoid logic contention
  }
  
  delayMicroseconds(10);  // Give lots of time for part to enter programming mode
  digitalWrite(OE, HIGH);
  digitalWrite(WR, HIGH);   // Now that we're in programming mode we can disable !WR
  delay(1);
}

void leaveHVProgMode() {
  setData(0);
  setDataDirection(INPUT);
  
  PAGEL = ORIGPAGEL;
  XA1 = ORIGXA1;
  digitalWrite(OE, HIGH);
  digitalWrite(RST, LOW);  // exit programming mode
  delay(1);
  digitalWrite(OE, LOW);
  digitalWrite(WR, LOW);
  digitalWrite(PAGEL, LOW);
  digitalWrite(XA1, LOW);
  digitalWrite(XA0, LOW);
  digitalWrite(BS1, LOW);
  digitalWrite(BS2, LOW);
  digitalWrite(VCC, VCCOFF);
}


void eraseHVPP(int mode) {
  
  sendHVPPCmdOrAddr(mode,true,B10000000); 
  digitalWrite(WR, LOW); 
  delay(1);
  digitalWrite(WR, HIGH);
  //delay(100);
  
  waitForHigh(RDY); // when RDY goes high, we are done
}
  
void eraseHVSP(void) {
  HVSP_write(HVSP_ERASE_CHIP_DATA, HVSP_ERASE_CHIP_INSTR1);
  HVSP_write(0x00, HVSP_ERASE_CHIP_INSTR2);
  HVSP_write(0x00, HVSP_ERASE_CHIP_INSTR3);
  waitForHigh(SDO); 
}


void burnHVPPFuse(int mode, byte fuse, int select)  // write high or low fuse to AVR
{
  
  // Send command to enable fuse programming mode
  if (select == LOCK_SEL)   sendHVPPCmdOrAddr(mode,true,B00100000);  
  else sendHVPPCmdOrAddr(mode,true,B01000000);  
  
  // Enable data loading
  digitalWrite(XA1, LOW);
  digitalWrite(XA0, HIGH);
  // Specify low byte
  digitalWrite(BS1, LOW); 
  if (mode != TINY2313)
    digitalWrite(BS2, LOW);  
  delay(1);
  
  // Load fuse value into target
  setData(fuse);
  setDataDirection(OUTPUT);
  
  strobe_xtal();  // latch DATA

  setData(0);
  setDataDirection(INPUT);
   
  // Decide which fuse location to burn
  switch (select) { 
  case HFUSE_SEL:
    digitalWrite(BS1, HIGH); // program HFUSE
    digitalWrite(BS2, LOW);
    break;
  case LFUSE_SEL:
    digitalWrite(BS1, LOW);  // program LFUSE
    digitalWrite(BS2, LOW);
    break;
  case EFUSE_SEL:
    digitalWrite(BS1, LOW);  // program EFUSE
    digitalWrite(BS2, HIGH);
    break;
  }
  delay(1);
   // Burn the fuse
  digitalWrite(WR, LOW); 
  delay(1);
  digitalWrite(WR, HIGH);
  //delay(100);
  
  waitForHigh(RDY); // when RDY goes high, we are done
  
  // Reset control lines to original state
  digitalWrite(BS1, LOW);
  digitalWrite(BS2, LOW);
}

void burnHVSPFuse(byte fuse, int select) {

  switch (select) {
  case LFUSE_SEL:
    HVSP_write(HVSP_WRITE_LFUSE_DATA, HVSP_WRITE_LFUSE_INSTR1);
    HVSP_write(fuse, HVSP_WRITE_LFUSE_INSTR2);
    HVSP_write(0x00, HVSP_WRITE_LFUSE_INSTR3);
    HVSP_write(0x00, HVSP_WRITE_LFUSE_INSTR4);
    break;

  case HFUSE_SEL:    
    HVSP_write(HVSP_WRITE_HFUSE_DATA, HVSP_WRITE_HFUSE_INSTR1);
    HVSP_write(fuse, HVSP_WRITE_HFUSE_INSTR2);
    HVSP_write(0x00, HVSP_WRITE_HFUSE_INSTR3);
    HVSP_write(0x00, HVSP_WRITE_HFUSE_INSTR4);
    break;
    
  case EFUSE_SEL:
    HVSP_write(HVSP_WRITE_EFUSE_DATA, HVSP_WRITE_EFUSE_INSTR1);
    HVSP_write(fuse, HVSP_WRITE_EFUSE_INSTR2);
    HVSP_write(0x00, HVSP_WRITE_EFUSE_INSTR3);
    HVSP_write(0x00, HVSP_WRITE_EFUSE_INSTR4);
    break;

  case LOCK_SEL:
    HVSP_write(HVSP_WRITE_LOCK_DATA, HVSP_WRITE_LOCK_INSTR1);
    HVSP_write(fuse, HVSP_WRITE_LOCK_INSTR2);
    HVSP_write(0x00, HVSP_WRITE_LOCK_INSTR3);
    HVSP_write(0x00, HVSP_WRITE_LOCK_INSTR4);
    break;
  }
  waitForHigh(SDO); 
  return;
}


byte readHVPPFuse(int mode, int select) {
  byte fuse;
  
  sendHVPPCmdOrAddr(mode,true,B00000100);  // Send command to read fuse bits
  
  // Configure DATA as input so we can read back fuse values from target
  setData(0);
  setDataDirection(INPUT);

  // Set control lines
  switch (select) {
  case LFUSE_SEL:  
      // Read LFUSE
      digitalWrite(BS2, LOW);
      digitalWrite(BS1, LOW);
      break;
  case HFUSE_SEL:
      // Read HFUSE
      digitalWrite(BS2, HIGH);
      digitalWrite(BS1, HIGH);
      break;
  case EFUSE_SEL:
      // Read EFUSE
      digitalWrite(BS2, HIGH);
      digitalWrite(BS1, LOW);
      break;
  case LOCK_SEL:
      // Read LOCK
      digitalWrite(BS2, LOW);
      digitalWrite(BS1, HIGH);
      break;    
  }
  
  //  Read fuse
  digitalWrite(OE, LOW);
  delay(1);
  
  fuse = getData();
  
  digitalWrite(OE, HIGH);  // Done reading, disable output enable line
  return fuse;
}

byte readHVSPFuse(int select) {
  byte fuse;
  
  switch (select) {

  case LFUSE_SEL:
    HVSP_read(HVSP_READ_LFUSE_DATA, HVSP_READ_LFUSE_INSTR1);
    HVSP_read(0x00, HVSP_READ_LFUSE_INSTR2);
    fuse=HVSP_read(0x00, HVSP_READ_LFUSE_INSTR3);
    break;

  case HFUSE_SEL:
    HVSP_read(HVSP_READ_HFUSE_DATA, HVSP_READ_HFUSE_INSTR1);
    HVSP_read(0x00, HVSP_READ_HFUSE_INSTR2);
    fuse=HVSP_read(0x00, HVSP_READ_HFUSE_INSTR3);
    break;

  case EFUSE_SEL:
    HVSP_read(HVSP_READ_EFUSE_DATA, HVSP_READ_EFUSE_INSTR1);
    HVSP_read(0x00, HVSP_READ_EFUSE_INSTR2);
    fuse=HVSP_read(0x00, HVSP_READ_EFUSE_INSTR3);
    break;

  case LOCK_SEL:
    HVSP_read(HVSP_READ_LOCK_DATA, HVSP_READ_LOCK_INSTR1);
    HVSP_read(0x00, HVSP_READ_LOCK_INSTR2);
    fuse=HVSP_read(0x00, HVSP_READ_LOCK_INSTR3);
    break;
  }
  return fuse;
}

unsigned long readHVPPSig(int mode) {

  unsigned long result = 0;

  // Configure DATA as input so we can read back fuse values from target
  setData(0);
  setDataDirection(INPUT);
  for (byte i=0;i<3;i++) {
    sendHVPPCmdOrAddr(mode,true,B00001000);
    sendHVPPCmdOrAddr(mode,false,i);
    digitalWrite(BS1,LOW);
    digitalWrite(OE, LOW);
    delay(1);
    result = (result<<8)+getData();
    digitalWrite(OE, HIGH); 
  }
  return result;
}


unsigned long readHVSPSig() {

  unsigned long result = 0;

  for (byte i=0;i<3;i++) {
    HVSP_read(HVSP_READ_SIG_DATA, HVSP_READ_SIG_INSTR1);
    HVSP_read(i, HVSP_READ_SIG_INSTR2);
    HVSP_read(0x00, HVSP_READ_SIG_INSTR3);
    result = (result<<8) + HVSP_read(0x00, HVSP_READ_SIG_INSTR4);
  }
  return result;
}


byte HVSP_read(byte data, byte instr) { // Read a byte using the HVSP protocol

  byte response = 0x00; // a place to hold the response from target

  digitalWrite(SCI, LOW);  // set clock low
  // 1st bit is always zero
  digitalWrite(SDI, LOW);
  digitalWrite(SII, LOW);
  sclk();
  
  // We capture a response on every readm even though only certain responses contain
  // valid data.  For fuses, the valid response is captured on the 3rd instruction write.
  // It is up to the program calling this function to figure out which response is valid.
  
  // The MSB of the response byte comes "early", that is, 
  // before the 1st non-zero-padded byte of the 3rd instruction is sent to the target.
  // For more information, see the ATtiny25/45/85 datasheet, Table 20-16 (pg. 164).
  if (digitalRead(SDO) == HIGH) // target sent back a '1'?
    response |= 0x80;  // set MSB of response byte high
  
  // Send each bit of data and instruction byte serially, MSB first
  // I do this by shifting the byte left 1 bit at a time and checking the value of the new MSB
  for (int i=0; i<8; i++) {  // i is bit number
    if ((data << i) & 0x80)  // shift data byte left and check if new MSB is 1 or 0
      digitalWrite(SDI, HIGH);  // bit was 1, set pin high
    else
      digitalWrite(SDI, LOW);   // bit was 0, set pin low
      
    if ((instr << i) & 0x80)   // same process for instruction byte
      digitalWrite(SII, HIGH);
    else
      digitalWrite(SII, LOW);
   sclk();
       
    if (i < 7) {  // remaining 7 bits of response are read here (one at a time)
      // note that i is one less than the bit position of response we are reading, since we read
      // the MSB above.  That's why I shift 0x40 right and not 0x80. 
      if(digitalRead(SDO) == HIGH)  // if we get a logic 1 from target,
        response |= (0x40 >> i);    // set corresponding bit of response to 1
    }
  }
  
  // Last 2 bits are always zero
  for (int i=0; i<2; i++) {
    digitalWrite(SDI, LOW);
    digitalWrite(SII, LOW);
    sclk();
  }
  
  return response;
}

void HVSP_write(byte data, byte instr) { // Write to target using the HVSP protocol

  digitalWrite(SCI, LOW);  // set clock low
  
  // 1st bit is always zero
  digitalWrite(SDI, LOW);
  digitalWrite(SII, LOW);
  sclk();  // latch bit
  
  // Send each bit of data and instruction byte serially, MSB first
  // I do this by shifting the byte left 1 bit at a time and checking the value of the new MSB
  for (int i=0; i<8; i++) {  // i is bit number
    if ((data << i) & 0x80)  // shift data byte left and check if new MSB is 1 or 0
      digitalWrite(SDI, HIGH);  // bit was 1, set pin high
    else
      digitalWrite(SDI, LOW);   // bit was 0, set pin low
      
    if ((instr << i) & 0x80)  // same process for instruction byte
      digitalWrite(SII, HIGH);
    else
      digitalWrite(SII, LOW);
      
   sclk();  // strobe SCI (serial clock) to latch data
  }
  
  // Last 2 bits are always zero
  for (int i=0; i<2; i++) {
    digitalWrite(SDI, LOW);
    digitalWrite(SII, LOW);   
    sclk();
  }
}

void sendHVPPCmdOrAddr(int mode, boolean is_cmd, byte cmd_or_addr)  // Send command to target AVR
{
  // Set controls for command mode
  if (is_cmd) digitalWrite(XA1, HIGH);
  else   digitalWrite(XA1,LOW);
  digitalWrite(XA0, LOW);
  digitalWrite(BS1, LOW);  
  
  if (mode != TINY2313)
    digitalWrite(BS2, LOW);  // Command load seems not to work if BS2 is high 
  
  setData(cmd_or_addr);
  setDataDirection(OUTPUT);
  
  strobe_xtal();  // latch DATA
  
  setData(0);
  setDataDirection(INPUT);
  digitalWrite(XA1, LOW);
}

void waitForHigh(byte signal) {
  for (int i=0; i<1000; i++) {
    if (digitalRead(signal) == HIGH) return;
    delay(1);
  }
  Serial.println(F("Timeout during write operation"));
}


void sclk(void) {  // send serial clock pulse, used by HVSP commands

  // These delays are much  longer than the minimum requirements,
  // but we don't really care about speed.
  delay(1);  
  digitalWrite(SCI, HIGH);
  delay(1);
  digitalWrite(SCI, LOW);
}

void strobe_xtal(void) {  // strobe xtal (usually to latch data on the bus)
  
  delay(1);
  digitalWrite(XTAL1, HIGH);  // pulse XTAL to send command to target
  delay(1);
  digitalWrite(XTAL1, LOW);
}

void setDataDirection(int dir) {
#ifdef FBD_MODE
  if (dir == INPUT) DDRB = 0;
  else DDRB = 0xFF;
#else
  for (byte pin = 2; pin < 10; pin++) pinMode(pin,dir);
#endif
}

byte getData(void) {
#ifdef FBD_MODE
  return PINB;
#else
  byte res = 0;
  for (byte pin = 2; pin < 10; pin++) 
    res = (res<<1)+digitalRead(pin);
  return(res);
#endif
}
    
void setData(byte data) {
#ifdef FBD_MODE
  PORTB = data;
#else
  for (byte pin = 2; pin < 10; pin++) {
    digitalWrite(pin,((data&B10000000) != 0));
    data = data << 1;
  }
#endif
}



int askByte(void) {  // get new byte value
  char serbuffer[2];
  byte i = 0;
  char next;
  
  while (i <= 2) {
    while (Serial.available() == 0);   // wait for a character to come in
    next = Serial.read();
    if (i <= 2 && (next == '\b' || next == '\x7F')) {
      if (i > 0) {
        Serial.print("\b \b");
        i--;
      }
    } else {
      if (i == 2) {
        if (next == ' ' || next == '\r' || next == '\n') break;
        else { 
          Serial.println(F("\nInvalid end-of-line character"));
          return -1;
        }
      } else {
        if (next >= 'a' && next <= 'f') next = toupper(next);
        if ((next >= '0' && next <= '9') || (next >= 'A' && next <= 'F')) {
          serbuffer[i++] = next;
          Serial.print(next);
        } else {
          Serial.println(F("Invalid hexadecimal character"));
          return -1;
        }
      }
    }
  }
  Serial.println();
  return(hex2dec(serbuffer[1]) + hex2dec(serbuffer[0]) * 16);
}


int hex2dec(byte c) { // converts one HEX character into a number
  if (c >= '0' && c <= '9') {
    return c - '0';
  }
  else if (c >= 'A' && c <= 'F') {
    return c - 'A' + 10;
  } 
  else return 0;
}

void printFuses(char *str, byte num, byte l, byte h, byte e) {
  Serial.print(str);
  switch (num) {
  case 1: Serial.print(F("Fuse byte:   ")); break;
  case 2: Serial.print(F("L/H-Fuses:   ")); break;
  case 3: Serial.print(F("L/H/E-Fuses: ")); break;
  }
  print2Hex(l,true);
  if (num > 1) print2Hex(h,true);
  if (num > 2) print2Hex(e,true);
  Serial.println();
}

void printLock(byte l) {
  Serial.print(F("Current lock byte:   "));
  print2Hex(l,true);
  Serial.println();
}

void print2Hex(byte val, boolean blank) 
{ // print two hex digits
  if (val < 0x10) Serial.print('0');
  Serial.print(val,HEX);
  if (blank) Serial.print(' ');
}

void showLed(boolean red, boolean green, unsigned int time) 
{
#ifdef FBD_MODE
  if (red) digitalWrite(XA0,HIGH);
  if (green) digitalWrite(ORIGXA1,HIGH);
#endif
  delay(time);
#ifdef FBD_MODE
  digitalWrite(XA0,LOW);
  digitalWrite(ORIGXA1,LOW);
#endif
}

void ledBlink(byte color,byte sec) {
  
#ifdef FBD_MODE
  for (int i=0; i < 10*sec; i++) {
    if (color == RED) digitalWrite(XA0,HIGH);
    else digitalWrite(XA1,HIGH);
    delay(50);
    digitalWrite(XA0,LOW);
    digitalWrite(XA1,LOW);
    delay(50);
  }
#endif
}
