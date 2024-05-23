.PHONY: lina-prepare
lina-prepare:
	cd lina; make benchmarks/sgemm/build/cpu_lina_default/linked_opt.bc
	ln -s lina/benchmarks/sgemm/build/cpu_lina_default/linked_opt.bc linked_opt.bc
	# XXX Profiling input/output dataset is hardcoded on this Makefile
	ln -s lina/datasets/sgemm/smallermedium/input/matrix1.txt matrix1.txt
	ln -s lina/datasets/sgemm/smallermedium/input/matrix2t.txt matrix2t.txt

.PHONY: vivado-decide
vivado-decide:
	cd vivado/benchmarks/sgemm/src/sdx_zcu104; make xo
	cp -pr vivado/benchmarks/sgemm/build/sdx_zcu104_hw/_x/_xocc_compile/sgemm/sgemm/solution/.autopilot/db/*.rpt .
