#include <iostream>
#include <windows.h>
#include<time.h>
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
    long long head, tail, freq;
    QueryPerformanceFrequency((LARGE_INTEGER *)&freq);
    QueryPerformanceCounter((LARGE_INTEGER *)&head);
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
    QueryPerformanceCounter((LARGE_INTEGER *)&tail);
    cout << (tail - head) * 1000.0 / freq << "ms" << endl;
    QueryPerformanceCounter((LARGE_INTEGER *)&head);
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
    QueryPerformanceCounter((LARGE_INTEGER *)&tail);
    cout << (tail - head) * 1000.0 / freq << "ms" << endl;
    //实验结果如下
    //平凡算法和cache优化算法均重复1000遍
    //20  1.24  1.0506  ms
    //50  7.6525ms   6.4234ms
    //100 31.7276ms   27.0271ms
    //500   778.474ms  759.766ms
    //1000   3127.93ms   4188.84ms
    return 0;
}
