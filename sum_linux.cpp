#include <iostream>
#include<sys/time.h>
#include<math.h>
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
    struct timeval start,last;
    gettimeofday(&start,NULL);
    for(int cnt=0;cnt<1000;cnt++)
    {
        func();
    }
    gettimeofday(&last,NULL);
    cout<<"time= "<<1000000*(last.tv_sec-start.tv_sec)+(last.tv_usec-start.tv_usec)<<endl;
    //在这里测试优化算法
    gettimeofday(&start,NULL);
    for(int i=0;i<1000;i++)
    {
        func1();
    }
    gettimeofday(&last,NULL);
    cout<<"time= "<<1000000*(last.tv_sec-start.tv_sec)+(last.tv_usec-start.tv_usec)<<endl;
    //测试结果
    //20 37  39  us
    //50 112 167  us
    //100  251 218 us
    //500   1353  1185  us
    //1000  2739 2637  us
    //2000  5448  4910 us
    //3000   8175  7798 us
    //5000   13707  12212us
    return 0;
}
