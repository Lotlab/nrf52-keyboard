OUTPUT_DIR = _build

CC := sdcc
CFLAGS := -mmcs51 --model-small --std-c11 -o $(OUTPUT_DIR)/ --opt-code-size
LDFLAGS := --xram-size 1024 --iram-size 256 --code-size 16384 --out-fmt-ihx

LIBS = $(OUTPUT_DIR)/system.rel \
		$(OUTPUT_DIR)/uart.rel \
		$(OUTPUT_DIR)/app_timer.rel \
		$(OUTPUT_DIR)/descriptor.rel \
		$(OUTPUT_DIR)/endpoints.rel \
		$(OUTPUT_DIR)/interrupt.rel

OUTPUT_IHX := $(OUTPUT_DIR)/main.ihx
OUTPUT_HEX := $(OUTPUT_DIR)/main.hex
OUTPUT_BIN := $(OUTPUT_DIR)/main.bin

$(OUTPUT_HEX): $(OUTPUT_IHX)
	packihx $(OUTPUT_IHX) > $(OUTPUT_HEX)

$(OUTPUT_BIN): $(OUTPUT_IHX)
	makebin -p $(OUTPUT_IHX) $(OUTPUT_BIN)

$(OUTPUT_IHX): main.c $(LIBS)
	mkdir -p $(OUTPUT_DIR)
	$(CC) $(LIBS) main.c $(CFLAGS) $(LDFLAGS) -o $(OUTPUT_IHX)

$(OUTPUT_DIR)/system.rel: system.c
	mkdir -p $(OUTPUT_DIR)
	$(CC) -c system.c $(CFLAGS)

$(OUTPUT_DIR)/uart.rel: uart.c
	$(CC) -c uart.c $(CFLAGS)

$(OUTPUT_DIR)/app_timer.rel: app_timer.c
	$(CC) -c app_timer.c $(CFLAGS)

$(OUTPUT_DIR)/descriptor.rel: descriptor.c
	$(CC) -c descriptor.c $(CFLAGS)

$(OUTPUT_DIR)/endpoints.rel: endpoints.c
	$(CC) -c endpoints.c $(CFLAGS)

$(OUTPUT_DIR)/interrupt.rel: interrupt.c
	$(CC) -c interrupt.c $(CFLAGS)

.PHONY:clean
clean:
	rm -r $(OUTPUT_DIR)

flash: $(OUTPUT_BIN)
	wchisptool -f $(OUTPUT_BIN)