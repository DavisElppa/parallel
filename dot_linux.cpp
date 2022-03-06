#include <iostream>
#include<sys/time.h>
#include<math.h>
using namespace std;
int main()
{
    int n;
    cin >> n;
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
    //实验结果如下
    //平凡算法和cache优化算法均重复1000遍
    //20  1166  1362    us
    //50    7130  9074   us 
    //100   28125  32509  us
    //500    715066  972804  us
    //1000   2933809  6071994   us
    return 0;
}
