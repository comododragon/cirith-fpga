benchmarks/mri_q/build/cpu_lina_default/linked_opt.bc: benchmarks/mri_q/build/cpu_lina_default/linked.bc
	cd benchmarks/mri_q; opt -mem2reg -instnamer -lcssa -indvars build/cpu_lina_default/linked.bc -o build/cpu_lina_default/linked_opt.bc

benchmarks/mri_q/build/cpu_lina_default/linked.bc: benchmarks/mri_q/build/cpu_lina_default/main.bc benchmarks/mri_q/build/cpu_lina_default/mri_q.bc benchmarks/mri_q/build/cpu_lina_default/file.bc benchmarks/mri_q/build/cpu_lina_default/parboil.bc
	cd benchmarks/mri_q; llvm-link build/cpu_lina_default/main.bc build/cpu_lina_default/mri_q.bc build/cpu_lina_default/file.bc build/cpu_lina_default/parboil.bc -o build/cpu_lina_default/linked.bc

benchmarks/mri_q/build/cpu_lina_default/main.bc: benchmarks/mri_q/src/cpu_lina/main.c benchmarks/mri_q/src/cpu_lina/mri_q.h benchmarks/mri_q/src/cpu_lina/file.h
	mkdir -p benchmarks/mri_q/build/cpu_lina_default
	cd benchmarks/mri_q; clang -g -O1 -emit-llvm -c -I../../common/include src/cpu_lina/main.c -o build/cpu_lina_default/main.bc

benchmarks/mri_q/build/cpu_lina_default/mri_q.bc: benchmarks/mri_q/src/cpu_lina/mri_q.c benchmarks/mri_q/src/cpu_lina/mri_q.h benchmarks/mri_q/src/cpu_lina/sincoslt.h
	mkdir -p benchmarks/mri_q/build/cpu_lina_default
	cd benchmarks/mri_q; clang -g -O1 -emit-llvm -c -I../../common/include src/cpu_lina/mri_q.c -o build/cpu_lina_default/mri_q.bc

benchmarks/mri_q/build/cpu_lina_default/file.bc: benchmarks/mri_q/src/cpu_lina/file.cc benchmarks/mri_q/src/cpu_lina/file.h
	mkdir -p benchmarks/mri_q/build/cpu_lina_default
	cd benchmarks/mri_q; clang -g -O1 -emit-llvm -c -I../../common/include src/cpu_lina/file.cc -o build/cpu_lina_default/file.bc

benchmarks/mri_q/build/cpu_lina_default/parboil.bc: common/src/parboil.c
	mkdir -p benchmarks/mri_q/build/cpu_lina_default
	cd benchmarks/mri_q; clang -g -O1 -emit-llvm -c -I../../common/include ../../common/src/parboil.c -o build/cpu_lina_default/parboil.bc
