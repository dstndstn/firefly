##

# Internal calibrated RC oscillator -- 8 MHz
# - fuse CKSEL 3:0 == 0010
# - start-up timer SUT 1:0 = 10 default
# - ^ckdiv8 = 0 default

# - default fuse low bits = x62
# - want to toggle ckdiv8 => xe2

# To hit RESET:
#  avrdude -p m168 -c bsd -E noreset

# To set fuses:
#  avrdude -p m168 -c bsd -U lfuse:w:0xe2:m

## mp3box:
# (run 'make ardheaders.tgz', copy to mp3box and tar xzf);
#  make ARD_DIR=.

F_CPU := 8000000UL
MCU_TARGET     := atmega168

HEX := firefly.hex

all: $(HEX) firefly.lst

CC := avr-gcc
CXX := avr-g++
OBJCOPY        = avr-objcopy
OBJDUMP        = avr-objdump

ARD_DIR := /Applications/Arduino.app/Contents/Java/hardware/arduino/avr/

CFLAGS := -O2 -DF_CPU=$(F_CPU) -Iv-usb/usbdrv -I. -mmcu=$(MCU_TARGET) \
	-DARDUINO=100 \
	-I$(ARD_DIR)/cores/arduino \
	-I$(ARD_DIR)/variants/standard \
	-DSTANDALONE=1

NEOPIXEL := Adafruit_NeoPixel.o

CAPTOUCH := CapacitiveSensor.o

ARDOBJS := wiring.o wiring_digital.o hooks.o WMath.o

MAP := firefly.map

firefly.elf: firefly.o $(NEOPIXEL) $(CAPTOUCH) $(ARDOBJS)
	$(CC) $(CFLAGS) -Wl,-Map,$(MAP) -o $@ $^

ARD_H := \
	cores/arduino/Arduino.h \
	cores/arduino/binary.h \
	cores/arduino/WCharacter.h \
	cores/arduino/WString.h \
	cores/arduino/HardwareSerial.h \
	cores/arduino/Stream.h \
	cores/arduino/Print.h \
	cores/arduino/Printable.h \
	cores/arduino/USBAPI.h \
	cores/arduino/wiring_private.h \
	variants/standard/pins_arduino.h

ardheaders.tgz: $(addprefix $(ARD_DIR), $(ARD_H))
	tar czf $@ -C $(ARD_DIR) $(ARD_H)

program:
	avrdude -p m168 -c bsd -U flash:w:$(HEX)
.PHONY: program

reset:
	avrdude -p m168 -c bsd -E noreset
.PHONY: reset

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

%.o: %.cpp
	$(CXX) $(CFLAGS) -c $< -o $@

%.o: %.S
	$(CC) $(CFLAGS) -c $< -o $@

%.lst: %.elf
	$(OBJDUMP) -h -S $< > $@

%.hex: %.elf
	$(OBJCOPY) -j .text -j .data -O ihex $< $@

clean:
	rm *.elf *.o *.hex *.lst *.map $(USB)
