#include <iostream>
#include<sys/time.h>
using namespace std;
const int N=10;
float m[N][N];
void setData()
{
    for(int i=0;i<N;i++)
    {
        for(int j=0;j<i;j++)
        {
            m[i][j]=0;
        }
        m[i][i]=1.0;
        for(int j=i+1;j<N;j++)
        {
            m[i][j]=rand();
        }
    }
    for(int k=0;k<N;k++)
    {
        for(int i=k+1;i<N;i++)
        {
            for(int j=0;j<N;j++)
            {
                m[i][j]+=m[k][j];
            }
        }
    }
}
int main()
{
    setData();
    struct timeval start,last;
    gettimeofday(&start,NULL);
    for(int k=0;k<N;k++)
    {
        for(int j=k+1;j<N;j++)
        {
            m[k][j]=m[k][j]/m[k][k];
        }
        m[k][k]=1.0;
        for(int i=k+1;i<N;i++)
        {
            for(int j=k+1;j<N;j++)
            {
                m[i][j]=m[i][j]-m[i][k]*m[k][j];
            }
            m[i][k]=0;
        }
    }
    gettimeofday(&last,NULL);
    cout<<"time= "<<1000000*(last.tv_sec-start.tv_sec)+(last.tv_usec-start.tv_usec)<<endl;
    return 0;
}
