benchmarks/lbm_small/build/cpu_lina_default/linked_opt.bc: benchmarks/lbm_small/build/cpu_lina_default/linked.bc
	cd benchmarks/lbm_small; opt -mem2reg -instnamer -lcssa -indvars build/cpu_lina_default/linked.bc -o build/cpu_lina_default/linked_opt.bc

benchmarks/lbm_small/build/cpu_lina_default/linked.bc: benchmarks/lbm_small/build/cpu_lina_default/main.bc benchmarks/lbm_small/build/cpu_lina_default/lbm.bc benchmarks/lbm_small/build/cpu_lina_default/parboil.bc
	cd benchmarks/lbm_small; llvm-link build/cpu_lina_default/main.bc build/cpu_lina_default/lbm.bc build/cpu_lina_default/parboil.bc -o build/cpu_lina_default/linked.bc

benchmarks/lbm_small/build/cpu_lina_default/main.bc: benchmarks/lbm_small/src/cpu_lina/main.c benchmarks/lbm_small/src/cpu_lina/config.h benchmarks/lbm_small/src/cpu_lina/lbm.h benchmarks/lbm_small/src/cpu_lina/lbm_1d_array.h benchmarks/lbm_small/src/cpu_lina/main.h
	mkdir -p benchmarks/lbm_small/build/cpu_lina_default
	cd benchmarks/lbm_small; clang -g -O1 -emit-llvm -c -I../../common/include src/cpu_lina/main.c -o build/cpu_lina_default/main.bc

benchmarks/lbm_small/build/cpu_lina_default/lbm.bc: benchmarks/lbm_small/src/cpu_lina/lbm.c benchmarks/lbm_small/src/cpu_lina/config.h benchmarks/lbm_small/src/cpu_lina/lbm.h benchmarks/lbm_small/src/cpu_lina/lbm_1d_array.h
	mkdir -p benchmarks/lbm_small/build/cpu_lina_default
	cd benchmarks/lbm_small; clang -g -O1 -emit-llvm -c -I../../common/include src/cpu_lina/lbm.c -o build/cpu_lina_default/lbm.bc

benchmarks/lbm_small/build/cpu_lina_default/parboil.bc: common/src/parboil.c
	mkdir -p benchmarks/lbm_small/build/cpu_lina_default
	cd benchmarks/lbm_small; clang -g -O1 -emit-llvm -c -I../../common/include ../../common/src/parboil.c -o build/cpu_lina_default/parboil.bc
