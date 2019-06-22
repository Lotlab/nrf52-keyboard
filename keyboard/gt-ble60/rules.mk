# MCU name
MCU = nrf52832

# Build Options
#   comment out to disable the options.
#
BOOTMAGIC_ENABLE ?= yes	# Virtual DIP switch configuration(+1000)
EXTRAKEY_ENABLE ?= yes	# Audio control and System control(+450)
CONSOLE_ENABLE ?= no	# Console for debug(+400)
COMMAND_ENABLE ?= no    # Commands for debug and configuration
NKRO_ENABLE ?= yes		# USB Nkey Rollover - if this doesn't work, see here: https://github.com/tmk/tmk_keyboard/wiki/FAQ#nkro-doesnt-work
USB_6KRO_ENABLE ?= yes # USB 6Key Rollover


