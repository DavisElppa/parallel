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
    n=20;
    initialize();
    struct timeval start,last;
    gettimeofday(&start,NULL);
    for(int i=0;i<1000;i++)
    {
        func1();
    }
    gettimeofday(&last,NULL);
    cout<<"time= "<<1000000*(last.tv_sec-start.tv_sec)+(last.tv_usec-start.tv_usec)<<endl;
    gettimeofday(&start,NULL);
     for(int cnt=0;cnt<1000;cnt++)
    {
        func();
    }
    gettimeofday(&last,NULL);
    cout<<"time= "<<1000000*(last.tv_sec-start.tv_sec)+(last.tv_usec-start.tv_usec)<<endl;
    return 0;
}
