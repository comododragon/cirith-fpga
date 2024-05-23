#!/bin/bash

# This script sets up the environment variables needed for each tool used by cirith, currently Vivado and Lina
# All variables are usual linux variables, prepended with a prefix that identifies each framework (CIR_VI_ for
# Vivado and CIR_LI_ for Lina)

# XXX We assume that some tools are in a known relative path from cirith's root directory (where this script is located)
# If this is not the case, substitute the lines below containing $CIR_ROOTD with the appropriate directions for each tool
CIR_ROOTD=$(dirname $(readlink -f $0))

# Set this to your Vivado 2018.2 root path
echo "Setting up Vivado variables..."
VIV_ROOTD=/home/perina/Xilinx
# Set this to your Lina bin folder
echo "Setting up Lina variables..."
LINA_BIN=$CIR_ROOTD/etc/lina/build/bin
LINAD_BIN=$CIR_ROOTD/etc/lina/llvm/tools/lina/misc/linad
# Set up codegen variables
echo "Setting up codegen variables..."
export CDG_ROOTD=$CIR_ROOTD/etc/lina/linatools/codegenrepo

# Configuration for profiling on Zynq using zynprof
echo "Setting up ZYN_* variables used by the ZCU104 board profiling framework..."
# Set this to the sdcard mount path that will be used on the FPGA board.
# (if you comment this line, the Parboil scripts will attempt to find the mountpoint for /dev/mmcblk0p1)
export ZYN_SDMNTPATH=/dev/null
# Set this to the FPGA board's TTY. The ZCU104 generates 4 USB ttys. The second one is the default.
# If no other ttyUSB device is connected, that would be /dev/ttyUSB1
export ZYN_ZYNTTY=/dev/ttyUSB1
# Set this to the FPGA board TTY baud. For ZCU104's tty1, default is 115200
export ZYN_ZYNBAUD=115200
# Set this to the TTY that communicates with the Arduino programmed with the irps5401/PSU sensors
# This is a single USB TTY. By convention we connect it after the ZCU104 board that generates 4 TTYs,
# therefore this defaults to /dev/ttyUSB4
export ZYN_ARDTTY=/dev/ttyUSB4
# Set this to the Arduino TTY baud. Default is 9600
export ZYN_ARDBAUD=9600

# XXX One probably won't need to edit anything below here (unless your installation setup is exotic)

# Save a snapshot of PATH and LD_LIBRARY_PATH
SNAP_PATH=$PATH
SNAP_LD_LIBRARY_PATH=$LD_LIBRARY_PATH

# Set up Vivado variables
echo "Setting up Vivado variables for cirith (CIR_VI_*)..."
# Here we source the Vivado script that changes PATH and LD_LIBRARY_PATH (it also exports XILINX_SDX and XILINX_VIVADO,
# but I don't see any problem in that). Then, we save these new variables and we restore the snapshots
source $VIV_ROOTD/SDx/2018.2/settings64.sh
export CIR_VI_PATH=$PATH
export CIR_VI_LD_LIBRARY_PATH=$LD_LIBRARY_PATH
export PATH=$SNAP_PATH
export LD_LIBRARY_PATH=$SNAP_LD_LIBRARY_PATH

# Set up Lina variables
echo "Setting up Lina variables for cirith (CIR_LI_*)..."
export CIR_LI_PATH=$LINAD_BIN:$LINA_BIN:$SNAP_PATH

echo "Done!"
