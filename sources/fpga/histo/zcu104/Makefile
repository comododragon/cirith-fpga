.PHONY: lina-prepare
lina-prepare:
	cd lina; make benchmarks/histo/build/base_lina_default/linked_opt.bc
	ln -s lina/benchmarks/histo/build/base_lina_default/linked_opt.bc linked_opt.bc
	# XXX Profiling input/output dataset is hardcoded on this Makefile
	ln -s lina/datasets/histo/default_patched/input/img.bin img.bin

.PHONY: vivado-decide
vivado-decide:
	cd vivado/benchmarks/histo/src/sdx_zcu104; make xo
	cp -pr vivado/benchmarks/histo/build/sdx_zcu104_hw/_x/_xocc_compile/histo/histo/solution/.autopilot/db/*.rpt .
