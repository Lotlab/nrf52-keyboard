COMMON_DIR = $(TMK_CORE_DIR)/common
SRC_FILES +=	$(COMMON_DIR)/host.c \
	$(COMMON_DIR)/keyboard.c \
	$(COMMON_DIR)/matrix.c \
	$(COMMON_DIR)/action.c \
	$(COMMON_DIR)/action_tapping.c \
	$(COMMON_DIR)/action_layer.c \
	$(COMMON_DIR)/action_util.c \
	$(COMMON_DIR)/print.c \
	$(COMMON_DIR)/debug.c \
	$(COMMON_DIR)/util.c \
	$(COMMON_DIR)/hook.c \

INC_FOLDERS += $(COMMON_DIR)

# Option modules
ifeq (yes,$(strip $(UNIMAP_ENABLE)))
    SRC_FILES += $(COMMON_DIR)/unimap.c
    OPT_DEFS += -DUNIMAP_ENABLE
    OPT_DEFS += -DACTIONMAP_ENABLE
else
    ifeq (yes,$(strip $(ACTIONMAP_ENABLE)))
	SRC_FILES += $(COMMON_DIR)/actionmap.c
	OPT_DEFS += -DACTIONMAP_ENABLE
    else
	SRC_FILES += $(COMMON_DIR)/keymap.c
    endif
endif

ifeq (yes,$(strip $(BOOTMAGIC_ENABLE)))
    SRC_FILES += $(COMMON_DIR)/bootmagic.c
    SRC_FILES += $(APP_SRC_DIR)/keyboard/keyboard_bootmagic.c
    SRC_FILES += $(APP_SRC_DIR)/tmk/eeconfig.c
    OPT_DEFS += -DBOOTMAGIC_ENABLE
else
    ifeq (yes,$(strip $(BOOTCHECK_ENABLE)))
    SRC_FILES += $(APP_SRC_DIR)/keyboard/keyboard_bootcheck.c
    OPT_DEFS += -DBOOTCHECK_ENABLE
    endif
endif

ifeq (yes,$(strip $(MOUSEKEY_ENABLE)))
    SRC_FILES += $(COMMON_DIR)/mousekey.c
    OPT_DEFS += -DMOUSEKEY_ENABLE
    OPT_DEFS += -DMOUSE_ENABLE
endif

ifeq (yes,$(strip $(EXTRAKEY_ENABLE)))
    OPT_DEFS += -DEXTRAKEY_ENABLE
endif

ifeq (yes,$(strip $(CONSOLE_ENABLE)))
    OPT_DEFS += -DCONSOLE_ENABLE
else
    OPT_DEFS += -DNO_PRINT
    OPT_DEFS += -DNO_DEBUG
endif

ifeq (yes,$(strip $(COMMAND_ENABLE)))
    SRC_FILES += $(APP_SRC_DIR)/keyboard/keyboard_command.c
    OPT_DEFS += -DCOMMAND_ENABLE
endif

ifeq (yes,$(strip $(NKRO_ENABLE)))
    OPT_DEFS += -DNKRO_ENABLE
    OPT_DEFS += -DPROTOCOL_LUFA
endif

ifeq (yes,$(strip $(USB_6KRO_ENABLE)))
    OPT_DEFS += -DUSB_6KRO_ENABLE
endif

ifeq (yes, $(strip $(KEYBOARD_LOCK_ENABLE)))
    OPT_DEFS += -DKEYBOARD_LOCK_ENABLE
endif

ifeq (yes,$(strip $(BACKLIGHT_ENABLE)))
    SRC_FILES += $(COMMON_DIR)/backlight.c
    OPT_DEFS += -DBACKLIGHT_ENABLE
endif

ifeq (yes,$(strip $(KEYMAP_SECTION_ENABLE)))
    OPT_DEFS += -DKEYMAP_SECTION_ENABLE
endif
ifeq (yes,$(strip $(RGBLIGHT_ENABLE)))
    OPT_DEFS += -DRGBLIGHT_ENABLE
endif
