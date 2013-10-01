This Arduino sketch is alterative firmware for manekinen's Fusebit
Doctor (see http://mdiy.pl/atmega-fusebit-doctor-hvpp/?lang=en). The
functionality is very similar. However, it is Open Source (under the
GPL) and in particular it corrects a bug that prevents the original
firmware to recognize chips from the ATmega8 family once the RSTENBL
or DWEN fuse has been activated.

You can install the firmware by simply uploading the HEX file to an
ATmega328(P) and deactivating the fuse CKDIV8, so that the chip runs
on 8MHz. Now you are all set.

You can either use the board interactively or stand alone. 

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
chip is unknown, it will still recogize a chip signature. If you know
how many fuse bytes the chip has, you can specify that and continue
from there in order to manually change the fuses to desirable values
(look into the manual!). If the board cannot detect any chip, you
still can try to send the commands to the chip. Specify the protocol:
HVPP for all ATmega chips, HVPP for Tiny for all MCU from the
ATtiny2313 and ATtiny26(1) family, and HVSP for all the other ATtiny
chips. 

If you want to to change the firmware, you cando this using the
Arduino IDE. In order to do so, you first need to install a
bootloader. Since the ATmega is running on 8 MHz, you need to use the
bootloader ATmegaBOOT_168_atmega328_pro_8MHz.hex (for your ATmega328P)
or ATmegaBOOT_168_pro_8MHz.hex (if you use an ATmega168). You also
should set the fuse BOOTRST (enabling the boot reset vector) and set
the boot loader size to 2048 bytes in the fuses. Now you can put the
chip onto the fusebit doctor board and connect via a serial line to
your computer. Once you have registered this serial line with your
Arduino IDE and have chosen "Arduino Pro or Pro Mini (3.3V 8MHz) w/
ATmega328", you can start to download sketches to the board. Note that
you always have to press the reset button in order to enable the
transfer of the program from your computer to the chip. The timing is
critical here. Press the button immediately after the compiler has
finished its work!


