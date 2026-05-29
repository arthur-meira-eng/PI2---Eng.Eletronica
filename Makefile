ARDUINO_CLI ?= arduino-cli
FQBN ?= esp32:esp32:esp32
CORE ?= esp32:esp32
SKETCH ?= PI2
BUILD_DIR ?= build/esp32
PORT ?= $(shell ls /dev/cu.usbserial* /dev/cu.wchusbserial* /dev/cu.SLAB_USBtoUART* /dev/cu.usbmodem* 2>/dev/null | head -n 1)
BAUD ?= 115200
UPLOAD_SPEED ?= 115200

.PHONY: all update-index core libs install compile check-port upload monitor ports clean

all: compile

update-index:
	$(ARDUINO_CLI) core update-index
	$(ARDUINO_CLI) lib update-index

core: update-index
	$(ARDUINO_CLI) core install $(CORE)

libs: update-index
	$(ARDUINO_CLI) lib install "Adafruit MPU6050"
	$(ARDUINO_CLI) lib install "Adafruit Unified Sensor"
	$(ARDUINO_CLI) lib install "Adafruit ADS1X15"
	$(ARDUINO_CLI) lib install "Adafruit BusIO"
	$(ARDUINO_CLI) lib install "MFRC522"
	$(ARDUINO_CLI) lib install "VL53L0X"

install: core libs

compile:
	$(ARDUINO_CLI) compile --fqbn $(FQBN) --build-path $(BUILD_DIR) $(SKETCH)

check-port:
	@if [ -z "$(PORT)" ]; then \
		echo "Nenhuma porta USB serial encontrada no macOS."; \
		echo "Conecte o ESP32 ou informe manualmente: make upload PORT=/dev/cu.usbserial-10"; \
		$(ARDUINO_CLI) board list; \
		exit 1; \
	fi

upload: check-port compile
	$(ARDUINO_CLI) upload --fqbn $(FQBN) --port $(PORT) --input-dir $(BUILD_DIR) --upload-property upload.speed=$(UPLOAD_SPEED) $(SKETCH)

monitor: check-port
	$(ARDUINO_CLI) monitor --port $(PORT) --config baudrate=$(BAUD) --config dtr=off --config rts=off

ports:
	$(ARDUINO_CLI) board list

clean:
	rm -rf $(BUILD_DIR)
