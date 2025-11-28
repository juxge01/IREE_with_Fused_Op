# echo "TEST 1: 10% offloading"
# $IREE_PATH/iree-compile mobilenetv2.mlir --compile-to=flow -o mobilenetv2_flow_0.1.mlir --iree-flow-enable-custom-fusion --iree-flow-enable-profiling-instrument=true --iree-flow-top-k-ratio=0.1

# echo "TEST 2: 30% offloading"
# $IREE_PATH/iree-compile mobilenetv2.mlir --compile-to=flow -o mobilenetv2_flow_0.3.mlir --iree-flow-enable-custom-fusion --iree-flow-enable-profiling-instrument=true --iree-flow-top-k-ratio=0.3

# echo "TEST 3: 50% offloading"
# $IREE_PATH/iree-compile mobilenetv2.mlir --compile-to=flow -o mobilenetv2_flow_0.5.mlir --iree-flow-enable-custom-fusion --iree-flow-enable-profiling-instrument=true --iree-flow-top-k-ratio=0.5

# echo "TEST 4: 80% offloading"
# $IREE_PATH/iree-compile mobilenetv2.mlir --compile-to=flow -o mobilenetv2_flow_0.8.mlir --iree-flow-enable-custom-fusion --iree-flow-enable-profiling-instrument=true --iree-flow-top-k-ratio=0.8

# echo "TEST 5: 100% offloading"
# $IREE_PATH/iree-compile mobilenetv2.mlir --compile-to=flow -o mobilenetv2_flow_1.0.mlir --iree-flow-enable-custom-fusion --iree-flow-enable-profiling-instrument=true --iree-flow-top-k-ratio=1.0

echo "NO-fusion Tests"
echo "TEST 1: 10% offloading"
$IREE_PATH/iree-compile mobilenetv2.mlir --compile-to=flow -o mobilenetv2_flow_0.1_nf.mlir 

echo "TEST 2: 30% offloading"
$IREE_PATH/iree-compile mobilenetv2.mlir --compile-to=flow -o mobilenetv2_flow_0.3_nf.mlir 

echo "TEST 3: 50% offloading"
$IREE_PATH/iree-compile mobilenetv2.mlir --compile-to=flow -o mobilenetv2_flow_0.5_nf.mlir 

echo "TEST 4: 80% offloading"
$IREE_PATH/iree-compile mobilenetv2.mlir --compile-to=flow -o mobilenetv2_flow_0.8_nf.mlir 

echo "TEST 5: 100% offloading"
$IREE_PATH/iree-compile mobilenetv2.mlir --compile-to=flow -o mobilenetv2_flow_1.0_nf.mlir 

echo "Fusion Tests"
echo "TEST 1: 10% offloading"
$IREE_PATH/iree-compile mobilenetv2.mlir --compile-to=flow -o mobilenetv2_flow_0.1_f.mlir --iree-intel-planner-bwh2d=1e12 --iree-intel-planner-pacc=1e11 --iree-intel-planner-launch=1e-6 --iree-flow-enable-custom-fusion  --iree-flow-top-k-ratio=0.1

echo "TEST 2: 30% offloading"
$IREE_PATH/iree-compile mobilenetv2.mlir --compile-to=flow -o mobilenetv2_flow_0.3_f.mlir --iree-intel-planner-bwh2d=1e12 --iree-intel-planner-pacc=1e11 --iree-intel-planner-launch=1e-6 --iree-flow-enable-custom-fusion  --iree-flow-top-k-ratio=0.3

echo "TEST 3: 50% offloading"
$IREE_PATH/iree-compile mobilenetv2.mlir --compile-to=flow -o mobilenetv2_flow_0.5_f.mlir --iree-intel-planner-bwh2d=1e12 --iree-intel-planner-pacc=1e11 --iree-intel-planner-launch=1e-6 --iree-flow-enable-custom-fusion  --iree-flow-top-k-ratio=0.5

echo "TEST 4: 80% offloading"
$IREE_PATH/iree-compile mobilenetv2.mlir --compile-to=flow -o mobilenetv2_flow_0.8_f.mlir --iree-intel-planner-bwh2d=1e12 --iree-intel-planner-pacc=1e11 --iree-intel-planner-launch=1e-6 --iree-flow-enable-custom-fusion  --iree-flow-top-k-ratio=0.8

echo "TEST 5: 100% offloading"
$IREE_PATH/iree-compile mobilenetv2.mlir --compile-to=flow -o mobilenetv2_flow_1.0_f.mlir --iree-intel-planner-bwh2d=1e12    --iree-intel-planner-pacc=1e11 --iree-intel-planner-launch=1e-6 --iree-flow-enable-custom-fusion  --iree-flow-top-k-ratio=1.0

echo ""
rg -i 'offload.target' 
