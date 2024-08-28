#!/usr/bin/env python3
import os
import sys
import re

def genMcuLine(desc):
    hv = re.search('<interface.*?type="hv',desc)
    if not hv: return ""
    name = re.search('<device.*?name="(.*?)"',desc).group(1)
    sig1 = re.search('<property name="SIGNATURE1" value="0x(.*?)"',desc).group(1)
    sig2 = re.search('<property name="SIGNATURE2" value="0x(.*?)"',desc).group(1)
    low = re.search('<register.*?name="LOW".*?initval="0x(.*?)"',desc).group(1)
    high = re.search('<register.*?name="HIGH".*?initval="0x(.*?)"',desc)
    high = high.group(1) if high else ""
    ext = re.search('<register.*?name="EXTENDED".*?initval="0x(.*?)"',desc)
    ext = ext.group(1) if ext else ""
    extnum = ext if ext else "00"
    highnum = high if high else "00"
    firstbyte = "0x0" + str(len(low + high + ext)//2) 
    return [ name, sig1 + sig2, "    { 0x" + sig1 + sig2 + ", " + firstbyte + low + ", 0x" + highnum + extnum + ", (uint16_t)" + name.lower() + " },", name.lower() ]


if len(sys.argv) > 1:
    paths = sys.argv[1:]
else:
    paths = [ '/Applications/microchip/mplabx/v6.20/packs/Microchip/ATmega_DFP/3.1.264/atdf/',
                  '/Applications/microchip/mplabx/v6.20/packs/Microchip/ATtiny_DFP/3.1.260/atdf/']

mlist = list()
for path in paths:
    for name in os.listdir(path):
        if os.path.isfile(os.path.join(path,name)):
            with open(os.path.join(path,name)) as f:
                entry = genMcuLine(f.read())
                if entry:
                    mlist = mlist + [entry]

sigs = list()
mlist = sorted(mlist)
for mcu in mlist:
    if not mcu in sigs:
        sigs += [mcu[1]]
        for variant in mlist:
            if variant != mcu and variant[1] == mcu[1]:
                mcu[0] = mcu[0] + "/" + variant[0]
    
sigs = list()
for row in mlist:
    if not row[1] in sigs:
        sigs += [row[1]]
        print('const char ' + row[3].lower() + '[] PROGMEM = "' + row[0] + '";')

sigs = list()
for row in mlist:
    if not row[1] in sigs:
        sigs += [row[1]]
        print(row[2])

