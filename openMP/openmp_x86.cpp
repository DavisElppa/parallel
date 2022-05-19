#include<omp.h>
#include <ctime>
#include <ratio>
#include<iostream>
#include<sys/time.h>
#include <emmintrin.h>
#include <immintrin.h>
using namespace std;
const int N=2048;
int  n=32;
bool parallel=1;
const int num_thread=8;
float** A = NULL;
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
void normal()//平凡算法
{
    int i, j, k;
    for (k = 0; k < n; k++)
    {
        for (j = k + 1; j < n; j++)
        {
            A[k][j] = A[k][j] / A[k][k];
        }
        A[k][k] = 1.0;
        for (i = k + 1; i < n; i++)
        {
            for (j = k + 1; j < n; j++)
            {
                A[i][j] = A[i][j] - A[i][k] * A[k][j];
            }
            A[i][k] = 0;
        }
    }
}
void openmp()
{
 	int i,j,k;
    #pragma omp parallel if(parallel),num_threads(num_thread),private(i,j,k)
    for(k=0;k<n;k++){
        #pragma omp single    //串行部分
        for(j=k+1;j<n;j++)
            A[k][j]/=A[k][k];
        A[k][k]=1.0;
        #pragma omp for
        for(i=k+1;i<n;i++){
            for(j=k+1;j<n;j++)
                A[i][j]-=A[i][k]*A[k][j];
            A[i][k]=0;
        }
    }
}
void openmp_sse()
{
 	int i,j,k;
    #pragma omp parallel if(parallel),num_threads(num_thread),private(i,j,k)
      for(int k=0;k<n;k++){
          #pragma omp single
          for(int i=0;i<2;i++){
            if(i==0)  
            for(j=k+1;j+4<=n;j+=4){
                __m128 vt= _mm_set1_ps(A[k][k]);
                __m128 va =_mm_loadu_ps(A[k]+j);
                va=_mm_div_ps(va,vt);
                _mm_storeu_ps(A[k]+j,va);
            }
            else
            for(;j<n;j++)
                A[k][j]=A[k][j]/A[k][k];
          }
          A[k][k]=1.0;
          #pragma omp for
          for(i=k+1;i<n;i++){
              __m128 vaik=_mm_set1_ps(A[i][k]);
              int j;
              for(j=k+1;j+4<=n;j+=4){
                  __m128 vakj=_mm_loadu_ps(A[k]+j);
                  __m128 vaij=_mm_loadu_ps(A[i]+j);
                  __m128 vx= _mm_mul_ps(vaik,vakj);
                  vaij = _mm_sub_ps(vaij,vx);
                  _mm_storeu_ps(A[i]+j,vaij); 
              }
              for(;j<n;j++)
                  A[i][j]=A[i][j]-A[i][k]*A[k][j];
              A[i][k]=0;
          }
        }
}
void openmp_avx()
{
 	int i,j,k;
    #pragma omp parallel if(parallel),num_threads(num_thread),private(i,j,k)
      for(int k=0;k<n;k++){
          #pragma omp single
          for(int i=0;i<2;i++){
            if(i==0)  
            for(j=k+1;j+8<=n;j+=8){
                __m256 vt = _mm256_set1_ps(A[k][k]);
                __m256 va =_mm256_loadu_ps(A[k]+j);
                va=_mm256_div_ps(va,vt);
                _mm256_storeu_ps(A[k]+j,va);
            }
            else
            for(;j<n;j++)
                A[k][j]=A[k][j]/A[k][k];
            }
          A[k][k]=1.0;
          #pragma omp for
          for(i=k+1;i<n;i++){
              __m256 vaik=_mm256_set1_ps(A[i][k]);
              int j;
              for(j=k+1;j+8<=n;j+=8){
                  __m256 vakj=_mm256_loadu_ps(A[k]+j);
                  __m256 vaij=_mm256_loadu_ps(A[i]+j);
                  __m256 vx= _mm256_mul_ps(vaik,vakj);
                  vaij = _mm256_sub_ps(vaij,vx);
                  _mm256_storeu_ps(A[i]+j,vaij); 
              }
              for(;j<n;j++)
                  A[i][j]=A[i][j]-A[i][k]*A[k][j];
              A[i][k]=0;
          }
        }
}
int main()
{
	struct timeval start, end;
	while(n<=N)
	{
		cout<<n<<endl;
		arr_reset(n);
        		gettimeofday(&start, NULL);
		normal();
		gettimeofday(&end, NULL);
		cout<<"time= "<<1000000*(end.tv_sec-start.tv_sec)+(end.tv_usec-start.tv_usec)<<endl;
		arr_reset(n);
        		gettimeofday(&start, NULL);
		openmp();
		gettimeofday(&end, NULL);
		cout<<"time= "<<1000000*(end.tv_sec-start.tv_sec)+(end.tv_usec-start.tv_usec)<<endl;
		arr_reset(n);
        		gettimeofday(&start, NULL);
		openmp_sse();
		gettimeofday(&end, NULL);
		cout<<"time= "<<1000000*(end.tv_sec-start.tv_sec)+(end.tv_usec-start.tv_usec)<<endl;
		arr_reset(n);
        		gettimeofday(&start, NULL);
		openmp_avx();
		gettimeofday(&end, NULL);
		cout<<"time= "<<1000000*(end.tv_sec-start.tv_sec)+(end.tv_usec-start.tv_usec)<<endl;
		cout<<endl;
		n*=2;
	}
	return 0;
}
//编译指令
g++ -fopenmp -march=native openmp.cpp -o openmp

