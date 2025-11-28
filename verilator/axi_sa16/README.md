<!-- mkdir obj_dir build_lib -->

rm obj*dir/* build/\_

verilator -cc --sv -O3 --Mdir obj_dir --top-module axi_sa16 -f filelist.f -CFLAGS "-O3 -fPIC" -LDFLAGS "-fPIC" -Wno-UNOPTFLAT -Wno-CASEINCOMPLETE -Wno-WIDTH -Wno-PINCONNECTEMPTY

make -C obj_dir -f Vaxi_sa16.mk

export VERILATOR_ROOT="/home/jueun/park/renode/renode-latest/verilator"

c++ -c -O3 -fPIC \
 -Iobj_dir \
 -I"$VERILATOR_ROOT/include" \
  -I"$VERILATOR_ROOT/include/vltstd" \
 "$VERILATOR_ROOT/include/verilated.cpp" \
 -o obj_dir/verilated.o

c++ -shared -o build_lib/libVtop.so -fPIC -O3 \
 -Iobj_dir \
 -I"$VERILATOR_ROOT/include" \
  -I"$VERILATOR_ROOT/include/vltstd" \
 sim_lib.cpp \
 obj_dir/Vaxi_sa16\_\_ALL.a \
 obj_dir/verilated.o \
 -pthread

export VTOP_LIBRARY_PATH="$(pwd)/build_lib/libVtop.so"
