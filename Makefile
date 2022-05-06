all: static dynamic


static: mandel_static.c
	mpicc mandel_static.c -o ./bin/mandel_static

dynamic: mandel_dynamic.c
	mpicc mandel_dynamic.c -o ./bin/mandel_dynamic

example: example.c
	mpicc example.c -o ./bin/example

run: 
	mpirun -np 1 bin/mandel_static
	mpirun -np 2 bin/mandel_static
	mpirun -np 4 bin/mandel_static
	mpirun -np 8 bin/mandel_static
	mpirun -np 1 bin/mandel_dynamic
	mpirun -np 2 bin/mandel_dynamic
	mpirun -np 4 bin/mandel_dynamic
	mpirun -np 8 bin/mandel_dynamic

run-example:
	mpirun -np 2 bin/example

clean:
	rm bin/*
	rm output/*