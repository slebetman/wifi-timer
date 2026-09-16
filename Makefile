WEMOS_D1_MINI_BOARD = esp8266:esp8266:d1_mini
WEMOS_D1_MINI_PORT = /dev/ttyUSB0

ESP32C3_SUPERMINI_BOARD = esp32:esp32:esp32c3:CDCOnBoot=cdc
ESP32C3_SUPERMINI_PORT = /dev/ttyACM0

all:
	arduino-cli compile -v --fqbn $(WEMOS_D1_MINI_BOARD)

d1-mini: all
	arduino-cli compile -v --fqbn $(WEMOS_D1_MINI_BOARD)

c3-supermini:
	arduino-cli compile -v --fqbn $(ESP32C3_SUPERMINI_BOARD)

upload-d1-mini: d1-mini
	arduino-cli upload -v -p $(WEMOS_D1_MINI_PORT) --fqbn $(WEMOS_D1_MINI_BOARD)

upload-c3-supermini:
	arduino-cli upload -v -p $(ESP32C3_SUPERMINI_PORT) --fqbn $(ESP32C3_SUPERMINI_BOARD)

monitor-d1-mini:
	picocom -b 115200 --lower-rts --lower-dtr $(WEMOS_D1_MINI_PORT)

monitor-c3-supermini:
	picocom -b 115200 --lower-rts --lower-dtr $(ESP32C3_SUPERMINI_PORT)
