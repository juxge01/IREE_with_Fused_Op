# ==== 경로 설정(환경에 맞게 조정) ====
RISCV_PREFIX ?= riscv32-unknown-elf
CC  := $(RISCV_PREFIX)-gcc
LD  := $(RISCV_PREFIX)-gcc
OBJDUMP := $(RISCV_PREFIX)-objdump

MLIR_OPT       ?= mlir-opt
MLIR_TRANSLATE ?= mlir-translate
LLC            ?= llc

CFLAGS := -Os -march=rv32imac -mabi=ilp32 -ffreestanding -nostdlib -Wall
LDFLAGS := -T linker.ld -nostartfiles -Wl,-Map=fw.map

LLCFLAGS = -O2 -filetype=obj -mtriple=riscv32-unknown-elf \
           -mcpu=generic -mattr=+m,+a,+c,+zicsr,+zifencei,-zmmul

all: fw.elf

# MLIR → LLVM IR → RISCV obj
mlir/offload_sa16.ll: mlir/offload_sa16.mlir
	$(MLIR_OPT) mlir/offload_sa16.mlir \
	  --convert-to-llvm --convert-func-to-llvm --reconcile-unrealized-casts \
	  | $(MLIR_TRANSLATE) -mlir-to-llvmir > $@

mlir/offload_sa16.o: mlir/offload_sa16.ll
	 llc $(LLCFLAGS) $< -o $@

# 나머지 소스
# runtime.o: runtime.c
# 	$(CC) $(CFLAGS) -c $< -o $@

accelerator_runtime.o: accelerator_runtime.c
	$(CC) $(CFLAGS) -c $< -o $@

selftest.o: selftest.c
	$(CC) $(CFLAGS) -c $< -o $@

main.o: main.c
	$(CC) $(CFLAGS) -c $< -o $@

start.o: start.S
	$(CC) $(CFLAGS) -c $< -o $@

fw.elf: start.o main.o accelerator_runtime.o selftest.o mlir/offload_sa16.o linker.ld
	$(LD) $(CFLAGS) start.o main.o selftest.o accelerator_runtime.o mlir/offload_sa16.o $(LDFLAGS) -o $@
	@echo "Built $@"
	$(OBJDUMP) -d $@ > fw.dump

clean:
	rm -f *.o fw.elf fw.map fw.dump
	rm -f mlir/*.ll mlir/*.o
