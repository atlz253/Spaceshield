.PHONY: all noshield clean

libtarget = bin/spaceshield.o

all: bin/hid.o bin/spaceshield.o
	if not exist "bin" md "bin"
	ar crs bin/libspaceshield.a bin/hid.o bin/spaceshield.o

noshield: bin/hid.o bin/nospaceshield.o
	if not exist "bin" md "bin"
	ar crs bin/libspaceshield.a bin/hid.o bin/nospaceshield.o

bin/hid.o: external/hidapi/windows/hid.c
	if not exist "bin" md "bin"
	gcc -c -Iexternal/hidapi/windows -Iexternal/hidapi/hidapi -o bin/hid.o external/hidapi/windows/hid.c

bin/spaceshield.o: bin/hid.o src/spaceshield.cpp
	if not exist "bin" md "bin"
	g++ -c -Iinclude -Iexternal/CImg -Iexternal/hidapi/hidapi -o bin/spaceshield.o src/spaceshield.cpp

bin/nospaceshield.o: bin/hid.o src/nospaceshield.cpp
	if not exist "bin" md "bin"
	g++ -c -Iinclude -Iexternal/CImg -Iexternal/hidapi/hidapi -o bin/nospaceshield.o src/nospaceshield.cpp

clean:
	rmdir /s /q bin