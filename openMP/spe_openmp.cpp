#include<iostream>
#include<sys/time.h>
#include<fstream>
#include<sstream>
#include <ctime>
#include <ratio>
#include<omp.h>
using namespace std;
const int col=130,elinenum=22,num_thread=8;
bool parallel=1; //列数、被消元行数
int tmp=0;
const int num_thread=8;
int bytenum=(col-1)/32+1;   //每个实例中的byte型数组数
class bitmatrix{
public:
    int mycol;    //首项
    int *mybyte;    
    bitmatrix(){    //初始化
        mycol=-1;
        mybyte = new int[bytenum];
        for(int i=0;i<bytenum;i++)
            mybyte[i]=0; 
    }
    bool isnull(){  //判断当前行是否为空行
        if(mycol==-1)return 1;
        return 0;
    }   
    void insert(int x){ //数据读入
        if(mycol==-1)mycol=x;
        int a=x/32,b=x%32;
        mybyte[a]|=(1<<b);
    }
    void doxor(bitmatrix b){  
        for(int i=0;i<bytenum;i++)
            mybyte[i]^=b.mybyte[i];
        for(int i=bytenum-1;i>=0;i--)
            for(int j=31;j>=0;j--)
                if((mybyte[i]&(1<<j))!=0){
                    mycol=i*32+j;
                    return;
                }
        mycol=-1;  
    }
};
bitmatrix *eliminer=new bitmatrix[col],*eline=new bitmatrix[elinenum];
void readdata(){
    ifstream ifs;
    ifs.open("eliminer1.txt");  //消元子
    string temp;
    while(getline(ifs,temp)){
        istringstream ss(temp);
        int x;
        int trow=0;
        while(ss>>x){
            if(!trow)trow=x;    //第一个读入元素代表行号
            eliminer[trow].insert(x);
        }
    }
    ifs.close();
    ifstream ifs2;
    ifs2.open("eline1.txt");     //被消元行,读入方式与消元子不同
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
void dowork(){  //消元
    for(int i=0;i<elinenum;i++){
        while(!eline[i].isnull()){  //只要被消元行非空，循环处理
            int tcol = eline[i].mycol;  //被消元行的首项
            if(!eliminer[tcol].isnull())    //如果存在对应消元子
                eline[i].doxor(eliminer[tcol]);
            else{
                eliminer[tcol]=eline[i];    //由于被消元行升格为消元子后不参与后续处理，可以直接用=来浅拷贝
                break;
            }
        }
    }
}
void dowork2(){
    int i,j;
    #pragma omp parallel if(parallel),num_threads(num_thread),private(i,j)
    for(i=col-1;i>=0;i--) 
        if(!eliminer[i].isnull()){
            #pragma omp for 
            for(j=0;j<elinenum;j++){
                if(eline[j].mycol==i)
                    eline[j].doxor(eliminer[i]);
            }
            }
        else{
            #pragma omp barrier
            #pragma omp single
            for(j=0;j<elinenum;j++){
                if(eline[j].mycol==i){
                    eliminer[i]=eline[j];
                    tmp=j+1;
                    break;
                    }
                tmp=j+2;
            }
            #pragma omp for
            for(j=tmp;j<elinenum;j++){
                if(eline[j].mycol==i)
                    eline[j].doxor(eliminer[i]);
            }
            }
}
int main(){
	struct timeval start, end;
   	readdata();
    	gettimeofday(&start, NULL);
	dowork();
	gettimeofday(&end, NULL);
	cout<<1000000*(end.tv_sec-start.tv_sec)+(end.tv_usec-start.tv_usec)<<endl;
    	readdata();
	gettimeofday(&start, NULL);
	dowork2();
	gettimeofday(&end, NULL);
	cout<<1000000*(end.tv_sec-start.tv_sec)+(end.tv_usec-start.tv_usec)<<endl;
    	return 0;
}