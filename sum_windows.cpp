#include <iostream>
#include<time.h>
#include<math.h>
#include<windows.h>
using namespace std;
int n;
double *a;
double sum=0;
void initialize()
{
    a=new double[n];
    for(int i=0;i<n;i++)
    {
        a[i]=rand()/double(RAND_MAX);//生成0-1随机浮点数
    }
}
void func()
{
    sum=0;
    for(int i=0;i<n;i++)
        sum+=a[i];
}
//在这里写优化算法
//优化算法的缺点也很明显，会更改数组中的值，因此无法进行大量重复来测试时间
void func1()
{
    for(int m=n;m>1;m/=2)
    {
        for(int i=0;i<m/2;i++)
        {
            a[i]=a[i*2]+a[i*2+1];
        }
    }
}
int main()
{
    cin>>n;
    initialize();
    //在这里测试平凡算法的时间
    long long head,tail,freq;
    // similar to CLOCKS_PER_SEC
    QueryPerformanceFrequency((LARGE_INTEGER*)&freq);
    QueryPerformanceCounter((LARGE_INTEGER*)&head);
    for(int cnt=0;cnt<1000;cnt++)
    {
        func();
    }
    QueryPerformanceCounter((LARGE_INTEGER*)&tail);
    cout<<"时间"<<(tail-head)*1000/freq<<"毫秒"<<endl;
    //在这里测试优化算法
    QueryPerformanceCounter((LARGE_INTEGER*)&head);
    for(int i=0;i<1000;i++)
    {
        func1();
    }
    QueryPerformanceCounter((LARGE_INTEGER*)&tail);
    cout<<"时间"<<(tail-head)*1000/freq<<"毫秒"<<endl;
    //测试结果
    //20 1.2331ms  1.0486ms
    //50 8.2282ms  6.6957ms
    //100 31.8713ms  28.3559ms
    //500 772.936ms  741.115ms
    //1000  3863.33ms  4200.98ms
    return 0;
}
