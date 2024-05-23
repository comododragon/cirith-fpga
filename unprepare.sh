#!/bin/bash

set -e

echo "WARNING: This script will undo the changes performed by \"prepare.sh\"!"
echo "This means that generated symbolic links will be deleted."
echo "This script WILL NOT delete files that are not symbolic links (for protections sake!)"
echo -n "Press ENTER to proceed or CTRL+C to abort..."
read

echo "Unpreparing fpga/histo/zcu104/lina..."
HISTOLFILES=(
	"sources/fpga/histo/zcu104/lina/common"
	"sources/fpga/histo/zcu104/lina/datasets"
)
for file in ${HISTOLFILES[@]}; do
	if [ -h $file ]; then
		echo "File \"$file\" is a symbolic link. Removing..."
		rm $file
	elif [ -e $file ]; then
		echo "File \"$file\" exists but is not a symbolic link. Will not remove."
	else
		echo "File \"$file\" not found. Skipping..."
	fi
done

echo "Unpreparing fpga/histo/zcu104/vivado..."
HISTOVFILES=(
	"sources/fpga/histo/zcu104/vivado/common"
	"sources/fpga/histo/zcu104/vivado/datasets"
)
for file in ${HISTOVFILES[@]}; do
	if [ -h $file ]; then
		echo "File \"$file\" is a symbolic link. Removing..."
		rm $file
	elif [ -e $file ]; then
		echo "File \"$file\" exists but is not a symbolic link. Will not remove."
	else
		echo "File \"$file\" not found. Skipping..."
	fi
done

echo "Unpreparing fpga/histo/zcu104_bsl/lina..."
HISTOLFILES=(
	"sources/fpga/histo/zcu104_bsl/lina/common"
	"sources/fpga/histo/zcu104_bsl/lina/datasets"
)
for file in ${HISTOLFILES[@]}; do
	if [ -h $file ]; then
		echo "File \"$file\" is a symbolic link. Removing..."
		rm $file
	elif [ -e $file ]; then
		echo "File \"$file\" exists but is not a symbolic link. Will not remove."
	else
		echo "File \"$file\" not found. Skipping..."
	fi
done

echo "Unpreparing fpga/histo/zcu104_bsl/vivado..."
HISTOVFILES=(
	"sources/fpga/histo/zcu104_bsl/vivado/common"
	"sources/fpga/histo/zcu104_bsl/vivado/datasets"
)
for file in ${HISTOVFILES[@]}; do
	if [ -h $file ]; then
		echo "File \"$file\" is a symbolic link. Removing..."
		rm $file
	elif [ -e $file ]; then
		echo "File \"$file\" exists but is not a symbolic link. Will not remove."
	else
		echo "File \"$file\" not found. Skipping..."
	fi
done

echo "Unpreparing fpga/histo/zcu104_rbw/lina..."
HISTOLFILES=(
	"sources/fpga/histo/zcu104_rbw/lina/common"
	"sources/fpga/histo/zcu104_rbw/lina/datasets"
)
for file in ${HISTOLFILES[@]}; do
	if [ -h $file ]; then
		echo "File \"$file\" is a symbolic link. Removing..."
		rm $file
	elif [ -e $file ]; then
		echo "File \"$file\" exists but is not a symbolic link. Will not remove."
	else
		echo "File \"$file\" not found. Skipping..."
	fi
done

echo "Unpreparing fpga/histo/zcu104_rbw/vivado..."
HISTOVFILES=(
	"sources/fpga/histo/zcu104_rbw/vivado/common"
	"sources/fpga/histo/zcu104_rbw/vivado/datasets"
)
for file in ${HISTOVFILES[@]}; do
	if [ -h $file ]; then
		echo "File \"$file\" is a symbolic link. Removing..."
		rm $file
	elif [ -e $file ]; then
		echo "File \"$file\" exists but is not a symbolic link. Will not remove."
	else
		echo "File \"$file\" not found. Skipping..."
	fi
done

echo "Unpreparing fpga/histo/zcu104_rbw_bsl/lina..."
HISTOLFILES=(
	"sources/fpga/histo/zcu104_rbw_bsl/lina/common"
	"sources/fpga/histo/zcu104_rbw_bsl/lina/datasets"
)
for file in ${HISTOLFILES[@]}; do
	if [ -h $file ]; then
		echo "File \"$file\" is a symbolic link. Removing..."
		rm $file
	elif [ -e $file ]; then
		echo "File \"$file\" exists but is not a symbolic link. Will not remove."
	else
		echo "File \"$file\" not found. Skipping..."
	fi
done

echo "Unpreparing fpga/histo/zcu104_rbw_bsl/vivado..."
HISTOVFILES=(
	"sources/fpga/histo/zcu104_rbw_bsl/vivado/common"
	"sources/fpga/histo/zcu104_rbw_bsl/vivado/datasets"
)
for file in ${HISTOVFILES[@]}; do
	if [ -h $file ]; then
		echo "File \"$file\" is a symbolic link. Removing..."
		rm $file
	elif [ -e $file ]; then
		echo "File \"$file\" exists but is not a symbolic link. Will not remove."
	else
		echo "File \"$file\" not found. Skipping..."
	fi
done

echo "Unpreparing fpga/lbm_small/zcu104/lina..."
LBMSMALLLFILES=(
	"sources/fpga/lbm_small/zcu104/lina/common"
	"sources/fpga/lbm_small/zcu104/lina/datasets"
)
for file in ${LBMSMALLLFILES[@]}; do
	if [ -h $file ]; then
		echo "File \"$file\" is a symbolic link. Removing..."
		rm $file
	elif [ -e $file ]; then
		echo "File \"$file\" exists but is not a symbolic link. Will not remove."
	else
		echo "File \"$file\" not found. Skipping..."
	fi
done

echo "Unpreparing fpga/lbm_small/zcu104/vivado..."
LBMSMALLVFILES=(
	"sources/fpga/lbm_small/zcu104/vivado/common"
	"sources/fpga/lbm_small/zcu104/vivado/datasets"
)
for file in ${LBMSMALLVFILES[@]}; do
	if [ -h $file ]; then
		echo "File \"$file\" is a symbolic link. Removing..."
		rm $file
	elif [ -e $file ]; then
		echo "File \"$file\" exists but is not a symbolic link. Will not remove."
	else
		echo "File \"$file\" not found. Skipping..."
	fi
done

echo "Unpreparing fpga/lbm_small/zcu104_bsl/lina..."
LBMSMALLLFILES=(
	"sources/fpga/lbm_small/zcu104_bsl/lina/common"
	"sources/fpga/lbm_small/zcu104_bsl/lina/datasets"
)
for file in ${LBMSMALLLFILES[@]}; do
	if [ -h $file ]; then
		echo "File \"$file\" is a symbolic link. Removing..."
		rm $file
	elif [ -e $file ]; then
		echo "File \"$file\" exists but is not a symbolic link. Will not remove."
	else
		echo "File \"$file\" not found. Skipping..."
	fi
done

echo "Unpreparing fpga/lbm_small/zcu104_bsl/vivado..."
LBMSMALLVFILES=(
	"sources/fpga/lbm_small/zcu104_bsl/vivado/common"
	"sources/fpga/lbm_small/zcu104_bsl/vivado/datasets"
)
for file in ${LBMSMALLVFILES[@]}; do
	if [ -h $file ]; then
		echo "File \"$file\" is a symbolic link. Removing..."
		rm $file
	elif [ -e $file ]; then
		echo "File \"$file\" exists but is not a symbolic link. Will not remove."
	else
		echo "File \"$file\" not found. Skipping..."
	fi
done

echo "Unpreparing fpga/lbm_small/zcu104_sD_rbw/lina..."
LBMSMALLLFILES=(
	"sources/fpga/lbm_small/zcu104_sD_rbw/lina/common"
	"sources/fpga/lbm_small/zcu104_sD_rbw/lina/datasets"
)
for file in ${LBMSMALLLFILES[@]}; do
	if [ -h $file ]; then
		echo "File \"$file\" is a symbolic link. Removing..."
		rm $file
	elif [ -e $file ]; then
		echo "File \"$file\" exists but is not a symbolic link. Will not remove."
	else
		echo "File \"$file\" not found. Skipping..."
	fi
done

echo "Unpreparing fpga/lbm_small/zcu104_sD_rbw/vivado..."
LBMSMALLVFILES=(
	"sources/fpga/lbm_small/zcu104_sD_rbw/vivado/common"
	"sources/fpga/lbm_small/zcu104_sD_rbw/vivado/datasets"
)
for file in ${LBMSMALLVFILES[@]}; do
	if [ -h $file ]; then
		echo "File \"$file\" is a symbolic link. Removing..."
		rm $file
	elif [ -e $file ]; then
		echo "File \"$file\" exists but is not a symbolic link. Will not remove."
	else
		echo "File \"$file\" not found. Skipping..."
	fi
done

echo "Unpreparing fpga/lbm_small/zcu104_sD_rbw_bsl/lina..."
LBMSMALLLFILES=(
	"sources/fpga/lbm_small/zcu104_sD_rbw_bsl/lina/common"
	"sources/fpga/lbm_small/zcu104_sD_rbw_bsl/lina/datasets"
)
for file in ${LBMSMALLLFILES[@]}; do
	if [ -h $file ]; then
		echo "File \"$file\" is a symbolic link. Removing..."
		rm $file
	elif [ -e $file ]; then
		echo "File \"$file\" exists but is not a symbolic link. Will not remove."
	else
		echo "File \"$file\" not found. Skipping..."
	fi
done

echo "Unpreparing fpga/lbm_small/zcu104_sD_rbw_bsl/vivado..."
LBMSMALLVFILES=(
	"sources/fpga/lbm_small/zcu104_sD_rbw_bsl/vivado/common"
	"sources/fpga/lbm_small/zcu104_sD_rbw_bsl/vivado/datasets"
)
for file in ${LBMSMALLVFILES[@]}; do
	if [ -h $file ]; then
		echo "File \"$file\" is a symbolic link. Removing..."
		rm $file
	elif [ -e $file ]; then
		echo "File \"$file\" exists but is not a symbolic link. Will not remove."
	else
		echo "File \"$file\" not found. Skipping..."
	fi
done

echo "Unpreparing fpga/mri_q/zcu104/lina..."
MRIQLFILES=(
	"sources/fpga/mri_q/zcu104/lina/common"
	"sources/fpga/mri_q/zcu104/lina/datasets"
)
for file in ${MRIQLFILES[@]}; do
	if [ -h $file ]; then
		echo "File \"$file\" is a symbolic link. Removing..."
		rm $file
	elif [ -e $file ]; then
		echo "File \"$file\" exists but is not a symbolic link. Will not remove."
	else
		echo "File \"$file\" not found. Skipping..."
	fi
done

echo "Unpreparing fpga/mri_q/zcu104/vivado..."
MRIQVFILES=(
	"sources/fpga/mri_q/zcu104/vivado/common"
	"sources/fpga/mri_q/zcu104/vivado/datasets"
)
for file in ${MRIQVFILES[@]}; do
	if [ -h $file ]; then
		echo "File \"$file\" is a symbolic link. Removing..."
		rm $file
	elif [ -e $file ]; then
		echo "File \"$file\" exists but is not a symbolic link. Will not remove."
	else
		echo "File \"$file\" not found. Skipping..."
	fi
done

echo "Unpreparing fpga/mri_q/zcu104_bsl/lina..."
MRIQLFILES=(
	"sources/fpga/mri_q/zcu104_bsl/lina/common"
	"sources/fpga/mri_q/zcu104_bsl/lina/datasets"
)
for file in ${MRIQLFILES[@]}; do
	if [ -h $file ]; then
		echo "File \"$file\" is a symbolic link. Removing..."
		rm $file
	elif [ -e $file ]; then
		echo "File \"$file\" exists but is not a symbolic link. Will not remove."
	else
		echo "File \"$file\" not found. Skipping..."
	fi
done

echo "Unpreparing fpga/mri_q/zcu104_bsl/vivado..."
MRIQVFILES=(
	"sources/fpga/mri_q/zcu104_bsl/vivado/common"
	"sources/fpga/mri_q/zcu104_bsl/vivado/datasets"
)
for file in ${MRIQVFILES[@]}; do
	if [ -h $file ]; then
		echo "File \"$file\" is a symbolic link. Removing..."
		rm $file
	elif [ -e $file ]; then
		echo "File \"$file\" exists but is not a symbolic link. Will not remove."
	else
		echo "File \"$file\" not found. Skipping..."
	fi
done

echo "Unpreparing fpga/mri_q/zcu104_sD_rbw/lina..."
MRIQLFILES=(
	"sources/fpga/mri_q/zcu104_sD_rbw/lina/common"
	"sources/fpga/mri_q/zcu104_sD_rbw/lina/datasets"
)
for file in ${MRIQLFILES[@]}; do
	if [ -h $file ]; then
		echo "File \"$file\" is a symbolic link. Removing..."
		rm $file
	elif [ -e $file ]; then
		echo "File \"$file\" exists but is not a symbolic link. Will not remove."
	else
		echo "File \"$file\" not found. Skipping..."
	fi
done

echo "Unpreparing fpga/mri_q/zcu104_sD_rbw/vivado..."
MRIQVFILES=(
	"sources/fpga/mri_q/zcu104_sD_rbw/vivado/common"
	"sources/fpga/mri_q/zcu104_sD_rbw/vivado/datasets"
)
for file in ${MRIQVFILES[@]}; do
	if [ -h $file ]; then
		echo "File \"$file\" is a symbolic link. Removing..."
		rm $file
	elif [ -e $file ]; then
		echo "File \"$file\" exists but is not a symbolic link. Will not remove."
	else
		echo "File \"$file\" not found. Skipping..."
	fi
done

echo "Unpreparing fpga/mri_q/zcu104_sD_rbw_bsl/lina..."
MRIQLFILES=(
	"sources/fpga/mri_q/zcu104_sD_rbw_bsl/lina/common"
	"sources/fpga/mri_q/zcu104_sD_rbw_bsl/lina/datasets"
)
for file in ${MRIQLFILES[@]}; do
	if [ -h $file ]; then
		echo "File \"$file\" is a symbolic link. Removing..."
		rm $file
	elif [ -e $file ]; then
		echo "File \"$file\" exists but is not a symbolic link. Will not remove."
	else
		echo "File \"$file\" not found. Skipping..."
	fi
done

echo "Unpreparing fpga/mri_q/zcu104_sD_rbw_bsl/vivado..."
MRIQVFILES=(
	"sources/fpga/mri_q/zcu104_sD_rbw_bsl/vivado/common"
	"sources/fpga/mri_q/zcu104_sD_rbw_bsl/vivado/datasets"
)
for file in ${MRIQVFILES[@]}; do
	if [ -h $file ]; then
		echo "File \"$file\" is a symbolic link. Removing..."
		rm $file
	elif [ -e $file ]; then
		echo "File \"$file\" exists but is not a symbolic link. Will not remove."
	else
		echo "File \"$file\" not found. Skipping..."
	fi
done

echo "Unpreparing fpga/sgemm/zcu104/lina..."
SGEMMLFILES=(
	"sources/fpga/sgemm/zcu104/lina/common"
	"sources/fpga/sgemm/zcu104/lina/datasets"
)
for file in ${SGEMMLFILES[@]}; do
	if [ -h $file ]; then
		echo "File \"$file\" is a symbolic link. Removing..."
		rm $file
	elif [ -e $file ]; then
		echo "File \"$file\" exists but is not a symbolic link. Will not remove."
	else
		echo "File \"$file\" not found. Skipping..."
	fi
done

echo "Unpreparing fpga/sgemm/zcu104/vivado..."
SGEMMVFILES=(
	"sources/fpga/sgemm/zcu104/vivado/common"
	"sources/fpga/sgemm/zcu104/vivado/datasets"
)
for file in ${SGEMMVFILES[@]}; do
	if [ -h $file ]; then
		echo "File \"$file\" is a symbolic link. Removing..."
		rm $file
	elif [ -e $file ]; then
		echo "File \"$file\" exists but is not a symbolic link. Will not remove."
	else
		echo "File \"$file\" not found. Skipping..."
	fi
done

echo "Unpreparing fpga/sgemm/zcu104_bsl/lina..."
SGEMMLFILES=(
	"sources/fpga/sgemm/zcu104_bsl/lina/common"
	"sources/fpga/sgemm/zcu104_bsl/lina/datasets"
)
for file in ${SGEMMLFILES[@]}; do
	if [ -h $file ]; then
		echo "File \"$file\" is a symbolic link. Removing..."
		rm $file
	elif [ -e $file ]; then
		echo "File \"$file\" exists but is not a symbolic link. Will not remove."
	else
		echo "File \"$file\" not found. Skipping..."
	fi
done

echo "Unpreparing fpga/sgemm/zcu104_bsl/vivado..."
SGEMMVFILES=(
	"sources/fpga/sgemm/zcu104_bsl/vivado/common"
	"sources/fpga/sgemm/zcu104_bsl/vivado/datasets"
)
for file in ${SGEMMVFILES[@]}; do
	if [ -h $file ]; then
		echo "File \"$file\" is a symbolic link. Removing..."
		rm $file
	elif [ -e $file ]; then
		echo "File \"$file\" exists but is not a symbolic link. Will not remove."
	else
		echo "File \"$file\" not found. Skipping..."
	fi
done

echo "Unpreparing fpga/sgemm/zcu104_sD_rbw/lina..."
SGEMMLFILES=(
	"sources/fpga/sgemm/zcu104_sD_rbw/lina/common"
	"sources/fpga/sgemm/zcu104_sD_rbw/lina/datasets"
)
for file in ${SGEMMLFILES[@]}; do
	if [ -h $file ]; then
		echo "File \"$file\" is a symbolic link. Removing..."
		rm $file
	elif [ -e $file ]; then
		echo "File \"$file\" exists but is not a symbolic link. Will not remove."
	else
		echo "File \"$file\" not found. Skipping..."
	fi
done

echo "Unpreparing fpga/sgemm/zcu104_sD_rbw/vivado..."
SGEMMVFILES=(
	"sources/fpga/sgemm/zcu104_sD_rbw/vivado/common"
	"sources/fpga/sgemm/zcu104_sD_rbw/vivado/datasets"
)
for file in ${SGEMMVFILES[@]}; do
	if [ -h $file ]; then
		echo "File \"$file\" is a symbolic link. Removing..."
		rm $file
	elif [ -e $file ]; then
		echo "File \"$file\" exists but is not a symbolic link. Will not remove."
	else
		echo "File \"$file\" not found. Skipping..."
	fi
done

echo "Unpreparing fpga/sgemm/zcu104_sD_rbw_bsl/lina..."
SGEMMLFILES=(
	"sources/fpga/sgemm/zcu104_sD_rbw_bsl/lina/common"
	"sources/fpga/sgemm/zcu104_sD_rbw_bsl/lina/datasets"
)
for file in ${SGEMMLFILES[@]}; do
	if [ -h $file ]; then
		echo "File \"$file\" is a symbolic link. Removing..."
		rm $file
	elif [ -e $file ]; then
		echo "File \"$file\" exists but is not a symbolic link. Will not remove."
	else
		echo "File \"$file\" not found. Skipping..."
	fi
done

echo "Unpreparing fpga/sgemm/zcu104_sD_rbw_bsl/vivado..."
SGEMMVFILES=(
	"sources/fpga/sgemm/zcu104_sD_rbw_bsl/vivado/common"
	"sources/fpga/sgemm/zcu104_sD_rbw_bsl/vivado/datasets"
)
for file in ${SGEMMVFILES[@]}; do
	if [ -h $file ]; then
		echo "File \"$file\" is a symbolic link. Removing..."
		rm $file
	elif [ -e $file ]; then
		echo "File \"$file\" exists but is not a symbolic link. Will not remove."
	else
		echo "File \"$file\" not found. Skipping..."
	fi
done