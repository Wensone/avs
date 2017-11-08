#include <cmath>
#include <iostream>
#include <fstream>
#include <cstdio>
#include <sys/time.h>
#include <cstdlib>
#include <omp.h>

using namespace std;

int n1, m1, n2, m2;
int **one_matrix,
	**two_matrix;


int **mul_matrix(bool method, int thread_count) {
    if(m1 != n2) {
        cout << "Error! m1 != n2" << endl;
        return NULL;
    }

    int **result;
    result = new int*[n1];
    for(int i = 0; i < n1; i++) {
        result[i] = new int[m2];
    }

	if(method){	   
	    omp_set_num_threads(thread_count);
	    int i, j, k;
		#pragma omp parallel for shared(one_matrix, two_matrix, result) private(i, j, k)
	    for (i = 0; i < n1; i++) {
	        for (j = 0; j < m2; j++) {
	            result[i][j] = 0;
	            for (k = 0; k < m1; k++) {
	                result[i][j] += (one_matrix[i][k] * two_matrix[k][j]);
	            }
	        }
	    }
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

 	//for(int i = 0; i < m2; i++) delete [] result[i];

    return result;
}

double wtime() {
    struct timeval t;
    gettimeofday(&t, NULL);
    return (double) t.tv_sec + (double) t.tv_usec * 1E-6;
}

void fill_matrix(int **matrix, int n, int m){
	srand(time(0));
	for (int i = 0; i < n; ++i)
	{
		for (int j = 0; j < m; ++j)
		{
			matrix[i][j] = j+1;//5 + rand() % 10; 
			cout << matrix[i][j] << ' '; 
		}
		cout << endl;
	}
}

int main(){
	const int loop_n = 1;
	double all_time;
	double time;
	double time_v[loop_n];
	int thread_count;
	int **tmp;
	cout.precision(10);

	cout << "Matrix demension one (n m):";
	cin >> n1 >> m1;
	cout << "Matrix demension two (n m):";
	cin >> n2 >> m2;
	
	one_matrix = new int*[n1];
	for (int j = 0; j < n1; ++j)
		one_matrix[j] = new int[m1]; 

	two_matrix = new int*[n2];
	for (int j = 0; j < n2; ++j)
		two_matrix[j] = new int[m2];

	fill_matrix(one_matrix, n1, m1);
	fill_matrix(two_matrix, n2, m2);

	//cout << "Enter thread count:";
	//cin >> thread_count;

	cout << "Calculation ..." << endl;

	for(thread_count = 2; thread_count < 11; thread_count += 2){
		cout << endl << "[ Thread count: " << thread_count << " ]" << endl;

		for(int i = 0; i < loop_n; ++i){
			time = -wtime();
			mul_matrix(false, thread_count);
			time += wtime();
			time_v[i] = time;
			all_time += time;
		}
		double M = all_time / loop_n;
	    double D = 0;
	    for (int i = 0; i < loop_n; ++i)
	        D += pow((double) (time_v[i] - M), 2);
	    double S = sqrt(D / loop_n);
	    cout << fixed << "Default:\t" << M
	    	<< fixed << " (" << S << ")" << endl;

	    all_time = 0.0;
	    for(int i = 0; i < loop_n; ++i){
			time = -wtime();
			tmp = mul_matrix(true, thread_count);
			time += wtime();
			time_v[i] = time;
			all_time += time;
		}
		M = all_time / loop_n;
	    D = 0;
	    for (int i = 0; i < loop_n; ++i)
	        D += pow((double) (time_v[i] - M), 2);
	    S = sqrt(D / loop_n);
	    cout << fixed << "Parallel:\t" << M
	    	<< fixed << " (" << S << ")" << endl;
	}

	for (int j = 0; j < m2; j++) {
        for (int k = 0; k < m1; k++) {
            cout << tmp[j][k] << ' ';
        }
        cout << endl;
	}

	return 0;
}