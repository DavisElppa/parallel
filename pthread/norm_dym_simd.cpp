#include <arm_neon.h>
#include<pthread.h>
#include <iostream>
#include <cstdlib>
#include <sys/time.h>
using namespace std;
const int N=2048;
int  n=32;
int count_T = 8;
float** A = NULL;
typedef struct {
	int k;
 	int t_id;
}threadParam_t;
void* threadFunc(void *param) {
	threadParam_t* p = (threadParam_t*)param;
 	int k = p->k;
	int i = k + p->t_id + 1;
	for (; i < n; i += count_T)
	{
		float32x4_t v1 = vmovq_n_f32(A[i][k]);
			float32x4_t v0, v2;
			int j = k + 1;
			for (; j <= n - 4; j += 4)
			{
				v2 = vld1q_f32(A[k] + j);
				v0 = vld1q_f32(A[i] + j);
				v2 = vmulq_f32(v1, v2);
				v0 = vsubq_f32(v0, v2);
				vst1q_f32(A[i] + j, v0);
			}
			for (; j < n; j++)
				A[i][j] = A[i][j] - A[i][k] * A[k][j];
			A[i][k] = 0.0;
	}
	pthread_exit(NULL);
}
void arr_reset(int n) {
	static int last_scale;
	if (A != NULL) {
		for (int i = 0; i < last_scale; i++)
			delete[] A[i];
		delete[] A;
	}
	last_scale = n;
	A = new float* [n];
	srand(time(0));
	for(int i = 0; i < n; i++) {
		A[i] = new float [n];
		for(int j = 0; j < i; j++)
			A[i][j] = 0;
		A[i][i] = 1.0;
		for(int j = i + 1; j < n; j++)
			A[i][j] = rand();
	}
	for(int k = 0; k < n; k++)
		for(int i = k + 1; i < n; i++)
			for(int j = 0; j < n; j++)
				A[i][j] += A[k][j];
}
int main()
{
	struct timeval start,last;
    while (n <= N) {
    	cout << "scale: " << n << endl;
	arr_reset(n);
	gettimeofday(&start,NULL);
	for (int k = 0; k < n; k++)
	{
		for (int j = k + 1; j < n; j++)
			A[k][j] = A[k][j] / A[k][k];
		A[k][k] = 1.0;
		int worker_count = count_T;
		pthread_t *handles = new pthread_t[worker_count];
		threadParam_t*param = new threadParam_t[worker_count];
		for (int t_id = 0; t_id < worker_count; t_id++)
		{
			param[t_id].k = k;
			param[t_id].t_id = t_id;
		}
		for (int t_id = 0; t_id < worker_count; t_id++)
			pthread_create(handles + t_id, NULL, threadFunc, param + t_id);
		for (int t_id = 0; t_id < worker_count; t_id++)
			pthread_join(handles[t_id], NULL);
		delete[] handles;
		delete[] param;
	}
	gettimeofday(&last,NULL);
    	cout<<"time= "<<1000000*(last.tv_sec-start.tv_sec)+(last.tv_usec-start.tv_usec)<<endl;	
	n*=2;
	cout<<endl;	
    }
    return 0;
}