
# Id of device to push.
CORE_ID ?= Benchtop

default: compile

compile:
	particle compile firmware  --saveTo firmware.bin

push: compile
	particle flash ${CORE_ID} firmware.bin

flash:
	sudo particle flash --usb firmware.bin

wifi:
	sudo particle setup wifi

clean:
	rm firmware.bin
