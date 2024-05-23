.PHONY: lina-prepare
lina-prepare:
	cd lina; make benchmarks/mri_q/build/cpu_lina_default/linked_opt.bc
	ln -s lina/benchmarks/mri_q/build/cpu_lina_default/linked_opt.bc linked_opt.bc
	# XXX Profiling input/output dataset is hardcoded on this Makefile
	ln -s lina/datasets/mri_q/smaller/input/32_32_32_dataset.bin 32_32_32_dataset.bin

.PHONY: vivado-decide
vivado-decide:
	cd vivado/benchmarks/mri_q/src/sdx_zcu104; make xo
	cp -pr vivado/benchmarks/mri_q/build/sdx_zcu104_hw/_x/_xocc_compile/mri_q/mri_q/solution/.autopilot/db/*.rpt .
