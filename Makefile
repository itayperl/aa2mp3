DLL_PATH := AAXSDKWin.dll

CC := i586-mingw32msvc-gcc

all: dump.exe

dump.exe: dump.c
	$(CC) -o $@ dump.c $(DLL_PATH)
