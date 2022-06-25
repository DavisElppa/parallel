#include "mpi.h" 
#include <iostream> 
#include<algorithm>
using namespace std;
const int n = 64,mpisize = 8;
float A[n][n], time1;
int main(int argc, char* argv[])
{
	int rank;
	double st, ed;
	MPI_Status status;
	MPI_Init(0, 0);
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);	//获取当前进程号
	if (rank == 0) {	//0号进程实现初始化
		for (int i = 0; i < n; i++) {
			for (int j = 0; j < n; j++)
				A[i][j] = 0;
			A[i][i] = 1.0;
			for (int j = i + 1; j < n; j++)
				A[i][j] = rand() % 1000 + 1;
		}
		for (int k = 0; k < n; k++)
			for (int i = k + 1; i < n; i++)
				for (int j = 0; j < n; j++)
					A[i][j] = int((A[i][j] + A[k][j])) % 1000 + 1.0;
		for (int j = 1; j < mpisize; j++) {
			for(int i= j;i<n;i+=mpisize-1)
			MPI_Send(&A[i][0], n, MPI_FLOAT, j, i, MPI_COMM_WORLD);
		}
	}
	else 
		for(int i=rank;i<n;i+=mpisize-1)
			MPI_Recv(&A[i][0], n, MPI_FLOAT,0, i, MPI_COMM_WORLD, &status);	
	MPI_Barrier(MPI_COMM_WORLD);	//各进程同步
	st = MPI_Wtime();	 //计时
	for (int k = 0; k < n; k++) {	
		if (rank == 0) {	//0号进程负责除法部分
			for (int j = k + 1; j < n; j++)
				A[k][j] /= A[k][k];
			A[k][k] = 1.0;
			for (int j = 1; j < mpisize; j++)
				MPI_Send(&A[k][0], n, MPI_FLOAT, j,n+k + 1, MPI_COMM_WORLD);
		}
		else
			MPI_Recv(&A[k][0], n, MPI_FLOAT, 0, n+k + 1, MPI_COMM_WORLD, &status);
		if (rank != 0) {
			int r2 = rank;
			while (r2 < k + 1)r2 += mpisize - 1;
			for (int i = r2; i < n; i += mpisize - 1) {//负责k+1行之后的各进程并发进行减法
				for (int j = k + 1; j < n; j++)
					A[i][j] -= A[i][k] * A[k][j];
				m1[i][k] = 0;
				if (i == k + 1) 
					MPI_Send(&A[i][0], n, MPI_FLOAT, 0, 0, MPI_COMM_WORLD);	//减法结果广播回0号进程
			}
		}
		else if(k<n-1)
			MPI_Recv(&A[k + 1][0], n, MPI_FLOAT, MPI_ANY_SOURCE, 0, MPI_COMM_WORLD, &status);
	}
	MPI_Barrier(MPI_COMM_WORLD);	//各进程同步
	ed = MPI_Wtime();	 //计时
	MPI_Finalize();
	if (rank == 0) {	//只有0号进程中有最终结果
		printf("time:   %.4lf\n", ed - st);
	}
	return 0;
}
