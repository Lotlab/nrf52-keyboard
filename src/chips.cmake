
if(NOT DEFINED CHIP_FAMILY)
    message("Please set CHIP_FAMILY in your device specific cmake")
endif()

string(TOLOWER ${CHIP_FAMILY} CHIP_FAMILY_LOWER)

include(${CMAKE_CURRENT_LIST_DIR}/chips/${CHIP_FAMILY_LOWER}.cmake)
