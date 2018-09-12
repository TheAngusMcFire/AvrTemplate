.DEFAULT_GOAL :=all

TOOLCHAIN=avr
CC=avr
# the include files are stored at "/usr/lib/avr/include" under linux

OUT_FOLDER = out
FILE_NAME = template

ELF_FILE = $(FILE_NAME).elf
HEX_FILE = $(FILE_NAME).hex
LST_FILE = $(FILE_NAME).lst
MAP_FILE = $(FILE_NAME).map

CPU=atmega328p

CFLAGS = -Wall -O0 -mmcu=$(CPU) -I./ -DWHO_AM_I=\"$(FILE_NAME)\"

SRC_FILES_C = $(wildcard *.c)
SRC_FILES_ASM += $(wildcard *.asm)
SRC_FILES_CPP += $(wildcard *.cpp)

INC_FILES = $(wildcard *.h)
INC_FILES += $(wildcard *.hpp)

OBJ = $(patsubst %.c, $(OUT_FOLDER)/%.o, $(SRC_FILES_C))
OBJ += $(patsubst %.asm, $(OUT_FOLDER)/%.o, $(SRC_FILES_ASM))
OBJ += $(patsubst %.cpp, $(OUT_FOLDER)/%.o, $(SRC_FILES_CPP))

DEBS= $(INC_FILES) makefile

PORT=/dev/ttyUSB1

debug:
	echo $(OBJ)

all: setup $(ELF_FILE) $(LST_FILE) $(HEX_FILE)

$(ELF_FILE): $(OBJ)
	$(CC)-gcc  $(OBJ) $(CFLAGS) -Wl,-Map=$(MAP_FILE) -o $(ELF_FILE)

./$(OUT_FOLDER)/%.o: %.asm $(DEBS)
	$(CC)-as -g -c $< -o $@	

./$(OUT_FOLDER)/%.o: %.c $(DEBS)
	$(CC)-gcc $(CFLAGS) -c $< -o $@
	
./$(OUT_FOLDER)/%.o: %.cpp $(DEBS)
	$(CC)-g++ $(CFLAGS) -c $< -o $@

$(LST_FILE): $(ELF_FILE)
	$(CC)-objdump -d $(ELF_FILE) > $(LST_FILE)

$(HEX_FILE): $(ELF_FILE)
	$(CC)-objcopy -O ihex -R .eeprom $(ELF_FILE) $(HEX_FILE)

download: $(HEX_FILE)
	avrdude -v -patmega328p -carduino -P$(PORT) -b57600 -D -Uflash:w:$(HEX_FILE):i 

rebuild: clean all

clean:
	rm -rf $(OUT_FOLDER)/* *.elf *.lst *.img *.map *.hex

install_dep:
	sudo apt install avr-libc avrdude

setup: $(OUT_FOLDER)

$(OUT_FOLDER):
	mkdir $(OUT_FOLDER)
