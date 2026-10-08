# Archival build convenience added during repository organization.
CC ?= cc
MPICC ?= mpicc
OMP_CXX ?= c++
CFLAGS ?= -O0 -g -Wall
OMP_FLAGS ?= -fopenmp

.PHONY: cpu pthreads mpi openmp clean
cpu: pthreads mpi openmp
pthreads: build/trapezoid_pthreads build/taskqueue
mpi: build/pi_mpi build/md5_crack
openmp: build/parallel_CYK

build:
	mkdir -p build

build/trapezoid_pthreads: assignments/03-pthreads-integration/src/trapezoid_pthreads.c | build
	$(CC) $(CFLAGS) -pthread $< -o $@

build/taskqueue: assignments/05-pthreads-task-queue/src/taskqueue.c | build
	$(CC) $(CFLAGS) -pthread $< -o $@

build/pi_mpi: assignments/01-mpi-pi/src/pi_mpi.c | build
	$(MPICC) $(CFLAGS) $< -o $@

build/md5_crack: assignments/02-mpi-md5/src/md5_crack.c assignments/02-mpi-md5/support/md5.c assignments/02-mpi-md5/support/md5.h | build
	$(MPICC) $(CFLAGS) -Iassignments/02-mpi-md5/support $(filter %.c,$^) -o $@

build/parallel_CYK: assignments/06-openmp-cyk/src/parallel_CYK.cpp | build
	$(OMP_CXX) -O0 -g -Wall $(OMP_FLAGS) $< -o $@

clean:
	rm -rf build
