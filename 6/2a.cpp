#include <vector>
#include <stdio.h>
#include <stdlib.h>
#include <cmath>
#include <mpi.h>
#include <time.h>
#include <iostream>
#include <string>
#include <unistd.h>

using namespace std;

int *getRandomValuesVector(int numberOfElements);

// Original loop: a[i] = a[i+3] * a[i+6]

int main(int argc, char **argv) {
    int GCD = 1;
    srand(0);
    int processNumber;
    int numberOfProcesses;

    MPI_Init(&argc, &argv);

    MPI_Comm_size(MPI_COMM_WORLD, &numberOfProcesses);
    MPI_Comm_rank(MPI_COMM_WORLD, &processNumber);

    int numberOfElements = 100;
    int chunkSize = numberOfElements / numberOfProcesses;

    int *mainArray;
    int *subArray = new int[chunkSize + GCD];

    int firstOffset = 2;
    int secondOffset = 3;
//    clock_t start, end;
//    start = clock();
    double start, end;
    start = MPI_Wtime();
//    cout << "Process number " << processNumber << " will do a scatter" << endl;
    if (processNumber == 0) {

        mainArray = getRandomValuesVector(numberOfElements + secondOffset);

        // This array will keep numbers in case of overlapping
        int *additionalArray = new int[numberOfProcesses * GCD];
        for (int i = 0; i < numberOfProcesses; ++i) {
            for (int j = 0; j < GCD; ++j) {
                additionalArray[i * GCD + j] = mainArray[chunkSize * (i + 1) + j + GCD];
            }
        }

//        cout << "Process number " << processNumber << " will start a scatter" << endl;
        MPI_Scatter(additionalArray, GCD, MPI_INT, subArray + chunkSize, GCD, MPI_INT, 0, MPI_COMM_WORLD);
    } else {
//        cout << "Process number " << processNumber << " will start a scatter" << endl;
        MPI_Scatter(NULL, GCD, MPI_INT, subArray + chunkSize, GCD, MPI_INT, 0, MPI_COMM_WORLD);
    }

    bool isRootProcess = processNumber == 0;
    MPI_Scatter(isRootProcess ? mainArray + firstOffset : NULL, chunkSize, MPI_INT, subArray, chunkSize, MPI_INT, 0,
                MPI_COMM_WORLD);
//    cout << "Process number " << processNumber << " finished a scatter" << endl;


    for (int i = 0; i < chunkSize; ++i) {
        subArray[i] *= subArray[i + 3];
        usleep(100000);
    }

    //    end = clock();
    end = MPI_Wtime();
    if (processNumber == 0) {
        MPI_Gather(subArray, chunkSize, MPI_INT, mainArray, chunkSize, MPI_INT, 0, MPI_COMM_WORLD);

        cout << endl << "-------------------------------------------------------------------" << endl;
        cout << "Final array: " << endl;
        for (int i = 0; i < numberOfElements; ++i) {
            cout << mainArray[i] << ", ";
        }
        cout << endl;

        double totalTime = ((double) (end - start));
        cout << "Total execution time = " << totalTime << " seconds." << endl;
    } else {
        MPI_Gather(subArray, chunkSize, MPI_INT, NULL, chunkSize, MPI_INT, 0, MPI_COMM_WORLD);
    }

    MPI_Finalize();
    return 0;
}

int *getRandomValuesVector(int numberOfElements) {
    int *randomValuesVector = new int[numberOfElements];
    cout << endl << "-------------------------------------------------------------------" << endl;
    cout << "Initial array: " << endl;
    for (int i = 0; i < numberOfElements; ++i) {
        int randomValue = 1 + rand() % 10;
        randomValuesVector[i] = randomValue;
        cout << randomValue << ", ";
    }
    cout << endl;
    return randomValuesVector;
}
