all: static dynamic

init: 
	mkdir -p bin
	mkdir -p images/static
	mkdir -p images/static_final
	mkdir -p images/dynamic
	mkdir -p images/dynamic_final
	
static: mandel_static.c
	mpicc mandel_static.c utils.c -o ./bin/mandel_static

dynamic: mandel_dynamic.c
	mpicc mandel_dynamic.c utils.c -o ./bin/mandel_dynamic

example: example.c
	mpicc example.c -o ./bin/example

unittest: unittest.c
	mpicc unittest.c utils.c -o ./bin/unittest

run: 
	mpirun -np 2 --oversubscribe bin/mandel_static
	mpirun -np 3 --oversubscribe bin/mandel_static
	mpirun -np 5 --oversubscribe bin/mandel_static
	mpirun -np 9 --oversubscribe bin/mandel_static
	mpirun -np 2 --oversubscribe bin/mandel_dynamic
	mpirun -np 3 --oversubscribe bin/mandel_dynamic
	mpirun -np 5 --oversubscribe bin/mandel_dynamic
	mpirun -np 9 --oversubscribe bin/mandel_dynamic

run-example:
	mpirun -np 2 bin/example

cleanbin:
	rm -r bin
	mkdir bin
cleandata:
	rm -r images
	mkdir -p images/static
	mkdir -p images/static_final
	mkdir -p images/dynamic
	mkdir -p images/dynamic_final