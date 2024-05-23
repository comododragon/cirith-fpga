benchmarks/histo/build/base_lina_default/linked_opt.bc: benchmarks/histo/build/base_lina_default/linked.bc
	cd benchmarks/histo; opt -mem2reg -instnamer -lcssa -indvars build/base_lina_default/linked.bc -o build/base_lina_default/linked_opt.bc

benchmarks/histo/build/base_lina_default/linked.bc: benchmarks/histo/build/base_lina_default/main.bc benchmarks/histo/build/base_lina_default/util.bc benchmarks/histo/build/base_lina_default/parboil.bc
	cd benchmarks/histo; llvm-link build/base_lina_default/main.bc build/base_lina_default/util.bc build/base_lina_default/parboil.bc -o build/base_lina_default/linked.bc

benchmarks/histo/build/base_lina_default/main.bc: benchmarks/histo/src/base_lina/main.c benchmarks/histo/src/base_lina/util.h benchmarks/histo/src/base_lina/bmp.h
	mkdir -p benchmarks/histo/build/base_lina_default
	cd benchmarks/histo; clang -g -O1 -emit-llvm -c -I../../common/include src/base_lina/main.c -o build/base_lina_default/main.bc

benchmarks/histo/build/base_lina_default/util.bc: benchmarks/histo/src/base_lina/util.c benchmarks/histo/src/base_lina/util.h benchmarks/histo/src/base_lina/bmp.h
	mkdir -p benchmarks/histo/build/base_lina_default
	cd benchmarks/histo; clang -g -O1 -emit-llvm -c -I../../common/include src/base_lina/util.c -o build/base_lina_default/util.bc

benchmarks/histo/build/base_lina_default/parboil.bc: common/src/parboil.c
	mkdir -p benchmarks/histo/build/base_lina_default
	cd benchmarks/histo; clang -g -O1 -emit-llvm -c -I../../common/include ../../common/src/parboil.c -o build/base_lina_default/parboil.bc
