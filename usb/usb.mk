PROJ_NAME := ch554
OUTPUT_DIR = _build
OBJ_DIR = $(OUTPUT_DIR)/$(PROJ_NAME)

ifndef USB_SOURCE_DIR
USB_SOURCE_DIR := ./
endif

SDCC := sdcc
SDCC_CFLAGS := -mmcs51 --model-small --std-c11 -o $(OBJ_DIR)/ --opt-code-size -I .
SDCC_LDFLAGS := --xram-size 1024 --iram-size 256 --code-size 16384 --out-fmt-ihx

LIBS := $(patsubst $(USB_SOURCE_DIR)/%.c,$(OBJ_DIR)/%.rel,$(wildcard $(USB_SOURCE_DIR)/*.c))

OUTPUT_IHX := $(OBJ_DIR)/$(PROJ_NAME).ihx
OUTPUT_HEX := $(OUTPUT_DIR)/$(PROJ_NAME).hex
OUTPUT_BIN := $(OUTPUT_DIR)/$(PROJ_NAME).bin

.PHONY:ch554, usb

usb: ch554
ch554: $(OUTPUT_HEX) $(OUTPUT_BIN)

$(OUTPUT_HEX): $(OUTPUT_IHX)
	packihx $(OUTPUT_IHX) > $(OUTPUT_HEX)

$(OUTPUT_BIN): $(OUTPUT_IHX)
	makebin -p $(OUTPUT_IHX) $(OUTPUT_BIN)

$(OBJ_DIR):
	-mkdir -p $(OBJ_DIR)

$(OUTPUT_IHX): $(LIBS)
	$(SDCC) $(LIBS) $(SDCC_CFLAGS) $(SDCC_LDFLAGS) -o $(OUTPUT_IHX)

$(OBJ_DIR)/%.rel: $(USB_SOURCE_DIR)/%.c $(OBJ_DIR)
	$(SDCC) $(SDCC_CFLAGS) -c $<

flash_usb: $(OUTPUT_BIN)
	wchisptool -f $(OUTPUT_BIN)