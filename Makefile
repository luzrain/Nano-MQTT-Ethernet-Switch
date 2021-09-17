.PHONY: build upload clean flash
	
PROGRAMMER = stk500	
SERIAL_PORT = COM3

build:
	arduino-cli compile -b atmega:avr:atmega328:cpu=atmega328p,clock=external16 -e --output-dir build

bootloader:
	arduino-cli burn-bootloader -b atmega:avr:atmega328:cpu=atmega328p,clock=external16 --port $(SERIAL_PORT) --programmer $(PROGRAMMER)

upload:
	arduino-cli upload -b atmega:avr:atmega328:cpu=atmega328p,clock=external16 --port $(SERIAL_PORT) --programmer $(PROGRAMMER)

clean:
	rm -rf build

flash: build bootloader upload
