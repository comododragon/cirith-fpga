KERNEL=histo
LANG=sdx
PLATFORM=<PLATFORM>
SOURCE=$(LANG)_$(PLATFORM)
TARGET=hw
BUILDPATH=benchmarks/$(KERNEL)/build/$(SOURCE)_$(TARGET)/$(PLATFORM)
DATASET=default
DATASETPATH=datasets/$(KERNEL)/$(DATASET)
# Default mount path is the one mounted for /dev/mmcblk0p1. If non-existent, /dev/null is set
# and user must manually set SDMNTPATH to the mounted first partition of the SD card
SDMNTPATH=$(shell cat /proc/mounts | grep /dev/mmcblk0p1 | cut -d ' ' -f 2)
ifeq ($(SDMNTPATH), )
    SDMNTPATH=/dev/null
endif
OUTFILE=$(shell basename $(DATASETPATH)/output/*)
OUTPUTPATH=benchmarks/$(KERNEL)/run/$(DATASET)

.PHONY: build
build: benchmarks/$(KERNEL)/src/$(SOURCE)
	$(MAKE) -C benchmarks/$(KERNEL)/src/$(SOURCE) PLATFORM=$(PLATFORM) $(TARGET)

.PHONY: export
export: $(BUILDPATH)/sd_card/program.xclbin $(BUILDPATH)/sd_card/$(KERNEL) $(DATASETPATH)/input $(SDMNTPATH)
	@\
		if [ -e $(SDMNTPATH)/.allowed ]; then\
			echo -n "The content of $(BUILDPATH)/sd_card will be copied to $(SDMNTPATH) (with forced overwrite!). Press ENTER to continue or CTRL+C to abort...";\
			read REPLY;\
		else\
			echo "Error: this SD card is not recognised as \"allowed\" by this script in order to proceed.";\
			echo "       This is a protection against overwriting files in an unwanted SD card!";\
			echo "       In order to allow this SD card, verify if you are using the correct SD card in $(SDMNTPATH)";\
			echo "       And generate the \".allowed\" file in the root directory. For example, run:";\
			echo "       $$ touch $(SDMNTPATH)/.allowed";\
			false;\
		fi
	cp -rf $(BUILDPATH)/sd_card/* $(SDMNTPATH)
	cp -rf $(DATASETPATH)/input/* $(SDMNTPATH)
	echo "#!/bin/sh" > $(SDMNTPATH)/execute
	echo "" >> $(SDMNTPATH)/execute
	echo -n "./$(KERNEL) " >> $(SDMNTPATH)/execute
	echo -n "-i $(shell cat $(DATASETPATH)/input/DESCRIPTION | grep "Inputs:" | sed "s/Inputs: //g" | tr -s " " ",") " >> $(SDMNTPATH)/execute
#	echo -n "-o $(shell ls -m $(DATASETPATH)/output | sed "s/, /,/g") " >> $(SDMNTPATH)/execute
	echo -n "-o $(OUTFILE) " >> $(SDMNTPATH)/execute
	echo "$(shell cat $(DATASETPATH)/input/DESCRIPTION | grep "Parameters:" | sed "s/Parameters: //g")" >> $(SDMNTPATH)/execute
	echo "" >> $(SDMNTPATH)/execute
	echo "echo \"---------------------------\"" >> $(SDMNTPATH)/execute
	rm -f $(SDMNTPATH)/$(OUTFILE)
	sync

.PHONY: import
import: $(SDMNTPATH)/$(OUTFILE)
	mkdir -p $(OUTPUTPATH)
	cp -p $(SDMNTPATH)/$(OUTFILE) $(OUTPUTPATH)/$(OUTFILE)

.PHONY: check
check: $(OUTPUTPATH)/$(OUTFILE)
	./benchmarks/$(KERNEL)/tools/compare-output $(DATASETPATH)/output/$(OUTFILE) $(OUTPUTPATH)/$(OUTFILE)

.PHONY: clean
clean:
	rm -r benchmarks/$(KERNEL)/build/$(LANG)_$(PLATFORM)_$(TARGET) benchmarks/$(KERNEL)/run
