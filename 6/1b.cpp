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

int getMaximumNumberOfProcesses(int numberOfProcesses);

void setDisplacementAndAmounts(int maximumNumberOfProcesses, int *amountsSent, int *displacementSent,
                               int *amountsReceived, int *displacementReceived);


int main(int argc, char **argv) {
    srand(0);
    int GCD = 3;
    int usedProcess;
    int processNumber;
    int numberOfProcesses;

    MPI_Init(&argc, &argv);

    MPI_Comm_size(MPI_COMM_WORLD, &numberOfProcesses);
    MPI_Comm_rank(MPI_COMM_WORLD, &processNumber);

    int numberOfElements = 100;

    int *mainArray;
    int *subArray;

    int firstOffset = 3;
    int secondOffset = 6;
    clock_t start, end;
    start = clock();

    int maximumNumberOfProcesses = getMaximumNumberOfProcesses(numberOfProcesses);
    int chunkSize = numberOfElements / maximumNumberOfProcesses;
    subArray = new int[maximumNumberOfProcesses * 2];
    int *amountsSent = new int[maximumNumberOfProcesses];
    int *displacementSent = new int[maximumNumberOfProcesses];
    int *amountsReceived = new int[maximumNumberOfProcesses];
    int *displacementReceived = new int[maximumNumberOfProcesses];

    setDisplacementAndAmounts(maximumNumberOfProcesses, amountsSent, displacementSent, amountsReceived,
                              displacementReceived);

    int *multiplicationArray = new int[amountsSent[processNumber]];

    if (processNumber == 0) {
        mainArray = getRandomValuesVector(chunkSize * numberOfProcesses + secondOffset);
    }

    for (int i = 0; i < numberOfElements / GCD; ++i) {
        if (processNumber == 0) {
            int position = 0;
            for (int j = 0; j < GCD; ++j) {
                subArray[j * 2] = mainArray[i * GCD + j + firstOffset];
                subArray[j * 2 + 1] = mainArray[i * GCD + j + secondOffset];
            }

        }
        MPI_Scatterv(processNumber == 0 ? subArray : NULL, amountsSent, displacementSent, MPI_INT, multiplicationArray,
                     amountsSent[processNumber],
                     MPI_INT, 0, MPI_COMM_WORLD);


        for (int j = 0; j < amountsReceived[processNumber]; ++j) {
            multiplicationArray[j] = multiplicationArray[j * 2] * multiplicationArray[j * 2 + 1];
            usleep(100000);
        }


        MPI_Gatherv(multiplicationArray, amountsReceived[processNumber], MPI_INT,
                    processNumber == 0 ? mainArray + i * GCD : NULL,
                    amountsReceived, displacementReceived, MPI_INT, 0, MPI_COMM_WORLD);

    }

    end = clock();
    if (processNumber == 0) {
        cout << endl << "-------------------------------------------------------------------" << endl;
        cout << "Final array: " << endl;
        for (int i = 0; i < chunkSize * numberOfProcesses + secondOffset; ++i) {
            cout << mainArray[i] << ", ";
        }
        cout << endl;
        double totalTime = (((double) (end - start)) / CLOCKS_PER_SEC) * 1000;
        cout << "Total execution time = " << totalTime << " miliseconds." << endl;
    }

    MPI_Finalize();
    return 0;
}

void setDisplacementAndAmounts(int maximumNumberOfProcesses, int *amountsSent, int *displacementSent,
                               int *amountsReceived, int *displacementReceived) {
    switch (maximumNumberOfProcesses) {
        case 0:
        case 1:
            amountsSent[0] = 6;
            displacementSent[0] = 0;

            amountsReceived[0] = 3;
            displacementReceived[0] = 0;
            break;
        case 2:
            amountsSent[0] = 4;
            amountsSent[1] = 2;

            displacementSent[0] = 0;
            displacementSent[1] = 4;

            amountsReceived[0] = 2;
            amountsReceived[1] = 1;

            displacementReceived[0] = 0;
            displacementReceived[1] = 2;
            break;
        default:
            amountsSent[0] = 2;
            amountsSent[1] = 2;
            amountsSent[2] = 2;

            displacementSent[0] = 0;
            displacementSent[1] = 2;
            displacementSent[2] = 4;

            amountsReceived[0] = 1;
            amountsReceived[1] = 1;
            amountsReceived[2] = 1;

            displacementReceived[0] = 0;
            displacementReceived[1] = 1;
            displacementReceived[2] = 2;
            break;
    }
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

int getMaximumNumberOfProcesses(int numberOfProcesses) {
    switch (numberOfProcesses) {
        case 0:
        case 1:
            return 1;
        case 2:
            return 2;
        default:
            return 3;
    }
}
