# <!-- mkdir obj_dir build_lib -->
rm obj_dir/* build_lib/*

verilator -cc --sv -O3   --Mdir obj_dir   --top-module axi_sa16   -f filelist.f   -CFLAGS "-O3 -fPIC"   -LDFLAGS "-fPIC"   -Wno-UNOPTFLAT -Wno-CASEINCOMPLETE -Wno-WIDTH -Wno-PINCONNECTEMPTY

ls obj_dir/

cd obj_dir

make -f Vaxi_sa16.mk

export VERILATOR_ROOT="/home/jueun/park/renode/renode-latest/verilator"

# c++ -c -O3 -fPIC   -Iobj_dir   -I"$VERILATOR_ROOT/include"   -I"$VERILATOR_ROOT/include/vltstd"   "$VERILATOR_ROOT/include/verilated.cpp"   -o obj_dir/verilated.o
c++ -c -O3 -fPIC     -I"$VERILATOR_ROOT/include"   -I"$VERILATOR_ROOT/include/vltstd"   "$VERILATOR_ROOT/include/verilated.cpp"   -o verilated.o

c++ -shared -o ../build_lib/libVtop.so -fPIC -O3 \
  -I. \
  -I"$VERILATOR_ROOT/include" \
  -I"$VERILATOR_ROOT/include/vltstd" \
  -DVM_THREADED \
  ../sim_lib.cpp \
  Vaxi_sa16__ALL.a \
  verilated.o \
  -pthread
  
echo "\n[done] -> ../build_lib/libVtop.so"
ls ../build_lib/

echo "export VTOP_LIBRARY_PATH=$(readlink -f ../build_lib/libVtop.so)"
export VTOP_LIBRARY_PATH="$(pwd)/../build_lib/libVtop.so"

echo "\n"
nm -D ../build_lib/libVtop.so | egrep 'sa16_(matmul|wait|conv|get_cycles)'
