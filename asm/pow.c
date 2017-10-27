#include <stdio.h>
#include <sys/time.h>
#include <stdlib.h>
#include <math.h>

long POW_asm(int arg1, int arg2) {
    volatile long result = 0;
    __asm__  __volatile__ (
	"mov	r3, r0;"
	"loop:;"
	"subs	r1, #1;"
	"mul.w	r3, r0, r3;"
	"cmp	r1, #1;"
	"bne.n	loop;"
	"mov	r0, r3;"
    : [res] "=r" (result) : [a1] "r" (arg1), [a2] "r" (arg2)
    );
    return result;
}

long POW_c(int a, int n) {
	long npow = 1;
	while(n){
		if(n & 1) npow *= a;
		a *= a;
		n >>= 1;
	}
	return npow;
}

double wtime() {
    struct timeval t;
    gettimeofday(&t, NULL);
    return (double) t.tv_sec + (double) t.tv_usec * 1E-6;
}

int main(int argc, char** argv) {
    int loop_n = 100000;
    double all_time = 0.0;
    double time;
    double time_v[loop_n];
    long res = 0;

	if(argc != 3) {
		perror("argc != 2");
		exit(0);
	}

    for (int i = 0; i < loop_n; ++i) {
        time = -wtime();
        res = POW_asm(atoi(argv[1]), atoi(argv[2]));
        time += wtime();
	time_v[i] = time;
        all_time += time;
    }
    double M = all_time / loop_n;
    double D = 0;
    for (int i = 0; i < loop_n; ++i) {
        D += pow((double) (time_v[i] - M), 2);
    }
    double S = sqrt(D / loop_n);
    printf("Asm Time: %.12f, result: %ld S %.12f\n", M, res, S);
    
	all_time = 0.0;
    res = 0;
    for (int j = 0; j < loop_n; ++j) {
	time = -wtime();
        res = POW_c(atoi(argv[1]), atoi(argv[2]));
        time += wtime();
	time_v[j] = time;
	all_time += time;
    }
    M = all_time / loop_n;
    D = 0;
    for (int i = 0; i < loop_n; ++i) {
        D += pow((double) (time_v[i] - M), 2);
    }
    S = sqrt(D / loop_n);
    printf("C Time:   %.12f, result: %ld S %.12f\n", M, res, S);
    return 0;
}
