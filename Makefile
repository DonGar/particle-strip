
# Id of device to push.
CORE_ID ?= Benchtop

.PHONY: default compile push flash wifi clean

default: compile

firmware.bin: firmware/*
	# This returns success, even on failure.
	particle compile firmware  --saveTo firmware.bin

compile: firmware.bin

push: clean compile
	# Clean first, to force an error out on failure.
	particle flash ${CORE_ID} firmware.bin

flash:
	sudo particle flash --usb firmware.bin

wifi:
	sudo particle setup wifi

clean:
	rm -f firmware.bin
