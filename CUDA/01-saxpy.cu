#include <stdio.h>

#define N 2048 * 2048 // Number of elements in each vector
#define stride 256*32*80
  

/*
 * Optimize this already-accelerated codebase. Work iteratively,
 * and use nvprof to support your work.
 *
 * Aim to profile `saxpy` (without modifying `N`) running under
 * 20us.
 *
 * Some bugs have been placed in this codebase for your edification.
 */

__global__ void saxpy(int * a, int * b, int * c)
{
    int tid = blockIdx.x * blockDim.x + threadIdx.x;
   
    for (int i = tid; i < N; i += stride)
      c[i] = 2 * a[i] + b[i];
}

int main()
{

    int deviceId;
    int numberOfSMs;

    cudaGetDevice(&deviceId);
    cudaDeviceGetAttribute(&numberOfSMs, cudaDevAttrMultiProcessorCount, deviceId);
    printf("Device ID: %d\tNumber of SMs: %d\n", deviceId, numberOfSMs);
  
    int *a, *b, *c;

    int size = N * sizeof (int); // The total number of bytes per vector

    
    cudaError_t cudaMallocErr;
    cudaMallocErr = cudaMallocManaged(&a, size);
    if(cudaMallocErr != cudaSuccess) printf("Error a: %s\n", cudaGetErrorString(cudaMallocErr));
    
    cudaMallocErr = cudaMallocManaged(&b, size);
    if(cudaMallocErr != cudaSuccess) printf("Error a: %s\n", cudaGetErrorString(cudaMallocErr));
    
    cudaMallocErr = cudaMallocManaged(&c, size);
    if(cudaMallocErr != cudaSuccess) printf("Error a: %s\n", cudaGetErrorString(cudaMallocErr));


    cudaMemPrefetchAsync(a, size, cudaCpuDeviceId);
    cudaMemPrefetchAsync(b, size, cudaCpuDeviceId);
    cudaMemPrefetchAsync(c, size, cudaCpuDeviceId);
    // Initialize memory
    for( int i = 0; i < N; ++i )
    {
        a[i] = 2;
        b[i] = 1;
        c[i] = 0;
    }
    
    cudaMemPrefetchAsync(a, size, deviceId);
    cudaMemPrefetchAsync(b, size, deviceId);
    cudaMemPrefetchAsync(c, size, deviceId);
    
    

    int threads_per_block = 128;
    int number_of_blocks = 32*numberOfSMs;
       
    cudaError_t saxpyErr;
    cudaError_t asyncErr;

    saxpy <<< number_of_blocks, threads_per_block >>> ( a, b, c );
    
    saxpyErr = cudaGetLastError();
    if(saxpyErr != cudaSuccess)printf("Error: %s\n", cudaGetErrorString(saxpyErr));
    
    asyncErr = cudaDeviceSynchronize();
    if(asyncErr != cudaSuccess) printf("Error: %s\n", cudaGetErrorString(asyncErr));


    // Print out the first and last 5 values of c for a quality check
    for( int i = 0; i < 5; ++i )
        printf("c[%d] = %d, ", i, c[i]);
    printf ("\n");
    for( int i = N-5; i < N; ++i )
        printf("c[%d] = %d, ", i, c[i]);
    printf ("\n");

    cudaFree( a ); cudaFree( b ); cudaFree( c );
}

