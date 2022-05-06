#include<iostream>
#include<fstream>
#include<sstream>
#include <ctime>
#include <ratio>
#include <chrono>
#include<sys/time.h>
using namespace std;
const int col=85401,elinenum=756; 
int bytenum=(col-1)/32+1; 
//写完的程序可以直接通过WinSP上传，这样改起来就容易了，也不用考虑换行
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
    ifs.open("eliminer11.txt");  //消元子
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
    ifs2.open("eline11.txt"); 
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
void dowork(){ 
    for(int i=0;i<elinenum;i++){
        while(!eline[i].isnull()){  
            int tcol = eline[i].mycol;  
            if(!eliminer[tcol].isnull())   
                eline[i].doxor(eliminer[tcol]);
            else{
                eliminer[tcol]=eline[i];  
                break;
            }
        }
    }
}
int main(){
    readdata();
    struct timeval start,last;
    gettimeofday(&start,NULL);
    dowork();
    gettimeofday(&last,NULL);
    cout<<"time= "<<1000000*(last.tv_sec-start.tv_sec)+(last.tv_usec-start.tv_usec)<<endl;	
    system("pause");
    return 0;
}