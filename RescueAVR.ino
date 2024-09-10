// -*- mode: c++; tab-width: 4; indent-tabs-mode: nil -*-

//  Title:        RescueAVR

#define VERSION  "3.0.2"

/*Copyright 2013-2021 by Bernhard Nebel and parts are copyrighted by Jeff Keyzer.
  License: GPLv3

  It uses the hardware designed by manekinen, but the software is completely
  new, using ideas und snippets from  Jeff Keyzer's sketch HVRescue_Shield.
  In addition, the program can also be run on an Arduino Uno, Nano, or Pro.

  The program can be either compiled for Arduino Uno, Nano, or Pro, Mega, or Leonardo, 
  or for Fusebit Doctor boards (in all other cases). If you want to compile it for the
  Fusebit Doctor board, I recommend to use the MiniCore.
 
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

Version 2.3 (21.1.2021)
  - added: support for ATtiny1634
  - fixed: corrected the code for reading the OSCCAL value
  - added: 'R - Restart' mow in all menus 
  - changed: resurrect to 'T')

Version 2.3.1 (25.1.2022)
  - added: some explanantion
  - changed: made compile mode dependend on target board specification 
  - fixed a few warnings
  - added: Github actions

Version 2.4.0 (20.8.2024)
  - added a few MCUs: ATtiny22, ATtiny43U, ATtiny87, ATtiny167, ATmega48PB, 
    ATmega88PB, ATmega168PB, ATmega328PB,
    ATmega640, ATmega1280, ATmega1281, ATmega2560, ATmega2561, ATmega8U2, ATmega16U2, 
    ATmega32U2, ATmega16U4, ATmega32U4,
    AT90S2333, ATtiny441, ATtiny841, ATtiny828
  - renamed: ATMEGA -> HVPP and TINY2313 -> TINYHVPP
  - added: ARDUINO_AVR_PRO is now in the list of boards that can execute 
    the Arduino version of the RescueAVR sketch 

Version 2.5.0 (28.8.2024)
  - fixed: handling of MCUs that read lock and fuse bits into one byte 
    (some of the very obsolete MCUs). These have now a marker in the number of
    of fuse bytes field at bit 7 in the MCU table.
  - fixed: use direct printing of MCU name in printMCUName instead of going via
    a buffer (which was too small and led to a buffer overrun)
  - fixed: for ATTiny11, ATtiny12, compare only the LB1 and LB2 bits of the 
    lock byte when verifying after resurrection!
  - fixed: give error message when selecting 'H' or 'X' and there are not enough fuse bytes
  - added: all missing currently supported MCUs after running extract.py on the ATDF files
  - added: a few more obsolete MCUs, e.g., AT90S1200, ATmega161, ...
  - added: restart option when asking for number of fuse bytes
  - removed: attempting to clear lock bits because this always fails! 

Version 2.6.0 (18.8.2024)
  - fixed: handling of MCUs that read fuse and lock bits in one byte (AT90S2323 etc.)
  - fixed: ATtiny11 & 12 handling 

Version 2.7.0 (8.9.2024)
  - added Arduino Mega as a possible host platform

Version 3.0.0 (9.9.2024)
  - changed: program logic does not require software resets any longer
  - changed: "setting xxx fuse" -> "new xxx fuse value"
  - added: when using the Arduino monitor window, you can now write the new byte value
    immediately after the command character
  - added: Leonardo as host board
  - fixed: questions for fuse bytes does now appear only once

Version 3.0.1 (9.9.2024)
  - changed: request to 'restart' instead of 'reset' 

Version 3.0.2 (10.9.2024)
  - fixed: for AT90S8515 and AT90S2313, it seems necessary to use multiple attempts to write the lock byte
  - fixed: for the AT90S chips, longer WR pulses are necessary for trhe erase function 
*/



/* Uncomment one of the following two lines, if you do not want to
   leave the decision on which board the sketch is executed to the
   specification of the target board
*/
// #define FBD_MODE
// #define ARDUINO_MODE
//#ifdef ARDUINO_AVR_LEONARDO
//   #error "Arduino Leonardo is not supported"
//#endif

#if !defined(FBD_MODE) && !defined(ARDUINO_MODE)
  #if !defined(ARDUINO_AVR_UNO) && !defined(ARDUINO_AVR_NANO) && !defined(ARDUINO_AVR_PRO) && \
    !defined(ARDUINO_AVR_MEGA) && !defined(ARDUINO_AVR_MEGA2560) && !defined(ARDUINO_AVR_LEONARDO) && \
    !defined(ARDUINO_SAMD_ZERO)
    #define FBD_MODE
  #else
    #define ARDUINO_MODE
  #endif
#endif


#include <avr/pgmspace.h>

// communication speed 
#define  BAUD         19200    // Serial port rate at which to talk to PC


// Names of a few obsolete MCUs supporting HV prog
const char at90pwm2[] PROGMEM = "AT90PWM2/3";
const char at90s1200[] PROGMEM = "AT90S1200";
const char at90s2313[] PROGMEM = "AT90S2313";
const char at90s2323[] PROGMEM = "AT90S2323";
const char at90s2333[] PROGMEM = "AT90S2333";
const char at90s2343[] PROGMEM = "AT90S2343";
const char at90s4414[] PROGMEM = "AT90S4414";
const char at90s4433[] PROGMEM = "AT90S4433";
const char at90s4434[] PROGMEM = "AT90S4434";
const char at90s8515[] PROGMEM = "AT90S8515";
const char at90s8535[] PROGMEM = "AT90S8535";
const char atmega103[] PROGMEM = "ATmega103";
const char atmega161[] PROGMEM = "ATmega161";
const char atmega163[] PROGMEM = "ATmega163";
const char atmega323[] PROGMEM = "ATmega323";
const char attiny22[] PROGMEM = "ATtiny22";
const char attiny28[] PROGMEM = "ATtiny28";

// Names of supported MCUs as extracted by extract.py
// Do not change manually!
const char at90can128[] PROGMEM = "AT90CAN128";
const char at90can32[] PROGMEM = "AT90CAN32";
const char at90can64[] PROGMEM = "AT90CAN64";
const char at90pwm1[] PROGMEM = "AT90PWM1/AT90PWM2B/AT90PWM3B";
const char at90pwm161[] PROGMEM = "AT90PWM161";
const char at90pwm216[] PROGMEM = "AT90PWM216/AT90PWM316";
const char at90pwm81[] PROGMEM = "AT90PWM81";
const char at90usb1286[] PROGMEM = "AT90USB1286/AT90USB1287";
const char at90usb162[] PROGMEM = "AT90USB162";
const char at90usb646[] PROGMEM = "AT90USB646/AT90USB647";
const char at90usb82[] PROGMEM = "AT90USB82";
const char atmega128[] PROGMEM = "ATmega128/ATmega128A";
const char atmega1280[] PROGMEM = "ATmega1280";
const char atmega1281[] PROGMEM = "ATmega1281";
const char atmega1284[] PROGMEM = "ATmega1284";
const char atmega1284p[] PROGMEM = "ATmega1284P";
const char atmega1284rfr2[] PROGMEM = "ATmega1284RFR2";
const char atmega128rfa1[] PROGMEM = "ATmega128RFA1";
const char atmega128rfr2[] PROGMEM = "ATmega128RFR2";
const char atmega16[] PROGMEM = "ATmega16/ATmega16A";
const char atmega162[] PROGMEM = "ATmega162";
const char atmega164a[] PROGMEM = "ATmega164A";
const char atmega164p[] PROGMEM = "ATmega164P/ATmega164PA";
const char atmega165a[] PROGMEM = "ATmega165A";
const char atmega165p[] PROGMEM = "ATmega165P/ATmega165PA";
const char atmega168[] PROGMEM = "ATmega168/ATmega168A";
const char atmega168p[] PROGMEM = "ATmega168P/ATmega168PA";
const char atmega168pb[] PROGMEM = "ATmega168PB";
const char atmega169a[] PROGMEM = "ATmega169A";
const char atmega169p[] PROGMEM = "ATmega169P/ATmega169PA";
const char atmega16hva[] PROGMEM = "ATmega16HVA";
const char atmega16hvb[] PROGMEM = "ATmega16HVB/ATmega16HVBrevB";
const char atmega16m1[] PROGMEM = "ATmega16M1";
const char atmega16u2[] PROGMEM = "ATmega16U2";
const char atmega16u4[] PROGMEM = "ATmega16U4";
const char atmega2560[] PROGMEM = "ATmega2560";
const char atmega2561[] PROGMEM = "ATmega2561";
const char atmega2564rfr2[] PROGMEM = "ATmega2564RFR2";
const char atmega256rfr2[] PROGMEM = "ATmega256RFR2";
const char atmega32[] PROGMEM = "ATmega32/ATmega32A";
const char atmega324a[] PROGMEM = "ATmega324A";
const char atmega324p[] PROGMEM = "ATmega324P";
const char atmega324pa[] PROGMEM = "ATmega324PA";
const char atmega324pb[] PROGMEM = "ATmega324PB";
const char atmega325[] PROGMEM = "ATmega325/ATmega325A";
const char atmega3250[] PROGMEM = "ATmega3250/ATmega3250A";
const char atmega3250p[] PROGMEM = "ATmega3250P/ATmega3250PA";
const char atmega325p[] PROGMEM = "ATmega325P/ATmega325PA";
const char atmega328[] PROGMEM = "ATmega328";
const char atmega328p[] PROGMEM = "ATmega328P";
const char atmega328pb[] PROGMEM = "ATmega328PB";
const char atmega329[] PROGMEM = "ATmega329/ATmega329A";
const char atmega3290[] PROGMEM = "ATmega3290/ATmega3290A";
const char atmega3290p[] PROGMEM = "ATmega3290P/ATmega3290PA";
const char atmega329p[] PROGMEM = "ATmega329P/ATmega329PA";
const char atmega32c1[] PROGMEM = "ATmega32C1";
const char atmega32hvb[] PROGMEM = "ATmega32HVB/ATmega32HVBrevB";
const char atmega32m1[] PROGMEM = "ATmega32M1";
const char atmega32u2[] PROGMEM = "ATmega32U2";
const char atmega32u4[] PROGMEM = "ATmega32U4";
const char atmega406[] PROGMEM = "ATmega406";
const char atmega48[] PROGMEM = "ATmega48/ATmega48A";
const char atmega48p[] PROGMEM = "ATmega48P/ATmega48PA";
const char atmega48pb[] PROGMEM = "ATmega48PB";
const char atmega64[] PROGMEM = "ATmega64/ATmega64A";
const char atmega640[] PROGMEM = "ATmega640";
const char atmega644[] PROGMEM = "ATmega644/ATmega644A";
const char atmega644p[] PROGMEM = "ATmega644P/ATmega644PA";
const char atmega644rfr2[] PROGMEM = "ATmega644RFR2";
const char atmega645[] PROGMEM = "ATmega645/ATmega645A";
const char atmega6450[] PROGMEM = "ATmega6450/ATmega6450A";
const char atmega6450p[] PROGMEM = "ATmega6450P";
const char atmega645p[] PROGMEM = "ATmega645P";
const char atmega649[] PROGMEM = "ATmega649/ATmega649A";
const char atmega6490[] PROGMEM = "ATmega6490/ATmega6490A";
const char atmega6490p[] PROGMEM = "ATmega6490P";
const char atmega649p[] PROGMEM = "ATmega649P";
const char atmega64c1[] PROGMEM = "ATmega64C1";
const char atmega64hve2[] PROGMEM = "ATmega64HVE2";
const char atmega64m1[] PROGMEM = "ATmega64M1";
const char atmega64rfr2[] PROGMEM = "ATmega64RFR2";
const char atmega8[] PROGMEM = "ATmega8/ATmega8A";
const char atmega8515[] PROGMEM = "ATmega8515";
const char atmega8535[] PROGMEM = "ATmega8535";
const char atmega88[] PROGMEM = "ATmega88/ATmega88A";
const char atmega88p[] PROGMEM = "ATmega88P/ATmega88PA";
const char atmega88pb[] PROGMEM = "ATmega88PB";
const char atmega8hva[] PROGMEM = "ATmega8HVA";
const char atmega8u2[] PROGMEM = "ATmega8U2";
const char attiny11[] PROGMEM = "ATtiny11";
const char attiny12[] PROGMEM = "ATtiny12";
const char attiny13[] PROGMEM = "ATtiny13/ATtiny13A";
const char attiny15[] PROGMEM = "ATtiny15";
const char attiny1634[] PROGMEM = "ATtiny1634";
const char attiny167[] PROGMEM = "ATtiny167";
const char attiny2313[] PROGMEM = "ATtiny2313/ATtiny2313A";
const char attiny24[] PROGMEM = "ATtiny24/ATtiny24A";
const char attiny25[] PROGMEM = "ATtiny25";
const char attiny26[] PROGMEM = "ATtiny26";
const char attiny261[] PROGMEM = "ATtiny261/ATtiny261A";
const char attiny4313[] PROGMEM = "ATtiny4313";
const char attiny43u[] PROGMEM = "ATtiny43U";
const char attiny44[] PROGMEM = "ATtiny44/ATtiny44A";
const char attiny441[] PROGMEM = "ATtiny441";
const char attiny45[] PROGMEM = "ATtiny45";
const char attiny461[] PROGMEM = "ATtiny461/ATtiny461A";
const char attiny48[] PROGMEM = "ATtiny48";
const char attiny828[] PROGMEM = "ATtiny828";
const char attiny84[] PROGMEM = "ATtiny84/ATtiny84A";
const char attiny841[] PROGMEM = "ATtiny841";
const char attiny85[] PROGMEM = "ATtiny85";
const char attiny861[] PROGMEM = "ATtiny861/ATtiny861A";
const char attiny87[] PROGMEM = "ATtiny87";
const char attiny88[] PROGMEM = "ATtiny88";



// table with signatures
// 1st word signature (w/o 1E),
// 2nd word: MSB: bit7=read lock byte for lock+fuses
//                bit0-bit3=number of fuses
// 2nd word LSB: low fuse,
// 3rd word MSB: high fuse,
// 3rd word LSB: extended fuse,
// 4th word: string address for MCU name

#define MCU_NUM (17+114)
const uint16_t mcu_types[MCU_NUM][4] PROGMEM =
  { // manually inserted obsolete MCUs
    { 0x9381, 0x0362, 0xDFF9, (uint16_t)at90pwm2 },
    { 0x9001, 0x81DF, 0x0000, (uint16_t)at90s1200 },
    { 0x9101, 0x81DE, 0x0000, (uint16_t)at90s2313 },
    { 0x9102, 0x81DF, 0x0000, (uint16_t)at90s2323 },
    { 0x9105, 0x01DA, 0x0000, (uint16_t)at90s2333 },
    { 0x9103, 0x81DE, 0x0000, (uint16_t)at90s2343 },
    { 0x9201, 0x81DF, 0x0000, (uint16_t)at90s4414 },
    { 0x9203, 0x01DA, 0x0000, (uint16_t)at90s4433 },
    { 0x9202, 0x81DF, 0x0000, (uint16_t)at90s4434 },
    { 0x9301, 0x81DF, 0x0000, (uint16_t)at90s8515 },
    { 0x9303, 0x81DF, 0x0000, (uint16_t)at90s8535 },
    { 0x9701, 0x03C1, 0xFFFF, (uint16_t)atmega103 },
    { 0x9401, 0x01D2, 0x0000, (uint16_t)atmega161 },
    { 0x9402, 0x02F2, 0xF900, (uint16_t)atmega163 },
    { 0x9501, 0x02F2, 0xF900, (uint16_t)atmega323 },
    { 0x9106, 0x81DE, 0x0000, (uint16_t)attiny22 },
    { 0x9107, 0x01F2, 0x0000, (uint16_t)attiny28 },
    // automatically extracted MCUs
    { 0x9781, 0x0362, 0x99FF, (uint16_t)at90can128 },
    { 0x9581, 0x0362, 0x99FF, (uint16_t)at90can32 },
    { 0x9681, 0x0362, 0x99FF, (uint16_t)at90can64 },
    { 0x9383, 0x0362, 0xDFF9, (uint16_t)at90pwm1 },
    { 0x948B, 0x0362, 0xD9FD, (uint16_t)at90pwm161 },
    { 0x9483, 0x0362, 0xDFF9, (uint16_t)at90pwm216 },
    { 0x9388, 0x0362, 0xD9FD, (uint16_t)at90pwm81 },
    { 0x9782, 0x035E, 0x99F3, (uint16_t)at90usb1286 },
    { 0x9482, 0x035E, 0xD9F4, (uint16_t)at90usb162 },
    { 0x9682, 0x035E, 0x9BF3, (uint16_t)at90usb646 },
    { 0x9382, 0x035E, 0xD9F4, (uint16_t)at90usb82 },
    { 0x9702, 0x03E1, 0x99FD, (uint16_t)atmega128 },
    { 0x9703, 0x0362, 0x99FF, (uint16_t)atmega1280 },
    { 0x9704, 0x0362, 0x99FF, (uint16_t)atmega1281 },
    { 0x9706, 0x0362, 0x99FF, (uint16_t)atmega1284 },
    { 0x9705, 0x0362, 0x99FF, (uint16_t)atmega1284p },
    { 0xa703, 0x0362, 0x99FE, (uint16_t)atmega1284rfr2 },
    { 0xa701, 0x0362, 0x99FF, (uint16_t)atmega128rfa1 },
    { 0xa702, 0x0362, 0x99FE, (uint16_t)atmega128rfr2 },
    { 0x9403, 0x02E1, 0x9900, (uint16_t)atmega16 },
    { 0x9404, 0x0362, 0x99FF, (uint16_t)atmega162 },
    { 0x940f, 0x0362, 0x99FF, (uint16_t)atmega164a },
    { 0x940a, 0x0362, 0x99FF, (uint16_t)atmega164p },
    { 0x9410, 0x0362, 0x99FF, (uint16_t)atmega165a },
    { 0x9407, 0x0362, 0x99FF, (uint16_t)atmega165p },
    { 0x9406, 0x0362, 0xDFF9, (uint16_t)atmega168 },
    { 0x940b, 0x0362, 0xDFF9, (uint16_t)atmega168p },
    { 0x9415, 0x0362, 0xDFF9, (uint16_t)atmega168pb },
    { 0x9411, 0x0362, 0x99FF, (uint16_t)atmega169a },
    { 0x9405, 0x0362, 0x99FF, (uint16_t)atmega169p },
    { 0x940c, 0x01DF, 0x0000, (uint16_t)atmega16hva },
    { 0x940d, 0x02DD, 0xE900, (uint16_t)atmega16hvb },
    { 0x9484, 0x0362, 0xD9FF, (uint16_t)atmega16m1 },
    { 0x9489, 0x035E, 0xD9F4, (uint16_t)atmega16u2 },
    { 0x9488, 0x0352, 0x99FB, (uint16_t)atmega16u4 },
    { 0x9801, 0x0362, 0x99FF, (uint16_t)atmega2560 },
    { 0x9802, 0x0362, 0x99FF, (uint16_t)atmega2561 },
    { 0xa803, 0x0362, 0x99FE, (uint16_t)atmega2564rfr2 },
    { 0xa802, 0x0362, 0x99FE, (uint16_t)atmega256rfr2 },
    { 0x9502, 0x02E1, 0x9900, (uint16_t)atmega32 },
    { 0x9515, 0x0362, 0x99FF, (uint16_t)atmega324a },
    { 0x9508, 0x0362, 0x99FF, (uint16_t)atmega324p },
    { 0x9511, 0x0362, 0x99FF, (uint16_t)atmega324pa },
    { 0x9517, 0x0362, 0x9907, (uint16_t)atmega324pb },
    { 0x9505, 0x0362, 0x99FF, (uint16_t)atmega325 },
    { 0x9506, 0x0362, 0x99FF, (uint16_t)atmega3250 },
    { 0x950e, 0x0362, 0x99FF, (uint16_t)atmega3250p },
    { 0x950d, 0x0362, 0x99FF, (uint16_t)atmega325p },
    { 0x9514, 0x0362, 0xD9FF, (uint16_t)atmega328 },
    { 0x950f, 0x0362, 0xD9FF, (uint16_t)atmega328p },
    { 0x9516, 0x0362, 0xD9F7, (uint16_t)atmega328pb },
    { 0x9503, 0x0362, 0x99FF, (uint16_t)atmega329 },
    { 0x9504, 0x0362, 0x99FF, (uint16_t)atmega3290 },
    { 0x950c, 0x0362, 0x99FF, (uint16_t)atmega3290p },
    { 0x950b, 0x0362, 0x99FF, (uint16_t)atmega329p },
    { 0x9586, 0x0362, 0xD9FF, (uint16_t)atmega32c1 },
    { 0x9510, 0x02DD, 0xE900, (uint16_t)atmega32hvb },
    { 0x9584, 0x0362, 0xD9FF, (uint16_t)atmega32m1 },
    { 0x958a, 0x035E, 0xD9F4, (uint16_t)atmega32u2 },
    { 0x9587, 0x0352, 0x99FB, (uint16_t)atmega32u4 },
    { 0x9507, 0x02CD, 0xFE00, (uint16_t)atmega406 },
    { 0x9205, 0x0362, 0xDFFF, (uint16_t)atmega48 },
    { 0x920a, 0x0362, 0xDFFF, (uint16_t)atmega48p },
    { 0x9210, 0x0362, 0xDFFF, (uint16_t)atmega48pb },
    { 0x9602, 0x03E1, 0x99FD, (uint16_t)atmega64 },
    { 0x9608, 0x0362, 0x99FF, (uint16_t)atmega640 },
    { 0x9609, 0x0362, 0x99FF, (uint16_t)atmega644 },
    { 0x960a, 0x0362, 0x99FF, (uint16_t)atmega644p },
    { 0xa603, 0x0362, 0x99FE, (uint16_t)atmega644rfr2 },
    { 0x9605, 0x0362, 0x99FF, (uint16_t)atmega645 },
    { 0x9606, 0x0362, 0x99FF, (uint16_t)atmega6450 },
    { 0x960e, 0x0362, 0x99FF, (uint16_t)atmega6450p },
    { 0x960D, 0x0362, 0x99FF, (uint16_t)atmega645p },
    { 0x9603, 0x0362, 0x99FF, (uint16_t)atmega649 },
    { 0x9604, 0x0362, 0x99FF, (uint16_t)atmega6490 },
    { 0x960C, 0x0362, 0x99FF, (uint16_t)atmega6490p },
    { 0x960b, 0x0362, 0x99FF, (uint16_t)atmega649p },
    { 0x9686, 0x0362, 0xD9FF, (uint16_t)atmega64c1 },
    { 0x9610, 0x02D7, 0xF900, (uint16_t)atmega64hve2 },
    { 0x9684, 0x0362, 0xD9FF, (uint16_t)atmega64m1 },
    { 0xa602, 0x0362, 0x99FE, (uint16_t)atmega64rfr2 },
    { 0x9307, 0x02E1, 0xD900, (uint16_t)atmega8 },
    { 0x9306, 0x02E1, 0xD900, (uint16_t)atmega8515 },
    { 0x9308, 0x02E1, 0xD900, (uint16_t)atmega8535 },
    { 0x930a, 0x0362, 0xDFF9, (uint16_t)atmega88 },
    { 0x930f, 0x0362, 0xDFF9, (uint16_t)atmega88p },
    { 0x9316, 0x0362, 0xDFF9, (uint16_t)atmega88pb },
    { 0x9310, 0x01DF, 0x0000, (uint16_t)atmega8hva },
    { 0x9389, 0x035E, 0xD9F4, (uint16_t)atmega8u2 },
    { 0x9004, 0x01F4, 0x0000, (uint16_t)attiny11 },
    { 0x9005, 0x0152, 0x0000, (uint16_t)attiny12 },
    { 0x9007, 0x026A, 0xFF00, (uint16_t)attiny13 },
    { 0x9006, 0x015C, 0x0000, (uint16_t)attiny15 },
    { 0x9412, 0x0362, 0xDFFF, (uint16_t)attiny1634 },
    { 0x9487, 0x0362, 0xDFFF, (uint16_t)attiny167 },
    { 0x910a, 0x0364, 0xDFFF, (uint16_t)attiny2313 },
    { 0x910b, 0x0362, 0xDFFF, (uint16_t)attiny24 },
    { 0x9108, 0x0362, 0xDFFF, (uint16_t)attiny25 },
    { 0x9109, 0x02E1, 0xF700, (uint16_t)attiny26 },
    { 0x910c, 0x0362, 0xDFFF, (uint16_t)attiny261 },
    { 0x920d, 0x0362, 0xDFFF, (uint16_t)attiny4313 },
    { 0x920c, 0x0362, 0xDFFF, (uint16_t)attiny43u },
    { 0x9207, 0x0362, 0xDFFF, (uint16_t)attiny44 },
    { 0x9215, 0x0362, 0xDFFF, (uint16_t)attiny441 },
    { 0x9206, 0x0362, 0xDFFF, (uint16_t)attiny45 },
    { 0x9208, 0x0362, 0xDFFF, (uint16_t)attiny461 },
    { 0x9209, 0x0362, 0xDFFF, (uint16_t)attiny48 },
    { 0x9314, 0x036E, 0xDFFF, (uint16_t)attiny828 },
    { 0x930c, 0x0362, 0xDFFF, (uint16_t)attiny84 },
    { 0x9315, 0x0362, 0xDFFF, (uint16_t)attiny841 },
    { 0x930b, 0x0362, 0xDFFF, (uint16_t)attiny85 },
    { 0x930d, 0x0362, 0xDFFF, (uint16_t)attiny861 },
    { 0x9387, 0x0362, 0xDFFF, (uint16_t)attiny87 },
    { 0x9311, 0x0362, 0xDFFF, (uint16_t)attiny88 },
  };


#ifdef FBD_MODE
/* pin assignment for the FBD board */
/************************************/
// data lines are the PORTB lines
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
// data lines are pins 2 to 9
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
// Note: not all ATtiny's have an HFUSE
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

// OSCCAL read
#define HVSP_READ_OSC_DATA       B00001000
#define HVSP_READ_OSC_INSTR1     B01001100
#define HVSP_READ_OSC_INSTR2     B00001100
#define HVSP_READ_OSC_INSTR3     B01111000
#define HVSP_READ_OSC_INSTR4     B01111100

// Erase chip
#define HVSP_ERASE_CHIP_DATA     B10000000
#define HVSP_ERASE_CHIP_INSTR1   B01001100
#define HVSP_ERASE_CHIP_INSTR2   B01100100
#define HVSP_ERASE_CHIP_INSTR3   B01101100


// Internal definitions
enum { HVPP, TINYHVPP, HVSP };
enum { LFUSE_SEL, HFUSE_SEL, EFUSE_SEL, LOCK_SEL };
enum { RED, GREEN };

// Global variables
byte mcu_mode;  // programming mode
byte mcu_fuses = 0; // number of fuses for the actual chip
boolean fuse_and_lock = false; // true when lock and fuse bits are together
boolean interactive_mode = true; // interaction over terminal
int mcu_index = -1; // index into mcu_types array - see above
unsigned long mcu_signature = 0; // signature as read from chip
boolean ec_allowed = true; // whether chip erase is allowed - only relevant for FBD

void setup() { // run once, when the sketch starts
  
#ifdef FBD_MODE
  pinMode(ECJUMPER, INPUT_PULLUP); // the jumper that allows chip erase
  if (digitalRead(0) == LOW)
  interactive_mode = false; // no RS232 connection!
#endif
  delay(2000);
  Serial.begin(BAUD);  // Open serial port
  while (!Serial); // wait for serial line on Leaonardo to come up
  Serial.println();
  Serial.println();
  Serial.print(F("RescueAVR Version "));
  Serial.print(VERSION);
  if (interactive_mode) Serial.print(F(" / interactive, "));
  else Serial.print(F(" / non-interactive, "));
#ifdef FBD_MODE
  Serial.println(F("running on Fusebit-Doctor Board"));
#else
  Serial.println(F("running on Arduino board"));
#endif
  Serial.println();
}

boolean startup(void) {
  char modec = ' ';

  XA1 = ORIGXA1;
  PAGEL = ORIGPAGEL;
  setData(0);
  setDataDirection(INPUT);
  pinMode(RST, OUTPUT);  // 12V Reset signal
  digitalWrite(RST, LOW);  // Turn off 12V 
  pinMode(VCC, OUTPUT);
  digitalWrite(VCC, VCCOFF); // Turn off VCC
  pinMode(RDY, INPUT);
  digitalWrite(RDY, LOW);
  pinMode(OE, OUTPUT);
  digitalWrite(OE, LOW);
  pinMode(WR, OUTPUT);
  digitalWrite(WR, LOW);
  pinMode(BS1, OUTPUT);
  digitalWrite(BS1, LOW);
  pinMode(XA0, OUTPUT);
  digitalWrite(XA0, LOW);
  pinMode(XA1, OUTPUT);
  digitalWrite(XA1, LOW);
  pinMode(PAGEL, OUTPUT);
  digitalWrite(PAGEL, LOW);
  pinMode(BS2, OUTPUT);
  digitalWrite(BS2, LOW);
  pinMode(XTAL1, OUTPUT);
  digitalWrite(XTAL1, LOW);
  
  Serial.println();
  for (mcu_mode = HVPP; mcu_mode <= HVSP; mcu_mode++) {
    enterHVProgMode(mcu_mode);
    mcu_signature = readSig(mcu_mode);
    leaveHVProgMode(mcu_mode);
    // Serial.print(F("DEBUG: SIG="));
    // Serial.println(mcu_signature,HEX);
    if ((mcu_signature>>16) == 0x1E && // start of signature and 
        mcu_signature != 0x1E1E1E) // not the default value on the data bus
      break;
  }
  if (mcu_mode > HVSP) {
    Serial.println(F("No chip found!"));
    Serial.println(F("Insert chip and restart or give details."));
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
      Serial.println(F("  R - Restart"));
      Serial.print(F("Choice: "));
      while (!Serial.available())  { };
      modec = toupper(Serial.read());
      Serial.println(modec);
      while (Serial.available()) Serial.read();
      if (modec == 'P' || modec == 'T' || modec == 'S' || modec == 'R') break;
      Serial.print("'");
      Serial.print(modec);
      Serial.println(F("' is not a valid choice."));
    }
    switch (modec) {
    case 'P': mcu_mode = HVPP; break;
    case 'T': mcu_mode = TINYHVPP; break;
    case 'S': mcu_mode = HVSP; break;
    case 'R': return false; break;
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
        showLed(false,true,1000); // 1 second green
      }
      printHVLine();
      mcu_index = -1;
    }
  }
  if (mcu_index < 0 ) {
    Serial.println(F("Unknown"));
    printHVLine();
    if (!interactive_mode) {
      showLed(true,false,3000); // 3 secs of red
      while (true) { };
    } else {
      mcu_fuses = 0;
      delay(10);
      while (Serial.available()) Serial.read();
      while (mcu_fuses < 1 || mcu_fuses > 3) {
        Serial.print(F("# of fuse bytes (1-3), R to abort: "));
        while (!Serial.available())  { };
        mcu_fuses = Serial.read();
        Serial.println(mcu_fuses);
        Serial.println((char)mcu_fuses);
        if (toupper(mcu_fuses) == 'R') {
          return false;
        }
        while (Serial.available()) Serial.read();
        mcu_fuses = mcu_fuses - '0';
      }
    }
  } else {
    printMCUName(mcu_index);
    Serial.println();
    printHVLine();
    mcu_fuses = (pgm_read_word(&(mcu_types[mcu_index][1]))>>8);
    fuse_and_lock = ((mcu_fuses & 0x80) != 0);
    mcu_fuses = mcu_fuses & 0x0F;
    if (!interactive_mode) showLed(false,true,3000); // 3 secs of green
  }
  return true;
}

void loop() {  // run over and over again
  
  byte hfuse = 0, lfuse = 0, efuse = 0, lock = 0, osccal = 0;  // fuse and lock values
  byte dhfuse = 0, dlfuse = 0, defuse = 0;  // default values
  char action = ' ';

  while (!startup());
  
  if (mcu_index >= 0) {
    dlfuse = pgm_read_word(&(mcu_types[mcu_index][1]))&0xFF;
    dhfuse = pgm_read_word(&(mcu_types[mcu_index][2]))>>8;
    defuse = pgm_read_word(&(mcu_types[mcu_index][2]))&0xFF;
  }

  enterHVProgMode(mcu_mode);
  

  lfuse = readFuse(mcu_mode,LFUSE_SEL,fuse_and_lock);
  if (mcu_fuses > 1)
    hfuse = readFuse(mcu_mode,HFUSE_SEL,fuse_and_lock);
  if (mcu_fuses > 2)
    efuse = readFuse(mcu_mode,EFUSE_SEL,fuse_and_lock);
  lock = readFuse(mcu_mode,LOCK_SEL,fuse_and_lock);
  osccal = readOSCCAL(mcu_mode);
  leaveHVProgMode(mcu_mode);

  printFuses("Current ",mcu_fuses,lfuse,hfuse,efuse);
  if (mcu_index >= 0) printFuses("Default ",mcu_fuses,dlfuse,dhfuse,defuse);

  printLock(lock);
  printOSCCAL(osccal);
 

  if (interactive_mode) {
    while (true) {
      while (Serial.available()) Serial.read();
      Serial.println();
      Serial.println(F("Choose:"));
      if (mcu_index >= 0) Serial.println(F("  T - Try to resurrect chip by all means"));
      Serial.println(F("  E - Erase chip"));
      if (mcu_index >= 0) Serial.println(F("  D - Burn default fuse values"));
      Serial.println(F("  L - Change low fuse"));
      if (mcu_fuses > 1) Serial.println(F("  H - Change high fuse"));
      if (mcu_fuses > 2) Serial.println(F("  X - Change extended fuse"));
      Serial.println(F("  K - Change lock byte"));
      Serial.println(F("  R - Restart"));
      Serial.print(F("Action: "));
      
      while (!Serial.available()) { };
      action = toupper(Serial.read());
      Serial.println(action);
      
      if (action == 'E' || action == 'D' || action == 'L' ||
          (action == 'H' && mcu_fuses > 1) ||
          (action == 'X' && mcu_fuses > 2) ||
          action == 'K' || action == 'T' || action == 'R') break;
      if (action >= ' ') {
        Serial.print("'");
        Serial.print(action);
        Serial.println("' is not a valid command char.");
      }
    }
  
    enterHVProgMode(mcu_mode);
    switch (action) {
    case 'R':
      return;
      break;
    case 'T':
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
        lfuse = readFuse(mcu_mode,LFUSE_SEL,fuse_and_lock);
        if (mcu_fuses > 1) hfuse = readFuse(mcu_mode,HFUSE_SEL,fuse_and_lock);
        if (mcu_fuses > 2) efuse = readFuse(mcu_mode,EFUSE_SEL,fuse_and_lock);
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
    case 'L': setAndVerifyOneByte(LFUSE_SEL, "low fuse value");
      break;
    case 'H': 
      if (mcu_fuses > 1) {
        setAndVerifyOneByte(HFUSE_SEL, "high fuse value");
      }
      break;
    case 'X':
      if (mcu_fuses > 2) {
        setAndVerifyOneByte(EFUSE_SEL, "extended fuse value");
      }
      break;
    case 'K':
      setAndVerifyOneByte(LOCK_SEL, "lock byte");
      break;
    }
    Serial.println();
    leaveHVProgMode(mcu_mode);
  } else { // non-interactive mode
    resurrection(dlfuse, dhfuse, defuse);
    Serial.println(F("Bye, bye ..."));
    while (true) { };
  }
}

void resurrection(byte dlf, byte dhf, byte def) {
  byte lf = 0, hf = 0, ef = 0, lk = 0;
  
  Serial.println(F("Start HV programming mode..."));
  enterHVProgMode(mcu_mode);
  showLed(true,true,400);
  
  // remove lock by erasing chip - if necessary and allowed
  lk = readFuse(mcu_mode, LOCK_SEL,fuse_and_lock);
#ifdef FBD_MODE
  ec_allowed = (digitalRead(ECJUMPER) == LOW);
#endif
  if (ec_allowed &&
      (lk != 0xFF &&
       (mcu_signature != 0x1E9005) &&  // if not ATtiny12, 
       (mcu_signature != 0x1E9004)) ||  // and not ATTiny11
      ((lk&0x06 ) != 0x06)) {
    Serial.println(F("Lock byte needs to be cleared. Erase chip ..."));
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
  lk = readFuse(mcu_mode, LOCK_SEL,fuse_and_lock);
  lf = readFuse(mcu_mode,LFUSE_SEL,fuse_and_lock);
  if (mcu_fuses > 1) hf = readFuse(mcu_mode,HFUSE_SEL,fuse_and_lock);
  if (mcu_fuses > 2) ef = readFuse(mcu_mode,EFUSE_SEL,fuse_and_lock);
  
  leaveHVProgMode(mcu_mode);
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
                  ef,def) ||
      (((mcu_signature == 0x1E9005) || // if ATtiny12, 
        (mcu_signature == 0x1E9004)) &&  // or ATTiny11
       ((lk&0x06 ) == 0x06) && // if LB1 & LB2 are unprogrammed 
       verifyFuses(mcu_fuses,
                   0xFF,0xFF,
                   lf,dlf,
                   hf,dhf,
                   ef,def))) {
    Serial.println(F("-> Resurrection was successful!"));
    ledBlink(GREEN,5);
  } else {
    Serial.println(F("-> Resurrection FAILED!"));
    if (!ec_allowed) 
      Serial.println(F("Try again with 'erase chip' jumper set"));
    ledBlink(RED,5);
  }
}


void setAndVerifyOneByte(int SEL, const char* mess) {
  int toburn;
  char testchar;
  boolean directbyte = false;

  if (Serial.available()) {
    testchar = toupper(Serial.peek());
    if ((testchar >= '0' && testchar <= '9') || (testchar >= 'A' && testchar <= 'F'))
      directbyte = true;
  }
  if (!directbyte) {
    while (Serial.available()) Serial.read();
  }
  Serial.print(F("New "));
  Serial.print(mess);
  Serial.print(F(": "));
  toburn = askByte();
  if (toburn >= 0x00) {
    Serial.print(F("\nSetting "));
    Serial.print(mess);
    Serial.print(F(" ... "));
    burnFuse(mcu_mode, toburn, SEL);
    verifyOneByte(SEL, toburn, mess);
  }
}

void verifyOneByte(int SEL, byte desired, const char* mess) {
  byte newval;

  showLed(true,true,600);
  Serial.println(F(" done"));
  delay(200);
  Serial.print(F("Verifying ... "));
  newval = readFuse(mcu_mode,SEL,fuse_and_lock);
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
  Serial.print((const __FlashStringHelper *)  pgm_read_word(&mcu_types[ix][3]));
}

void printHVLine(void) {
  Serial.print(F("HV type:   "));
  switch (mcu_mode) {
  case HVPP: Serial.println(F("HVPP")); break;
  case TINYHVPP: Serial.println(F("HVPP for Tiny")); break;
  case HVSP: Serial.println(F("HVSP")); break;
  default: Serial.println(F("Unknown")); break;
  }
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

byte readOSCCAL(int mode) {
  if (mode == HVSP) return readHVSPOSCCAL();
  else return readHVPPOSCCAL(mode);
}



byte readFuse(int mode, int selection, boolean read_fuse_and_lock) {
  int locsel = selection;
  byte fuseval, tempval = 0;
  if (read_fuse_and_lock) selection = LOCK_SEL;
  if (mode == HVSP) fuseval = readHVSPFuse(selection);
  else fuseval = readHVPPFuse(mode,selection);
  if (selection == LFUSE_SEL && mcu_signature == 0x1E9004) // read fuse of ATtiny11
    return (fuseval | 0x80);
  if (!read_fuse_and_lock) return fuseval;
  else {
    if (locsel == LFUSE_SEL) return (fuseval | 0xC0);
    else
      if (locsel == LOCK_SEL) { 
        if (fuseval & 0x80) tempval |= 0x02;
        if (fuseval & 0x40) tempval |= 0x04;
        return (tempval | 0xF9);
      } else return 0xFF;
  }
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
  if (mode == TINYHVPP) {
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

void leaveHVProgMode(int mode) {
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
  delay(10); //usually 1 is enough, but the older S90 nee longer!
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


void burnHVPPFuse(int mode, byte fuse, int select)  // write extended, high or low fuse to AVR
{
  for (uint8_t t=0; t < 3; t++) {
    // for some of the AT90S chips, multiple attempts appear to be necessary!
    
    // Send command to enable fuse programming mode
    if (select == LOCK_SEL)   sendHVPPCmdOrAddr(mode,true,B00100000);  
    else sendHVPPCmdOrAddr(mode,true,B01000000);  
    
    // Enable data loading
    digitalWrite(XA1, LOW);
    digitalWrite(XA0, HIGH);
    // Specify low byte
    digitalWrite(BS1, LOW); 
    if (mode != TINYHVPP)
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
    delayMicroseconds(1600);
    digitalWrite(WR, HIGH);
    delay(10);
  
    waitForHigh(RDY); // when RDY goes high, we are done
  
    // Reset control lines to original state
    digitalWrite(BS1, LOW);
    digitalWrite(BS2, LOW);
    delay(20);
  }
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
  
  sendHVPPCmdOrAddr(mode,true,B00000100);  // Send command to read fuseand lock  bits
  
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
  byte fuse = 0;
  
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


byte readHVPPOSCCAL(int mode) {
  byte result;

  setData(0);
  setDataDirection(INPUT);
  sendHVPPCmdOrAddr(mode,true,B00001000);
  sendHVPPCmdOrAddr(mode,false,0);
  digitalWrite(BS1,HIGH);
  digitalWrite(OE, LOW);
  delay(1);
  result = getData();
  digitalWrite(OE, HIGH); 
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

byte readHVSPOSCCAL() {
  HVSP_read(HVSP_READ_OSC_DATA, HVSP_READ_OSC_INSTR1);
  HVSP_read(0x00, HVSP_READ_OSC_INSTR2);
  HVSP_read(0x00, HVSP_READ_OSC_INSTR3);
  return HVSP_read(0x00, HVSP_READ_OSC_INSTR4);
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
  
  if (mode != TINYHVPP)
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

void printFuses(const char *str, byte num, byte l, byte h, byte e) {
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

void printOSCCAL(byte l) {
  Serial.print(F("Oscillator calibr.:  "));
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
  (void)red; (void)green; (void)time;
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
  (void)sec; (void)color;
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
