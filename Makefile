ARDUINO_CLI ?= arduino-cli
FQBN ?= esp32:esp32:esp32
SKETCH ?= PI2
BUILD_DIR ?= build/esp32
PORT ?= /dev/ttyUSB0
BAUD ?= 115200

.PHONY: all compile upload monitor ports clean

all: compile

compile:
	$(ARDUINO_CLI) compile --fqbn $(FQBN) --build-path $(BUILD_DIR) $(SKETCH)

upload: compile
	$(ARDUINO_CLI) upload --fqbn $(FQBN) --port $(PORT) --input-dir $(BUILD_DIR) $(SKETCH)

monitor:
	$(ARDUINO_CLI) monitor --port $(PORT) --config baudrate=$(BAUD) --config dtr=off --config rts=off

ports:
	$(ARDUINO_CLI) board list

clean:
	rm -rf $(BUILD_DIR)
