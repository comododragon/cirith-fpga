.PHONY: lina-prepare
lina-prepare:
	cd lina; make benchmarks/lbm_small/build/cpu_lina_default/linked_opt.bc
	ln -s lina/benchmarks/lbm_small/build/cpu_lina_default/linked_opt.bc linked_opt.bc
	# XXX Profiling input/output dataset is hardcoded on this Makefile
	ln -s lina/datasets/lbm_small/short/input/120_120_30_ldc.of 120_120_30_ldc.of

.PHONY: vivado-decide
vivado-decide:
	cd vivado/benchmarks/lbm_small/src/sdx_zcu104; make xo
	cp -pr vivado/benchmarks/lbm_small/build/sdx_zcu104_hw/_x/_xocc_compile/lbm_small/lbm_small/solution/.autopilot/db/*.rpt .
