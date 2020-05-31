#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <mpi.h>
#include <time.h>
#include <iostream>

using namespace std;

void parallelMatrixVectorMult(float *, float *, int, int);

int main(int argc, char **argv) {
    int processNumber;
    int numberOfProcesses;
    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &numberOfProcesses);
    MPI_Comm_rank(MPI_COMM_WORLD, &processNumber);

    int sizeOfMatrix = 2048;
    int numberOfElements = sizeOfMatrix / numberOfProcesses;
    clock_t start,end;

    float *mainMatrix = NULL;
    float *mainVector = NULL;
    float *submatrix = new float[numberOfElements * sizeOfMatrix];
    float *subvector = new float[sizeOfMatrix];

    if (processNumber == 0) {

        // Initialize matrix and vector's values
        mainMatrix = new float[sizeOfMatrix * sizeOfMatrix];
        mainVector = new float[sizeOfMatrix];

        for (int i = 0; i < sizeOfMatrix; ++i) {
            mainVector[i] = i;
        }
//        cout << "Vector values:\n";
//        for (int i = 0; i < sizeOfMatrix; ++i) {
//            cout << mainVector[i] << endl;
//        }

        for (int i = 0; i < sizeOfMatrix; ++i) {
            for (int j = 0; j < sizeOfMatrix; ++j) {
                mainMatrix[i * sizeOfMatrix + j] = i * 2 + j;
            }
        }

//        cout << "Matrix values:\n";
//        for (int i = 0; i < sizeOfMatrix; ++i) {
//            for (int j = 0; j < sizeOfMatrix; ++j) {
//                cout << mainMatrix[i * sizeOfMatrix + j] << " ";
//            }
//            cout << "\n";
//        }
    }

    // Scatter matrix values to other processes
    MPI_Scatter(mainMatrix, numberOfElements * sizeOfMatrix, MPI_FLOAT, submatrix, numberOfElements * sizeOfMatrix, MPI_FLOAT, 0, MPI_COMM_WORLD);

    // Scatter vector values to other processes
    MPI_Scatter(mainVector, numberOfElements, MPI_FLOAT, subvector, numberOfElements, MPI_FLOAT, 0, MPI_COMM_WORLD);

    start = clock();
    parallelMatrixVectorMult(submatrix, subvector, sizeOfMatrix, numberOfElements);
    end = clock();

    // Gather the results to the result vector
    MPI_Gather(subvector, numberOfElements, MPI_FLOAT, mainVector, numberOfElements, MPI_FLOAT, 0, MPI_COMM_WORLD);

    if (processNumber == 0) {
        double totalTime = ((double) (end - start)) / CLOCKS_PER_SEC;
        cout << "Total execution time = " << totalTime << " seconds." << endl;
    }

    MPI_Finalize();
    return 0;
}

void parallelMatrixVectorMult(float *submatrix, float *subvector, int sizeOfMatrix, int numberOfElements) {
    float *arrVector = new float[sizeOfMatrix];

    // MPI_Allgather is an MPI_Gather followed by an MPI_Bcast
    // Gather and broadcast all data
    MPI_Allgather(subvector, numberOfElements, MPI_FLOAT, arrVector, numberOfElements, MPI_FLOAT, MPI_COMM_WORLD);

    for (int i = 0; i < numberOfElements; ++i) {
        subvector[i] = 0;
        for (int j = 0; j < sizeOfMatrix; ++j) {
            subvector[i] = subvector[i] + submatrix[i * sizeOfMatrix + j] * arrVector[j];
        }
    }
}
