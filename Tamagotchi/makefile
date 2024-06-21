.PHONY: all noshield clean

all: libspaceshield bin/main.exe

noshield: libextras = noshield
noshield: all

libspaceshield:
	if not exist "bin" md "bin"
	make -C external/spaceshield $(libextras)
	copy external\\spaceshield\\bin\\libspaceshield.a bin\\libspaceshield.a

bin/main.exe: bin/libspaceshield.a src/main.cpp
	if not exist "bin" md "bin"
	g++ -Iexternal/spaceshield/include -o bin/main.exe src/main.cpp -Lbin -lspaceshield -lgdi32

clean:
	make -C external/spaceshield clean
	rmdir /s /q bin