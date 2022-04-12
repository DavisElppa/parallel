#include<iostream>
#include<sys/time.h>
#include <cstdlib>
#include <arm_neon.h>
using namespace std;
const int N=16;
float** A = NULL;
int *R;
int *lp;
void arr_reset(int n) {
	static int last_scale;
	if (A != NULL) {
		for (int i = 0; i < last_scale; i++)
			delete[] A[i];
		delete[] A;
		delete[] R;
		delete[] lp;
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

	R=new int[n];
	for(int i=0;i<n;i++)
	{
		R[i]=-1;
	}

	
	lp=new int[n];
	for(int i=0;i<n;i++)
	{
		for(int j=0;j<n;j++)
		{
			if(A[i][j]!=0)
			{
				lp[i]=j;
				break;
			}
			if(j==n-1)
				lp[i]=n;
		}
	}
}
void spegauss(int n)
{
	for(int i=0;i<n;i++)
	{
		while(lp[i]!=n)
		{
			if(R[lp[i]]!=-1)
			{
				int row=R[lp[i]];
				int col=lp[i];
				float mul=(float)A[i][col]/(float)A[row][col];
				A[i][col]=0.0;
				
				float32x4_t v1 = vmovq_n_f32(mul);
				float32x4_t v0, v2;
				int x;
				for(x=col+1;x<=n-4;x+=4)
				{
					v2 = vld1q_f32(A[row] + x);
					v0 = vld1q_f32(A[i] + x);
					v2 = vmulq_f32(v1, v2);
					v0 = vsubq_f32(v0, v2);
					vst1q_f32(A[i] + x, v0);
				}
				for(x;x<n;x++)
				{
					A[i][x]=A[i][x]-mul*A[row][x];
				}

				
				for(int x=col+1;x<n;x++)
				{
					if(A[i][x]!=0)
					{
						lp[i]=x;
						break;
					}
					if(x==n-1)
						lp[i]=n;
				}
			}
			else
			{
				R[lp[i]]=i;
				break;
			}
		}
	}
}
int main()
{
        arr_reset(N);
        struct timeval start,last;
        gettimeofday(&start,NULL);
        spegauss(N);
        gettimeofday(&last,NULL);
        cout<<"time= "<<1000000*(last.tv_sec-start.tv_sec)+(last.tv_usec-start.tv_usec)<<endl;
        return 0;
}