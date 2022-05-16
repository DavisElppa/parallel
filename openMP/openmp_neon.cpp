#include<omp.h>
#include<arm_neon.h>
#include <ctime>
#include <ratio>
#include<iostream>
#include<sys/time.h>
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
void openmp_neon()
{
	int i,j,k;
    #pragma omp parallel if(parallel),num_threads(num_thread),private(i,j,k)
    for(k=0;k<n;k++){
        #pragma omp single    //串行部分
        for(int i=0;i<2;i++){
           if(i==0)
          for(j=k+1;j+4<=n;j+=4){
                float32x4_t vt=vld1q_dup_f32(A[k]+k);
                float32x4_t va =vld1q_f32(A[k]+j);
                va=vdivq_f32(va,vt);
                vst1q_f32(A[k]+j,va);
              }
         else
          for(;j<n;j++)
              A[k][j]=A[k][j]/A[k][k];
         }
         A[k][k]=1.0;
        #pragma omp for     //并行部分
        for(i=k+1;i<n;i++){
           float32x4_t vaik=vld1q_dup_f32(A[i]+k);
               for(j=k+1;j+4<=n;j+=4){
                   float32x4_t vakj=vld1q_f32(A[k]+j);
                  float32x4_t vaij=vld1q_f32(A[i]+j);
                    float32x4_t vx= vmulq_f32(vaik,vakj);
                  vaij = vsubq_f32(vaij,vx);
                 vst1q_f32(A[i]+j,vaij); 
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
		openmp_neon();
		gettimeofday(&end, NULL);
		cout<<"time= "<<1000000*(end.tv_sec-start.tv_sec)+(end.tv_usec-start.tv_usec)<<endl;
		cout<<endl;
		n*=2;
	}
	return 0;
}


