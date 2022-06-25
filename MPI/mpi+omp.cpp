#include "mpi.h" 
#include <iostream> 
#include<algorithm>
#include<omp.h>
using namespace std;
const int n = 64, mpisize = 8,num_thread=8;
float A[n][n], time1;
int main(int argc, char* argv[])
{
	int rank;
	double st, ed;
	MPI_Status status;
	MPI_Init(0, 0);
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);	//获取当前进程号
	int r1 = rank * (n / mpisize), r2 = (rank == mpisize - 1) ? n - 1 : (rank + 1)*(n / mpisize) - 1;
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
			int t1 = j * (n / mpisize), t2 = (j == mpisize - 1) ? n - 1 : (j + 1)*(n / mpisize) - 1;
			MPI_Send(&A[t1][0], n*(t2 - t1 + 1), MPI_FLOAT, j, n + 1, MPI_COMM_WORLD);
		}
	}
	else
		MPI_Recv(&A[r1][0], n*(r2 - r1 + 1), MPI_FLOAT, 0, n + 1, MPI_COMM_WORLD, &status);
	MPI_Barrier(MPI_COMM_WORLD);	//各进程同步
	st = MPI_Wtime();	 //计时
	int i, j, k;
	#pragma omp parallel if(parallel),num_threads(num_thread),private(i,j,k)
	for (k = 0; k < n; k++) {
		if (rank == 0) {	//0号进程负责除法部分
			#pragma omp single    //串行部分
			for ( j = k + 1; j < n; j++)
				A[k][j] /= A[k][k];
			A[k][k] = 1.0;
			for ( j = 1; j < mpisize; j++)
				MPI_Send(&A[k][0], n, MPI_FLOAT, j, k + 1, MPI_COMM_WORLD);
		}
		else
			MPI_Recv(&A[k][0], n, MPI_FLOAT, 0, k + 1, MPI_COMM_WORLD, &status);
		if (r2 >= k + 1) {		//负责k+1行之后的各进程并发进行减法
			#pragma omp for
			for ( i = max(k + 1, r1); i <= r2; i++) {
				for ( j = k + 1; j < n; j++)
					A[i][j] -= A[i][k] * A[k][j];
				A[i][k] = 0;
				if (i == k + 1 && rank != 0)
					MPI_Send(&A[i][0], n, MPI_FLOAT, 0, 0, MPI_COMM_WORLD);	//减法结果广播回0号进程
			}
		}
		//MPI_Barrier(MPI_COMM_WORLD);
		if (rank == 0 && k + 1 > r2&&k + 1 < n)
			MPI_Recv(&A[k + 1][0], n, MPI_FLOAT, MPI_ANY_SOURCE, 0, MPI_COMM_WORLD, &status);
	}
	MPI_Barrier(MPI_COMM_WORLD);	//各进程同步
	ed = MPI_Wtime();	 //计时
	MPI_Finalize();
	if (rank == 0) {	//只有0号进程中有最终结果
		printf("time:  %.4lf\n", ed - st);
	}
	return 0;
}