#include <stdio.h>
#include<semaphore.h>
#include <sys/time.h>
#include<iostream>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <xmmintrin.h> //SSE
#include <emmintrin.h> //SSE2
#include <pmmintrin.h> //SSE3
#include <tmmintrin.h> //SSSE3
#include <smmintrin.h> //SSE4.1
#include <nmmintrin.h> //SSSE4.2
#include <immintrin.h> //AVX¡¢AVX2
using namespace std;
float**A=NULL;
float**B=NULL;
int n=32;
void normal(float** a, int n)
{
    int i, j, k;
    for (k = 0; k < n; k++)
    {
        for (j = k + 1; j < n; j++)
        {
            a[k][j] = a[k][j] / a[k][k];
        }
        a[k][k] = 1.0;
        for (i = k + 1; i < n; i++)
        {
            for (j = k + 1; j < n; j++)
            {
                a[i][j] = a[i][j] - a[i][k] * a[k][j];
	    }
            a[i][k] = 0;
        }
    }
}
struct threadParam_t
{
    int k;
    int t_id;
int t;
};
void* threadFunc(void* param)
{
    threadParam_t* p = (threadParam_t*)param;
    int k = p->k;
    int t_id = p->t_id;
	int t=p->t;
    int i = k + t_id*t+ 1;
    int j;
for(;i<k+t_id*t+1+t;i++)
{
     __m256 t1;
            __m256 t2;
            for (j = k + 1; j % 8 != 0 && j < n; j++)
            {
                A[i][j] = A[i][j] - A[i][k] * A[k][j];
            }
            for (; j <= n - 8; j += 8)
            {
                t1 = _mm256_load_ps(A[i] + j);
                t2 = _mm256_load_ps(A[k] + j);
                __m256 t3 = _mm256_set1_ps(A[i][k]);
                t3 = _mm256_mul_ps(t3, t2);
                t1 = _mm256_sub_ps(t1, t3);
                _mm256_store_ps(A[i] + j, t1);
            }
            for (; j < n; j++)
            {
                A[i][j] = A[i][j] - A[i][k] * A[k][j];
            }
            A[i][k] = 0;
}
    pthread_exit(NULL);
}
void m_rest(float** A, int n)
{
    for (int i = 0; i < n; i++)
    {
        for (int j = 0; j < i; j++)
            A[i][j] = 0;
        A[i][i] = 1.0;
        for (int j = i + 1; j < n; j++)
        {
            float x = rand();
            while (x == 0) x = rand();
            A[i][j] = x;
        }
    }
    for (int k = 0; k < n; k++)
        for (int i = k + 1; i < n; i++)
            for (int j = 0; j < i; j++)
                A[i][j] += A[k][j];
}
int main()
{
while(n<=1600)
{
    srand((unsigned)time(NULL));
         A = (float**)aligned_alloc(32, n * sizeof(float**));
         B = (float**)aligned_alloc(32, n * sizeof(float**));
        struct timeval tstart, tend;
        double timeUsed0=0.0, timeUsed1 = 0.0;
        for (int i = 0; i < n; i++)A[i] = (float*)aligned_alloc(32, n * sizeof(float*));
        for (int i = 0; i < n; i++)B[i] = (float*)aligned_alloc(32, n * sizeof(float*));
        m_rest(A, n);
        m_rest(B, n);
        gettimeofday(&tstart, NULL);
        normal(B, n);
        gettimeofday(&tend, NULL);
        timeUsed0 = 1000000 * (tend.tv_sec - tstart.tv_sec) + tend.tv_usec - tstart.tv_usec;
        gettimeofday(&tstart, NULL);
    int j;
            for (int k = 0; k < n; ++k)
            {
                  __m256 temp ;
        __m256 t = _mm256_set1_ps(A[k][k]);
        for(j = k + 1; j % 8 != 0 && j < n; j++)
        {
            A[k][j] = A[k][j] / A[k][k];
        }
        for (; j <= n - 8; j += 8)
        {

            temp = _mm256_load_ps(A[k] + j);
            temp = _mm256_div_ps(temp, t);
            _mm256_store_ps(A[k] + j, temp);
        }
        for (; j < n; j++)
        {
            A[k][j] = A[k][j] / A[k][k];
        }
        A[k][k] = 1.0;
                int work_count = 8;
	int t1= (n - k - 1)/work_count;
                pthread_t* handles = new pthread_t[work_count];
                threadParam_t* param = new threadParam_t[work_count];
		void *status;
                for (int t_id = 0; t_id < work_count; t_id++)
                {
                    param[t_id].k = k;
                    param[t_id].t_id = t_id;
		        param[t_id].t=t1;
                }
                for (int t_id = 0; t_id < work_count; t_id++)
                {
                    pthread_create(&handles[t_id],NULL,threadFunc,(void*)&param[t_id]);
                }
                for (int t_id = 0; t_id < work_count; t_id++)
                {
                    pthread_join(handles[t_id],NULL);
                }
            }
        gettimeofday(&tend, NULL);
        timeUsed1 = 1000000 * (tend.tv_sec - tstart.tv_sec) + tend.tv_usec - tstart.tv_usec;
        cout <<timeUsed0<<" "<<timeUsed1<<" "<< timeUsed0 / timeUsed1 * 100 << "%" << endl;
	n*=2;
}
}