benchmarks/sgemm/build/cpu_lina_default/linked_opt.bc: benchmarks/sgemm/build/cpu_lina_default/linked.bc
	cd benchmarks/sgemm; opt -mem2reg -instnamer -lcssa -indvars build/cpu_lina_default/linked.bc -o build/cpu_lina_default/linked_opt.bc

benchmarks/sgemm/build/cpu_lina_default/linked.bc: benchmarks/sgemm/build/cpu_lina_default/main.bc benchmarks/sgemm/build/cpu_lina_default/sgemm.bc benchmarks/sgemm/build/cpu_lina_default/io.bc benchmarks/sgemm/build/cpu_lina_default/parboil.bc
	cd benchmarks/sgemm; llvm-link build/cpu_lina_default/main.bc build/cpu_lina_default/sgemm.bc build/cpu_lina_default/io.bc build/cpu_lina_default/parboil.bc -o build/cpu_lina_default/linked.bc

benchmarks/sgemm/build/cpu_lina_default/main.bc: benchmarks/sgemm/src/cpu_lina/main.c benchmarks/sgemm/src/cpu_lina/sgemm.h
	mkdir -p benchmarks/sgemm/build/cpu_lina_default
	cd benchmarks/sgemm; clang -g -O1 -emit-llvm -c -I../../common/include src/cpu_lina/main.c -o build/cpu_lina_default/main.bc

benchmarks/sgemm/build/cpu_lina_default/sgemm.bc: benchmarks/sgemm/src/cpu_lina/sgemm.c benchmarks/sgemm/src/cpu_lina/sgemm.h
	mkdir -p benchmarks/sgemm/build/cpu_lina_default
	cd benchmarks/sgemm; clang -g -O1 -emit-llvm -c -I../../common/include src/cpu_lina/sgemm.c -o build/cpu_lina_default/sgemm.bc

benchmarks/sgemm/build/cpu_lina_default/io.bc: benchmarks/sgemm/src/cpu_lina/io.c
	mkdir -p benchmarks/sgemm/build/cpu_lina_default
	cd benchmarks/sgemm; clang -g -O1 -emit-llvm -c -I../../common/include src/cpu_lina/io.c -o build/cpu_lina_default/io.bc

benchmarks/sgemm/build/cpu_lina_default/parboil.bc: common/src/parboil.c
	mkdir -p benchmarks/sgemm/build/cpu_lina_default
	cd benchmarks/sgemm; clang -g -O1 -emit-llvm -c -I../../common/include ../../common/src/parboil.c -o build/cpu_lina_default/parboil.bc
