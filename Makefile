include include/*.mk

build:
	pio run --environment esp32dev_serial

upload:
	pio run --environment esp32dev_serial -t upload

upload_ota:
	mdns_name=$(MDNS_NAME) ota_password=$(OTA_PASSWORD) pio run --environment esp32dev_ota -t upload

monitor:
	pio device monitor

clean:
	rm -fR .pio/
