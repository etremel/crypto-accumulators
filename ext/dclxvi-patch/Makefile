CC=gcc
CFLAGS=-g -std=c99 -fomit-frame-pointer

all: libdclxvi.a

%.o: %.s
	$(CC) $(CFLAGS) -c -o $@ $^

%.o: %.c
	$(CC) $(CFLAGS) -DQHASM -c -o $@ $^

libdclxvi.a: consts.o fp2e_add2.o fp2e_add.o fp2e_conjugate.o fp2e_double2.o \
	fp2e_double.o fp2e_mul_fpe.o fp2e_mul.o fp2e_mulxi.o fp2e_neg2.o fp2e_neg.o \
	fp2e_parallel_coeffmul.o fp2e_short_coeffred.o fp2e_square.o fp2e_sub2.o \
	fp2e_sub.o fp2e_triple2.o fp2e_triple.o fpe_mul.o heap_rootreplaced.o scalar_sub_nored.o \
	cpucycles.o curvepoint_fp.o curvepoint_fp_multiscalar.o final_expo.o fp12e.o fp2e.o fp6e.o fpe.o \
	index_heap.o linefunction.o mul.o mydouble.o optate.o parameters.o scalar.o twistpoint_fp2.o twistpoint_fp2_multiscalar.o
	rm -f libdclxvi.a
	ar -r libdclxvi.a $^

.PHONY: clean

clean:
	rm -f *.o
	rm -f libdclxvi.a
