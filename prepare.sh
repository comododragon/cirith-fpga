#!/bin/bash

set -e

echo "************************************ CIRITH-FPGA Preparer ************************************"
echo "This script prepares this folder for proper execution of the cirith script"
echo "That includes creating some symlinks, selecting scripts..."
echo "***********************************************************************************************"

echo ""
echo "WARNING: BEFORE RUNNING THIS SCRIPT, YOU SHOULD HAVE GIT-CLONED LINA AND POPULATED THE PARBOIL FOLDER"
echo "See etc/lina/README and etc/parboil/README"
echo -n "Press ENTER to proceed or CTRL+C to abort..."
read

echo ""
echo "This repository supports two versions of lina:"
echo ""
echo "    1. New version of lina that includes a cache daemon (linad)"
echo "       This cache daemon helps to accelerate multiple lina executions when executed in parallel"
echo "       This implementation is present on the cachedaemon branch of lina's new repository:"
echo "       https://github.com/comododragon/linaii/tree/cachedaemon"
echo "       The cache daemon is experimental, but usable"
echo ""
echo "    2. Old "new" version of lina, without cache daemon"
echo "       This version is functionally equivalent, including off-chip support, but without the"
echo "       cache daemon. This might degrade performance if executed in parallel with many threads"
echo "       This implementation is present on the main branch of lina's new repository:"
echo "       https://github.com/comododragon/linaii/tree/master"
echo ""
echo "Each version requires a different run.py script to dispatch lina calls."
echo -n "Which version do you want to use [DEFAULT=1, CTRL+C to abort]? "
read LINAVERSION

if [ 2 == $LINAVERSION ]; then
	echo "Selected option 2: moving largedse run.py to the linatools folder"
	rm -f etc/lina/linatools/run.py
	cp -p etc/lina/llvm/tools/lina/misc/largedse/run.py etc/lina/linatools/run.py
else
	echo "Selected option 1: moving linad run.py to the linatools folder"
	rm -f etc/lina/linatools/run.py
	cp -p etc/lina/llvm/tools/lina/misc/linad/run.py etc/lina/linatools/run.py
fi

echo "WARNING: This script will modify the \"sources\" folder! It should be executed only once, after extracting or git-cloning this repo"
echo -n "Press ENTER to proceed or CTRL+C to abort..."
read

echo "Preparing fpga/histo/zcu104/lina..."
cd sources/fpga/histo/zcu104/lina
ln -s ../../../../../etc/parboil/common .
ln -s ../../../../../etc/parboil/cirithcustomdatasets datasets
cd -

echo "Preparing fpga/histo/zcu104/vivado..."
cd sources/fpga/histo/zcu104/vivado
ln -s ../../../../../etc/parboil/common .
ln -s ../../../../../etc/parboil/cirithcustomdatasets datasets
cd -

echo "Preparing fpga/histo/zcu104_bsl/lina..."
cd sources/fpga/histo/zcu104_bsl/lina
ln -s ../../../../../etc/parboil/common .
ln -s ../../../../../etc/parboil/cirithcustomdatasets datasets
cd -

echo "Preparing fpga/histo/zcu104_bsl/vivado..."
cd sources/fpga/histo/zcu104_bsl/vivado
ln -s ../../../../../etc/parboil/common .
ln -s ../../../../../etc/parboil/cirithcustomdatasets datasets
cd -

echo "Preparing fpga/histo/zcu104_rbw/lina..."
cd sources/fpga/histo/zcu104_rbw/lina
ln -s ../../../../../etc/parboil/common .
ln -s ../../../../../etc/parboil/cirithcustomdatasets datasets
cd -

echo "Preparing fpga/histo/zcu104_rbw/vivado..."
cd sources/fpga/histo/zcu104_rbw/vivado
ln -s ../../../../../etc/parboil/common .
ln -s ../../../../../etc/parboil/cirithcustomdatasets datasets
cd -

echo "Preparing fpga/histo/zcu104_rbw_bsl/lina..."
cd sources/fpga/histo/zcu104_rbw_bsl/lina
ln -s ../../../../../etc/parboil/common .
ln -s ../../../../../etc/parboil/cirithcustomdatasets datasets
cd -

echo "Preparing fpga/histo/zcu104_rbw_bsl/vivado..."
cd sources/fpga/histo/zcu104_rbw_bsl/vivado
ln -s ../../../../../etc/parboil/common .
ln -s ../../../../../etc/parboil/cirithcustomdatasets datasets
cd -

echo "Preparing fpga/lbm_small/zcu104/lina..."
cd sources/fpga/lbm_small/zcu104/lina
ln -s ../../../../../etc/parboil/common .
ln -s ../../../../../etc/parboil/cirithcustomdatasets datasets
cd -

echo "Preparing fpga/lbm_small/zcu104/vivado..."
cd sources/fpga/lbm_small/zcu104/vivado
ln -s ../../../../../etc/parboil/common .
ln -s ../../../../../etc/parboil/cirithcustomdatasets datasets
cd -

echo "Preparing fpga/lbm_small/zcu104_bsl/lina..."
cd sources/fpga/lbm_small/zcu104_bsl/lina
ln -s ../../../../../etc/parboil/common .
ln -s ../../../../../etc/parboil/cirithcustomdatasets datasets
cd -

echo "Preparing fpga/lbm_small/zcu104_bsl/vivado..."
cd sources/fpga/lbm_small/zcu104_bsl/vivado
ln -s ../../../../../etc/parboil/common .
ln -s ../../../../../etc/parboil/cirithcustomdatasets datasets
cd -

echo "Preparing fpga/lbm_small/zcu104_sD_rbw/lina..."
cd sources/fpga/lbm_small/zcu104_sD_rbw/lina
ln -s ../../../../../etc/parboil/common .
ln -s ../../../../../etc/parboil/cirithcustomdatasets datasets
cd -

echo "Preparing fpga/lbm_small/zcu104_sD_rbw/vivado..."
cd sources/fpga/lbm_small/zcu104_sD_rbw/vivado
ln -s ../../../../../etc/parboil/common .
ln -s ../../../../../etc/parboil/cirithcustomdatasets datasets
cd -

echo "Preparing fpga/lbm_small/zcu104_sD_rbw_bsl/lina..."
cd sources/fpga/lbm_small/zcu104_sD_rbw_bsl/lina
ln -s ../../../../../etc/parboil/common .
ln -s ../../../../../etc/parboil/cirithcustomdatasets datasets
cd -

echo "Preparing fpga/lbm_small/zcu104_sD_rbw_bsl/vivado..."
cd sources/fpga/lbm_small/zcu104_sD_rbw_bsl/vivado
ln -s ../../../../../etc/parboil/common .
ln -s ../../../../../etc/parboil/cirithcustomdatasets datasets
cd -

echo "Preparing fpga/mri_q/zcu104/lina..."
cd sources/fpga/mri_q/zcu104/lina
ln -s ../../../../../etc/parboil/common .
ln -s ../../../../../etc/parboil/cirithcustomdatasets datasets
cd -

echo "Preparing fpga/mri_q/zcu104/vivado..."
cd sources/fpga/mri_q/zcu104/vivado
ln -s ../../../../../etc/parboil/common .
ln -s ../../../../../etc/parboil/cirithcustomdatasets datasets
cd -

echo "Preparing fpga/mri_q/zcu104_bsl/lina..."
cd sources/fpga/mri_q/zcu104_bsl/lina
ln -s ../../../../../etc/parboil/common .
ln -s ../../../../../etc/parboil/cirithcustomdatasets datasets
cd -

echo "Preparing fpga/mri_q/zcu104_bsl/vivado..."
cd sources/fpga/mri_q/zcu104_bsl/vivado
ln -s ../../../../../etc/parboil/common .
ln -s ../../../../../etc/parboil/cirithcustomdatasets datasets
cd -

echo "Preparing fpga/mri_q/zcu104_sD_rbw/lina..."
cd sources/fpga/mri_q/zcu104_sD_rbw/lina
ln -s ../../../../../etc/parboil/common .
ln -s ../../../../../etc/parboil/cirithcustomdatasets datasets
cd -

echo "Preparing fpga/mri_q/zcu104_sD_rbw/vivado..."
cd sources/fpga/mri_q/zcu104_sD_rbw/vivado
ln -s ../../../../../etc/parboil/common .
ln -s ../../../../../etc/parboil/cirithcustomdatasets datasets
cd -

echo "Preparing fpga/mri_q/zcu104_sD_rbw_bsl/lina..."
cd sources/fpga/mri_q/zcu104_sD_rbw_bsl/lina
ln -s ../../../../../etc/parboil/common .
ln -s ../../../../../etc/parboil/cirithcustomdatasets datasets
cd -

echo "Preparing fpga/mri_q/zcu104_sD_rbw_bsl/vivado..."
cd sources/fpga/mri_q/zcu104_sD_rbw_bsl/vivado
ln -s ../../../../../etc/parboil/common .
ln -s ../../../../../etc/parboil/cirithcustomdatasets datasets
cd -

echo "Preparing fpga/sgemm/zcu104/lina..."
cd sources/fpga/sgemm/zcu104/lina
ln -s ../../../../../etc/parboil/common .
ln -s ../../../../../etc/parboil/cirithcustomdatasets datasets
cd -

echo "Preparing fpga/sgemm/zcu104/vivado..."
cd sources/fpga/sgemm/zcu104/vivado
ln -s ../../../../../etc/parboil/common .
ln -s ../../../../../etc/parboil/cirithcustomdatasets datasets
cd -

echo "Preparing fpga/sgemm/zcu104_bsl/lina..."
cd sources/fpga/sgemm/zcu104_bsl/lina
ln -s ../../../../../etc/parboil/common .
ln -s ../../../../../etc/parboil/cirithcustomdatasets datasets
cd -

echo "Preparing fpga/sgemm/zcu104_bsl/vivado..."
cd sources/fpga/sgemm/zcu104_bsl/vivado
ln -s ../../../../../etc/parboil/common .
ln -s ../../../../../etc/parboil/cirithcustomdatasets datasets
cd -

echo "Preparing fpga/sgemm/zcu104_sD_rbw/lina..."
cd sources/fpga/sgemm/zcu104_sD_rbw/lina
ln -s ../../../../../etc/parboil/common .
ln -s ../../../../../etc/parboil/cirithcustomdatasets datasets
cd -

echo "Preparing fpga/sgemm/zcu104_sD_rbw/vivado..."
cd sources/fpga/sgemm/zcu104_sD_rbw/vivado
ln -s ../../../../../etc/parboil/common .
ln -s ../../../../../etc/parboil/cirithcustomdatasets datasets
cd -

echo "Preparing fpga/sgemm/zcu104_sD_rbw_bsl/lina..."
cd sources/fpga/sgemm/zcu104_sD_rbw_bsl/lina
ln -s ../../../../../etc/parboil/common .
ln -s ../../../../../etc/parboil/cirithcustomdatasets datasets
cd -

echo "Preparing fpga/sgemm/zcu104_sD_rbw_bsl/vivado..."
cd sources/fpga/sgemm/zcu104_sD_rbw_bsl/vivado
ln -s ../../../../../etc/parboil/common .
ln -s ../../../../../etc/parboil/cirithcustomdatasets datasets
cd -
