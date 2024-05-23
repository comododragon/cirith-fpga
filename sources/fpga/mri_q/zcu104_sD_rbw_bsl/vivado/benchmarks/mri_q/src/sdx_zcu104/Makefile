# Default tools
CC=aarch64-linux-gnu-gcc
VIVADO=$(XILINX_VIVADO)/bin/vivado
XOCC=$(XILINX_SDX)/bin/xocc

# sanitize_dsa: create a filesystem-friendly name from DSA name
#               $(1): name of DSA
COLON=:
PERIOD=.
UNDERSCORE=_
sanitize_dsa = $(strip $(subst $(PERIOD),$(UNDERSCORE),$(subst $(COLON),$(UNDERSCORE),$(1))))
device2dsa = $(if $(filter $(suffix $(1)),.xpfm),$(shell $(COMMON_REPO)/utility/parsexpmf.py $(1) dsa 2>/dev/null),$(1))
device2sandsa = $(call sanitize_dsa,$(call device2dsa,$(1)))
device2dep = $(if $(filter $(suffix $(1)),.xpfm),$(dir $(1))/$(shell $(COMMON_REPO)/utility/parsexpmf.py $(1) hw 2>/dev/null) $(1),)

# Standard variable. Run "make TARGET=... PLATFORM=... FREQ=..." to customise
TARGET=hw
PLATFORM=<PLATFORM>
DSA = $(call device2sandsa, $(PLATFORM))
CLKID=<CLKID>
SOURCEPATH=../../src/sdx_$(PLATFORM)
BUILDPATH=../../build/sdx_$(PLATFORM)_$(TARGET)
COMMONPATH=../../../../common

# CC compile and link flags
CCFLAGS=-fPIC -I$(COMMONPATH)/include -I$(SOURCEPATH) -O2 -Wall -I$(XILINX_SDX)/runtime/include/1_2/ -I/$(XILINX_SDX)/Vivado_HLS/include/
CCLINKFLAGS=-lm -lxilinxopencl -lpthread -lrt -ldl -lcrypt -L$(XILINX_SDX)/runtime/lib/aarch64

# XOCC compile flags
XOCCFLAGS=-t $(TARGET) --platform $(PLATFORM) --save-temps --clkid $(CLKID) --xp prop:solution.hls_pre_tcl=$(SOURCEPATH)/hls_hook.tcl
BANKFLAGS=<BANKFLAGS>

# By default, auto-pipelining is disabled
AUTOPIPELINING=no
ifeq ($(AUTOPIPELINING), no)
    PIPELININGFLAG=--xp param:compiler.enableAutoPipelining=false
else
    PIPELININGFLAG=
endif

# checkForVivado: check if vivado binary is reachable
define checkForVivado
    $(if $(wildcard $(VIVADO)), , $(error vivado binary not found, please check your Xilinx installation and/or the init script, e.g. /path/to/xilinx/SDx/20xx.x/settings64.sh))
endef

# checkForXilinxVivado: check if XILINX_VIVADO is set
define checkForXilinxVivado
	$(if $(XILINX_VIVADO), , $(error XILINX_VIVADO is not set or empty, perhaps forgot to source the init script, e.g. /path/to/xilinx/SDx/20xx.x/settings64.sh?))
endef

# checkForPlatform: check if PLATFORM is set
define checkForPlatform
	$(if $(PLATFORM), , $(error PLATFORM is empty))
endef

# checkForTarget: check if TARGET is set
define checkForTarget
	$(if $(TARGET), , $(error TARGET is empty. Please set to a compatible target: hw, hw_emu or sw_emu))
endef

# checkForXOCC: check if xocc binary is reachable
define checkForXOCC
    $(if $(wildcard $(XOCC)), , $(error xocc binary not found, please check your Xilinx installation and/or the init script, e.g. /path/to/xilinx/SDx/20xx.x/settings64.sh))
endef

# checkForXilinxSDx: check if XILINX_SDX is set
define checkForXilinxSDx
    $(if $(XILINX_SDX), , $(error XILINX_SDX is not set or empty, perhaps forgot to source the init script, e.g. /path/to/xilinx/SDx/20xx.x/settings64.sh?))
endef

# checkForCC: check if CC binary is reachable
define checkForCC
	$(if $(findstring 0, $(shell which $(CC) &> /dev/null; echo $$?)), , $(error CC binary not found, please check your Xilinx installation and/or the init script, e.g. /path/to/xilinx/SDx/20xx.x/settings64.sh))
endef

# checkForHostBinary: check if stuff for compiling host binary is properly set
define checkForHostBinary
	$(call checkForTarget)
	$(call checkForCC)
	$(call checkForXilinxSDx)
endef

# checkForXclbin: check if stuff for synthesising OpenCL kernels is properly set
define checkForXclbin
	$(call checkForXilinxSDx)
	$(call checkForXOCC)
	$(call checkForTarget)
	$(call checkForPlatform)
endef

# checkForXo: check if stuff for creating OpenCL objects is properly set
define checkForXo
	$(call checkForXilinxVivado)
	$(call checkForVivado)
	$(call checkForTarget)
	$(call checkForPlatform)
endef

# Top-level make command for hw generation
.PHONY: hw
hw: $(BUILDPATH)/$(DSA)/sd_card/mri_q

# Make command for host binary
.PHONY: host
host: $(BUILDPATH)/$(DSA)/mri_q

# Make command for OpenCL synthesis
.PHONY: xclbin
xclbin: $(BUILDPATH)/$(DSA)/program.xclbin

# Make command for OpenCL objects
.PHONY: xo
xo: $(BUILDPATH)/$(DSA)/mri_q.xo

# Copies host executable to SD folder
$(BUILDPATH)/$(DSA)/sd_card/mri_q: $(BUILDPATH)/$(DSA)/mri_q $(BUILDPATH)/$(DSA)/program.xclbin
	$(call checkForTarget)
	cd $(BUILDPATH); cp -p $(DSA)/mri_q $(DSA)/sd_card/mri_q
#	cp aux/* fpga/$(TARGET)/$(DSA)/sd_card

# Compiles host executable
$(BUILDPATH)/$(DSA)/mri_q: clfuncs.c clfuncs.h file.h mri_q.h consts.h sincoslt.h main.c mri_q.c file.cc $(COMMONPATH)/include/parboil.h $(COMMONPATH)/src/parboil.c
	$(call checkForHostBinary)
	mkdir -p $(BUILDPATH)/$(DSA)
	cd $(BUILDPATH); $(CC) $(SOURCEPATH)/mri_q.c $(SOURCEPATH)/main.c $(SOURCEPATH)/clfuncs.c $(SOURCEPATH)/file.cc $(COMMONPATH)/src/parboil.c -o $(DSA)/mri_q $(CCFLAGS) $(CCLINKFLAGS)

# Synthesises OpenCL kernels
$(BUILDPATH)/$(DSA)/program.xclbin: $(BUILDPATH)/$(DSA)/mri_q.xo
	$(call checkForXclbin)
	cd $(BUILDPATH); $(XOCC) $(XOCCFLAGS) $(XOCCLDFLAGS) -lo $(DSA)/program.xclbin $(DSA)/mri_q.xo --sys_config ocl -R2
	# Append two lines to init.sh, responsible for writing onto /etc/profile the XILINX_OPENCL export and cd to /mnt
	cd $(BUILDPATH); echo "\necho -e \"\\\\nexport XILINX_OPENCL=/mnt/embedded_root\" >> /etc/profile" >> $(DSA)/sd_card/init.sh
	cd $(BUILDPATH); echo "echo -e \"cd /mnt\" >> /etc/profile" >> $(DSA)/sd_card/init.sh

# Compiles OpenCL object for the kernel
$(BUILDPATH)/$(DSA)/mri_q.xo: mri_q.cl sincoslt.h consts.h
	$(call checkForXo)
	mkdir -p $(BUILDPATH)/$(DSA)
	cd $(BUILDPATH); $(XOCC) $(BANKFLAGS) $(XOCCFLAGS) -c --messageDb $(DSA)/mri_q.mdb --xp misc:solution_name=_xocc_compile --xp param:compiler.version=31 $(PIPELININGFLAG) $(SOURCEPATH)/mri_q.cl -o $(DSA)/mri_q.xo -R2

# Clean all
.PHONY: clean
clean:
	rm -Irf $(BUILDPATH)
