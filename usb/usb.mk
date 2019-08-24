PROJ_NAME := ch554

ifndef OUTPUT_DIR
ifdef OUTPUT_DIRECTORY
	OUTPUT_DIR = $(OUTPUT_DIRECTORY)
else
	OUTPUT_DIR = _build
endif
endif
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
	@echo DONE ch554

$(OUTPUT_HEX): $(OUTPUT_IHX)
	@echo Preparing: $(OUTPUT_HEX)
	@packihx $(OUTPUT_IHX) > $(OUTPUT_HEX)

$(OUTPUT_BIN): $(OUTPUT_IHX)
	@echo Preparing: $(OUTPUT_BIN)
	@makebin -p $(OUTPUT_IHX) $(OUTPUT_BIN)

$(OBJ_DIR):
	-@mkdir -p $(OBJ_DIR)

$(OUTPUT_IHX): $(LIBS)
	@echo Linking target: $(notdir $(OUTPUT_IHX))
	@$(SDCC) $(LIBS) $(SDCC_CFLAGS) $(SDCC_LDFLAGS) -o $(OUTPUT_IHX)

$(OBJ_DIR)/%.rel: $(USB_SOURCE_DIR)/%.c $(OBJ_DIR)
	@echo Compiling usb file: $(notdir $<)
	@$(SDCC) $(SDCC_CFLAGS) -c $<

flash_usb: $(OUTPUT_BIN)
	wchisptool -f $(OUTPUT_BIN)