You can use this sketch in order to try to resurrect AVR chips with
wrong fuses using an Arduino and a breadboard, or you can use it as an
alternative firmware for manekinen's Fusebit Doctor PCB (see
http://mdiy.pl/atmega-fusebit-doctor-hvpp/?lang=en). The functionality
is very similar. However, it is Open Source (under the GPL) and can be
freely modified and extended.

1. Using the sketch on an Arduino with a breadboard
=================================

I have tried out the sketch with an ATtiny84, ATtiny85, and an
ATmega328. I have included Fritzing wiring sketches for these three
chips. For other chips, you have to consult the data sheet of the
particular microprocessor. 

Be careful with the 12 Volt! If you feed it into the wrong pin of your
chip, it will probably be immediately gone!

The transistors, I have used are fairly standard once. You can use
probably any other reasonable type. But make sure that the pins are
ordered as in the picture, i.e. CBE.

Once you have set everything up and loaded the sketch into the Arduino,
just open the Arduino Serial Monitor. The Arduino will reset and will
try to read the signature of the chip. If successful, the sketch will
tell you the name of the chip, and you can perform different
actions. Most probably R (= resurrection) is, what you want to
choose. If the sketch tell you "No chip found!", then either your chip
is really dead, or you made a wiring mistake (much more likely!).


2. Installing and using the software an a Fusebit-Doctor board
=======================================

You can install the firmware by simply uploading the HEX file to an
ATmega328(P) and deactivating the fuse CKDIV8, so that the chip runs
on 8MHz. Now you are all set.

You can either use the board interactively or stand alone, as with the
original firmware.

Stand alone mode: After power-up, all LEDs are off and you can insert
a chip. After pressing the button, the board will first try to
recognize the chip. If successful, the green LED will be on for 3
secs, if unsuccessful (chip unknown or completely broken), the red
light will be on for 3 secs. In the rare case that the chip is known,
but we do not know the default fuses, first the green LED is on for 1
second, then the red LED is on for 3 seconds. After that, the board
tries to reset all lock bits and then tries to set the fuses to a safe
default setting. If successful, the green LED blinks, otherwise the
red LED blinks. If unsuccessful, you can try to set the erase jumper,
which allows for erasing the entire chip in order to recover it.

Interactive mode: Set your terminal to 19200 baud, no parity, 1
stop-bit. The board tries to recognize the chip after pressing the
button and then allows you to interactively change any fuse or lock,
to reset the fuses to its default value, or to erase the chip. If the
chip is unknown, it will still recognize a chip signature. If you know
how many fuse bytes the chip has, you can specify that and continue
from there in order to manually change the fuses to desirable values
(look into the manual!). If the board cannot detect any chip, you
still can try to send the commands to the chip. Specify the protocol:
HVPP for all ATmega chips, HVPP for Tiny for all MCUs from the
ATtiny2313 and ATtiny26(1) family, and HVSP for all the other ATtiny
chips. 

If you want to change the firmware, you can do this using the Arduino
IDE. In order to do so, you first need to install a bootloader. Since
the ATmega is running on 8 MHz, you need to use the bootloader
ATmegaBOOT_168_atmega328_pro_8MHz.hex (for your ATmega328P) or
ATmegaBOOT_168_pro_8MHz.hex (if you use an ATmega168). You also should
set the fuse BOOTRST (enabling the boot reset vector) and set the boot
loader size to 2048 bytes in the fuses. Now you can put the chip onto
the fusebit doctor board and connect via a serial line to your
computer. Once you have registered this serial line with your Arduino
IDE and have chosen "Arduino Pro or Pro Mini (3.3V 8MHz) w/ ATmega328"
as the board you want to work with, you can start to download sketches
to the board. Note that you always have to press the reset button in
order to enable the transfer of the program from your computer to the
chip. The timing is critical here. Press the button immediately after
the compiler has finished its work!



