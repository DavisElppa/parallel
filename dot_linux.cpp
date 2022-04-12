#include <iostream>
#include<sys/time.h>
#include<math.h>
using namespace std;
int main()
{
    int n;
    n=20;
    double* a = new double[n];
    for(int i=0;i<n;i++){
        a[i] = rand()/(double)(RAND_MAX);
    }
    double** b = new double*[n];
    for(int i=0;i<n;i++){
        b[i] = new double[n];
        for(int j=0;j<n;j++){
            b[i][j] = rand()/(double)(RAND_MAX);
        }
    }
    double* sum = new double[n];
    struct timeval start,last;
    gettimeofday(&start,NULL);
    for(int cnt=0;cnt<1000;cnt++)
    {
        for(int i=0;i<n;i++)
        {
            sum[i]=0;
            for(int j=0;j<n;j++)
            {
                sum[i]+=b[j][i]*a[j];
            }
        }
    }
    gettimeofday(&last,NULL);
    cout<<"time= "<<1000000*(last.tv_sec-start.tv_sec)+(last.tv_usec-start.tv_usec)<<endl;
    gettimeofday(&start,NULL);
    for(int cnt=0;cnt<1000;cnt++)
    {
        for(int i=0;i<n;i++)
        {
            sum[i] = 0.0;
        }
        for(int j = 0; j < n; j++){
            for(int i = 0; i < n; i++){
                sum[i] += b[j][i] * a[j];
            }
        }
    }
    gettimeofday(&last,NULL);
    cout<<"time= "<<1000000*(last.tv_sec-start.tv_sec)+(last.tv_usec-start.tv_usec)<<endl;
    return 0;
}

perf record -e L1-dcache-load-misses,L1-dcache-loads,L1-dcache-stores -g ./dot1