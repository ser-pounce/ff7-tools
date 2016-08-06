.PHONY: all
all: ulgp.exe touphScript.exe flevel-tidy.exe

root := $(dir $(lastword $(MAKEFILE_LIST)))

include $(root)boost_filesystem/Makefile
include $(root)boost_system/Makefile
include $(root)ff7/Makefile
include $(root)z/Makefile

include $(root)ulgp/Makefile
include $(root)flevel-tidy/Makefile
include $(root)touphScript/Makefile

