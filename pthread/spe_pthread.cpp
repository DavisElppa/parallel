#include<iostream>
#include<fstream>
#include<sys/time.h>
#include<sstream>
#include <ctime>
#include <ratio>
#include <chrono>
#include<pthread.h>
#include<semaphore.h>
using namespace std;
const int col=130,elinenum=8,num_thread=8; 
int bytenum=(col-1)/32+1; 
 typedef struct{
    int t_id;   //线程id
}threadparam_t;
pthread_barrier_t barrier;
pthread_barrier_t barrier2;
pthread_barrier_t barrier3;
class bitmatrix{
public:
    int mycol;  
    int *mybyte;    
    bitmatrix(){  
        mycol=-1;
        mybyte = new int[bytenum];
        for(int i=0;i<bytenum;i++)
            mybyte[i]=0; 
    }
    bool isnull(){  
        if(mycol==-1)return 1;
        return 0;
    }   
    void insert(int x){ 
        if(mycol==-1)mycol=x;
        int a=x/32,b=x%32;
        mybyte[a]|=(1<<b);
    }
};
bitmatrix *eliminer=new bitmatrix[col],*eline=new bitmatrix[elinenum];
void readdata(){
    ifstream ifs;
    ifs.open("eliminer1.txt");  
    string temp;
    while(getline(ifs,temp)){
        istringstream ss(temp);
        int x;
        int trow=0;
        while(ss>>x){
            if(!trow)trow=x;  
            eliminer[trow].insert(x);
        }
    }
    ifs.close();
    ifstream ifs2;
    ifs2.open("eline1.txt");
    int trow=0;
    while(getline(ifs2,temp)){
        istringstream ss(temp);
        int x;
        while(ss>>x){
            eline[trow].insert(x);
        }
        trow++;
    }
    ifs2.close();
}
void *threadfunc(void *param){
    threadparam_t *p=(threadparam_t *)param;
    int t_id=p->t_id;
    for(int i=0;i<elinenum;i++){
        while(!eline[i].isnull()){ 
            int tcol = eline[i].mycol; 
            if(!eliminer[tcol].isnull()){ 
                for(int j=t_id;j<bytenum;j+=num_thread)
                    eline[i].mybyte[j]^=eliminer[tcol].mybyte[j];
            }
            else{
                pthread_barrier_wait(&barrier3);
                    eliminer[tcol]=eline[i];
                break;
            }
            pthread_barrier_wait(&barrier);
            if(t_id==0){
                bool f=1;
                for(int j=bytenum-1;j>=0&&f;j--)
                    for(int k=31;k>=0&&f;k--)
                        if((eline[i].mybyte[j]&(1<<k))!=0){
                            eline[i].mycol=j*32+k;
                            f=0;
                        }
                if(f)eline[i].mycol=-1;
            }
            pthread_barrier_wait(&barrier2);        
        }
    }
    pthread_exit(NULL);
}
void dowork(){ 
    pthread_barrier_init(&barrier,NULL,num_thread);
    pthread_barrier_init(&barrier2,NULL,num_thread);
    pthread_barrier_init(&barrier3,NULL,num_thread);
    //创建线程
    pthread_t handles[num_thread];
    threadparam_t param[num_thread];
    for(int t_id=0;t_id<num_thread;t_id++){
        param[t_id].t_id=t_id;
        pthread_create(&handles[t_id],NULL,threadfunc,(void*)&param[t_id]);
    }
    for(int i=0;i<num_thread;i++)
        pthread_join(handles[i],NULL);
    pthread_barrier_destroy(&barrier);
    pthread_barrier_destroy(&barrier2);
    pthread_barrier_destroy(&barrier3);
}
int main(){
    readdata();
    struct timeval start,last;
    gettimeofday(&start,NULL);
    dowork();
    gettimeofday(&last,NULL);
    cout<<"time= "<<1000000*(last.tv_sec-start.tv_sec)+(last.tv_usec-start.tv_usec)<<endl;	
    return 0;
}