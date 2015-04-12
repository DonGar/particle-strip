
# Id of device to push.
CORE_ID ?= Benchtop

default: compile

compile:
	spark compile firmware  --saveTo firmware.bin

push:
	spark flash ${CORE_ID} .

flash:
	sudo spark flash --usb firmware.bin

wifi:
	sudo spark setup wifi

clean:
	rm firmware.bin
