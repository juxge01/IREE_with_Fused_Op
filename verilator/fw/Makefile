CROSS  ?= riscv64-unknown-elf
CFLAGS  = -Os -march=rv32imac -mabi=ilp32 -ffreestanding -nostdlib -Wall
LDFLAGS = -T linker.ld -nostartfiles -Wl,-Map=fw.map

all: fw.elf

fw.elf: start.S main.c linker.ld
	$(CROSS)-gcc $(CFLAGS) start.S main.c -o $@ $(LDFLAGS)

clean:
	rm -f fw.elf fw.map

