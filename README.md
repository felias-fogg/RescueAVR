

# RescueAVR

[![License: GPL v3](https://img.shields.io/badge/License-GPLv3-blue.svg)](https://www.gnu.org/licenses/gpl-3.0)
![Hit Counter](https://visitor-badge.laobi.icu/badge?page_id=felias-fogg_RescueAVR)
[![Build Status](https://github.com/felias-fogg/RescueAVR/workflows/Build/badge.svg)](https://github.com/felias-fogg/RescuAVR/actions)

This sketch can resurrect many classic AVR chips with wrong fuse settings using high-voltage (HV) programming. 

Sometimes, you may erroneously set a fuse bit, such as the clock source bit, and then the chip does not respond to ISP programming anymore. In this case, HV programming can help. One can easily reset all fuses to their factory setting, and then ISP programming is possible again. Note that the sketch does not implement a general HV programmer but can only perform some basic tasks, such as fuse setting and erasing the entire chip. Furthermore, you should not apply this technique "in-system" without pre-cautions. HV programming means that you apply 12 V to the RESET pin. So, if you do that "in-system," you should make sure that the RESET line is not connected to the rest of the system (e.g., by a pull-up resistor). 

To do HV programming, you need an Arduino Uno, Nano, or Pro Mini, a breadboard, a PNP/NPN transistor pair, a few resistors, many jumper wires, and an external regulated 12-volt supply. If you are the happy owner of a *RecueAVR shield* for an Arduino Uno, everything is already included and set up, including the 12 V source. You can build such a shield using the KiCAD design files in the [PCB directory](pcb/) or buy the PCBs or a kit at Tindie. The [assembly process](pcb/assembly.md) is straightforward and involves only THT components.

Furthermore, the sketch is also an alternative firmware for [manekinen's Fusebit Doctor](https://web.archive.org/web/20180225102717/http://mdiy.pl/atmega-fusebit-doctor-hvpp/?lang=en). The pin mapping is different between these two versions. When the sketch is compiled for an Arduino Uno, Nano, or Pro Mini in the Arduino IDE, it will use the Arduino Uno pin mapping. Otherwise, it uses the pin mapping for the Fusebit Doctor. One can also force which version is produced by defining the compile-time constants  `ARDUINO_MODE` or `FBD_MODE`, respectively.

When using the sketch, remember to set the monitor baud rate to 19200 baud (no parity, 1 stop-bit). A [user manual](docs/manual.md) for the sketch is provided in the docs folder.

The sketch uses many of the ideas and code of [MightyOhm's HV Rescue Shield 2](https://mightyohm.com/blog/products/hv-rescue-shield-2-x/) and is inspired by [manekinen's Fusebit Doctor](https://web.archive.org/web/20180225102717/http://mdiy.pl/atmega-fusebit-doctor-hvpp/?lang=en). 

