DLL := AAXSDKWin.dll

POSSIBLE_CCS := i586-mingw32msvc-gcc i686-w64-mingw32-gcc

MINGWCC := $(firstword $(foreach cc,$(POSSIBLE_CCS), \
            $(shell which $(cc) 2> /dev/null)))

ifndef MINGWCC
    $(error No mingw32 compiler found)
endif

all: dump.exe

$(DLL):
	$(error The DLL $(DLL) from the Audible app should be provided)

dump.exe: dump.c $(DLL)
	$(MINGWCC) -o $@ dump.c $(DLL)
