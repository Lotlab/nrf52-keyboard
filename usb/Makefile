PROJ_NAME := ch554
OUTPUT_DIR = _build
OBJ_DIR = $(OUTPUT_DIR)/$(PROJ_NAME)

CC := sdcc
CFLAGS := -mmcs51 --model-small --std-c11 -o $(OBJ_DIR)/ --opt-code-size
LDFLAGS := --xram-size 1024 --iram-size 256 --code-size 16384 --out-fmt-ihx

LIBS := $(patsubst %.c,$(OBJ_DIR)/%.rel,$(wildcard *.c))

OUTPUT_IHX := $(OBJ_DIR)/$(PROJ_NAME).ihx
OUTPUT_HEX := $(OUTPUT_DIR)/$(PROJ_NAME).hex
OUTPUT_BIN := $(OUTPUT_DIR)/$(PROJ_NAME).bin

.PHONY:clean,default

default: $(OUTPUT_HEX) $(OUTPUT_BIN)

$(OUTPUT_HEX): $(OUTPUT_IHX)
	packihx $(OUTPUT_IHX) > $(OUTPUT_HEX)

$(OUTPUT_BIN): $(OUTPUT_IHX)
	makebin -p $(OUTPUT_IHX) $(OUTPUT_BIN)

$(OUTPUT_DIR):
	-mkdir $(OUTPUT_DIR)

$(OBJ_DIR): $(OUTPUT_DIR)
	-mkdir $(OBJ_DIR)

$(OUTPUT_IHX): $(LIBS)
	$(CC) $(LIBS) $(CFLAGS) $(LDFLAGS) -o $(OUTPUT_IHX)

$(OBJ_DIR)/%.rel: %.c $(OBJ_DIR)
	$(CC) $(CFLAGS) -c $<

clean:
	rm -r $(OUTPUT_DIR)

flash: $(OUTPUT_BIN)
	wchisptool -f $(OUTPUT_BIN)