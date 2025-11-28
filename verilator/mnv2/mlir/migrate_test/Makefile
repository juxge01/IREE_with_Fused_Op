IREE_IMPORT ?= iree-import-tflite
IREE_COMPILE ?= /home/jueun/park/IREE/iree-rv32-springbok-legacy/build/iree_compiler/bin/iree-compile
IREE_OPT ?= /home/jueun/jux/project/iree/build/tools/iree-opt

MIGRATE_SCRIPT ?= ./iree_mlir_migrate.sh
MIGRATE_MODE   ?= propagate

TFLITE_MODEL = mobilenetv2.tflite
TOSA_MLIR    = mobilenetv2.mlir
FLOW_MLIR    = mobilenetv2.flow.mlir
OFFLOAD_MLIR = mobilenetv2.offload.mlir

OFFLOAD_TARGET := $(shell grep -oP 'flow\.executable\.export[[:space:]]+(?:public|private)[[:space:]]+@\K[[:alnum:]_.$-]+' $(FLOW_MLIR) | head -n1)

IREE_COMPILE_FLAGS = \
	--iree-input-type=tosa \
	--iree-hal-target-backends=llvm-cpu \
	--compile-to=flow

all: $(FLOW_MLIR)

$(FLOW_MLIR): $(TOSA_MLIR) $(MIGRATE_SCRIPT)
	@echo "--- Compiling TOSA MLIR to Flow MLIR ---"
	$(IREE_COMPILE) $(TOSA_MLIR) $(IREE_COMPILE_FLAGS) -o $(FLOW_MLIR)
	@echo "--- Migrating Flow MLIR syntax (MODE=$(MIGRATE_MODE)) ---"
	@chmod +x $(MIGRATE_SCRIPT)
	MODE=$(MIGRATE_MODE) $(MIGRATE_SCRIPT) $(FLOW_MLIR)
	@echo "Successfully created $(FLOW_MLIR)\n"

$(TOSA_MLIR): $(TFLITE_MODEL)
	@echo "--- Importing TFLite to TOSA MLIR ---"
	$(IREE_IMPORT) $(TFLITE_MODEL) -o $(TOSA_MLIR)
	@echo "Successfully created $(TOSA_MLIR)\n"

.PHONY: migrate
migrate: $(FLOW_MLIR)
	@echo "--- Migrating Flow MLIR syntax (MODE=$(MIGRATE_MODE)) ---"
	MODE=$(MIGRATE_MODE) $(MIGRATE_SCRIPT) $(FLOW_MLIR)
	@echo "Successfully migrated $(FLOW_MLIR)\n"

.PHONY: identify_offload
identify_offload: $(FLOW_MLIR)
	@echo "--- Identifying offloadable operations in Flow MLIR ---"
# 	@echo "Target export: $(OFFLOAD_TARGET)"
# 	@set -e; \
	$(IREE_OPT) $< --iree-sa16-identify-offload-target=$(OFFLOAD_TARGET) -o $@ 2> offload.log; \
	if grep -q "Target .* not found" offload.log; then \
	  echo "!! Offload target not found. See offload.log"; \
	  rm -f $@; \
	  exit 1; \
	fi
	$(IREE_OPT) $< --iree-sa16-identify-offload-target -o ${OFFLOAD_MLIR} 2> offload.log; 
	@echo "Successfully created $(OFFLOAD_MLIR)\n"

clean:
	@echo "--- Cleaning up generated files ---"
	rm -f *.bak *.mlir

.PHONY: all clean
