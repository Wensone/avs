#include <cmath>
#include <iostream>
#include <sys/time.h>
#include <thread>
#include <chrono>
#include <random>
#include <ctime>

using namespace std;

int n1, m1, n2, m2;
volatile int **one_matrix,
	**two_matrix;
volatile int **result;

void *multiply(int count, int rank){
	int thread_count = count;
	int thread_rank = rank;

	int point_per_proc = n1 / thread_count;
	int lb = thread_rank * point_per_proc;
	int ub = (thread_rank == thread_count - 1) ? (n1 - 1) : (lb + point_per_proc - 1);

	for (int i = lb; i <= ub; i++) {
        for (int j = 0; j < m2; j++) {
            result[i][j] = 0;
            for (int k = 0; k < m1; k++) {
                result[i][j] += (one_matrix[i][k] * two_matrix[k][j]);
            }
        }
    }
}

int mul_matrix(int thread_count, bool method) {
    if(m1 != n2) {
        cout << "Error! m1 != n2" << endl;
        return 1;
    }
	if(method){
		std::thread thread_t[thread_count];
	    for (int i = 0; i < thread_count; i++) {
            thread_t[i] = std::thread(multiply, thread_count, i);
	    }
	    for (int i = 0; i < thread_count; i++)
	        thread_t[i].join();
	    
 	} else {
 		for (int i = 0; i < n1; i++) {
	        for (int j = 0; j < m2; j++) {
	            result[i][j] = 0;
	            for (int k = 0; k < m1; k++) {
	                result[i][j] += (one_matrix[i][k] * two_matrix[k][j]);
	            }
	        }
	    }
 	}
 	return 0;
}

double wtime() {
    struct timeval t = {};
    gettimeofday(&t, NULL);
    return (double) t.tv_sec + (double) t.tv_usec * 1E-6;
}

void fill_matrix(volatile int **matrix, int n, int m){
	for (int i = 0; i < n; ++i) {
		for (int j = 0; j < m; ++j) {
			matrix[i][j] = 5 + rand() % 10;
		}
	}
}

int main(){
	const int loop_n = 100;
	double all_time = 0.0;
	double time;
	double time_v[loop_n];
	int thread_count, max_thread;
	cout.precision(10);

	cout << "Matrix demension: ";
	cin >> n1;
	n2 = m1 = m2 = n1;

    if ((n1 != m2) || (n2 != m1)) {cout << "Incorrect dimension of matrices!" << endl; return 1;}

	one_matrix = new volatile int*[n1];
	for (int j = 0; j < n1; ++j)
		one_matrix[j] = new volatile int[m1];

	two_matrix = new volatile int* [n2];
	for (int j = 0; j < n2; ++j)
		two_matrix[j] = new volatile int [m2];

	fill_matrix(one_matrix, n1, m1);
	fill_matrix(two_matrix, n2, m2);

	result = new volatile int* [n1];
    for(int i = 0; i < n1; i++) {
        result[i] = new int [m2];
    }

	cout << "Max threads: ";
	cin >> max_thread;

	cout << "Calculation ...";
    
    for(int i = 0; i < loop_n; ++i) {
        time = -wtime();
        mul_matrix(0, false);
        time += wtime();
        time_v[i] = time;
        all_time += time;
    }
    double M = all_time / loop_n;
    double D = 0;
    for (int i = 0; i < loop_n; ++i)
        D += pow((time_v[i] - M), 2);
    double S = sqrt(D / loop_n);
    cout << endl << endl << fixed << "Th. [1] Serial:\t\t" << M
         << fixed << " Deviation (" << S << ")" << endl;

	for(thread_count = 2; thread_count <= max_thread; thread_count += 2){
		cout << "Th. [" << thread_count << "] ";

	    all_time = 0.0;
	    for(int i = 0; i < loop_n; ++i) {
			time = -wtime();
			mul_matrix(thread_count, true);
			time += wtime();
			time_v[i] = time;
			all_time += time;
		}
		M = all_time / loop_n;
	    D = 0;
	    for (int i = 0; i < loop_n; ++i)
	        D += pow((time_v[i] - M), 2);
	    S = sqrt(D / loop_n);
	    cout << fixed << "Parallel:\t" << M
	         << fixed << " Deviation (" << S << ")" << endl;

	}
    if (n1 <= 10) {
        cout << endl << "Result: " << endl;
        for (int j = 0; j < n1; j++) {
            for (int k = 0; k < m2; k++) {
                cout << result[j][k] << ' ';
            }
            cout << endl;
        }
    }

	for(int i = 0; i < m2; i++) delete [] result[i];

	return 0;
}