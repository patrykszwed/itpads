#include <vector>
#include <stdio.h>
#include <stdlib.h>
#include <cmath>
#include <mpi.h>
#include <sstream>
#include <time.h>
#include <iostream>
#include <string>
#include <limits.h>


using namespace std;

class Minimum {
public:
    int value;
    int processNumber;
    int index;

    Minimum(int v, int pn, int idx) {
        value = v;
        processNumber = pn;
        index = idx;
    }
};

void selectionSort(int, int, vector<int> &);
void swapElements(int, int, int, vector<int> &, vector<int> &, Minimum, Minimum);

int main(int argc, char **argv) {
    srand(0);
    int processNumber;
    int numberOfProcesses;

    MPI_Init(&argc, &argv);

    MPI_Comm_size(MPI_COMM_WORLD, &numberOfProcesses);
    MPI_Comm_rank(MPI_COMM_WORLD, &processNumber);

    int numberOfElements = 32;
    int chunkSize = numberOfElements / numberOfProcesses;

    // Initialize and print arrayToSort with random values from 1 to 1000
    vector<int> arrayToSort;
    if (processNumber == 0) {
        cout << endl << "-------------------------------------------------------------------" << endl;
        cout << "Initial array: " << endl;
        for (int i = 0; i < numberOfElements; ++i) {
            int valueToSort = 1 + (rand() % 1000);
            cout << valueToSort << ", ";
            arrayToSort.push_back(valueToSort);
        }
    }

    // Send parts of arrayToSort
    vector<int> subArrayToSort(chunkSize);
    MPI_Scatter(arrayToSort.data(), chunkSize, MPI_INT, subArrayToSort.data(), chunkSize, MPI_INT, 0, MPI_COMM_WORLD);

    // Sort the array
    selectionSort(numberOfElements, processNumber, subArrayToSort);

    // Gather sorted subArrays
    vector<int> sortedArray(numberOfElements);
    MPI_Gather(subArrayToSort.data(), subArrayToSort.size(), MPI_INT, sortedArray.data(), subArrayToSort.size(), MPI_INT, 0, MPI_COMM_WORLD);

    if (processNumber == 0) {
        cout << endl << "-------------------------------------------------------------------" << endl;
        cout << "Sorted elements: ";
        for (int i = 0; i < numberOfElements; ++i) {
            cout << sortedArray.at(i) << ", ";
        }
        cout << endl;
    }

    MPI_Finalize();
    return 0;
}

void selectionSort(int numberOfElements, int processNumber, vector<int> &subArrayToSort) {
    int subArrayToSortSize = subArrayToSort.size();
    int currentProcessNumber = 0, currentElementIndex = 0;

    Minimum localMinimum = Minimum(INT_MAX, processNumber, -1);
    Minimum globalMinimum = Minimum(INT_MAX, processNumber, -1);

    // This vector holds boolean to check if this specific element has already been swapped
    vector<int> isElementSwapped(subArrayToSortSize, 0);

    for (int index = 0; index < numberOfElements; ++index) {
        localMinimum.value = INT_MAX;

        // Calculate currentProcessNumber and currentElementIndex
        if (index != 0) {
            currentProcessNumber = index / subArrayToSortSize;
            currentElementIndex = index % subArrayToSortSize;
        }

        // Find the local minimum
        for (int i = 0; i < subArrayToSortSize; ++i) {
            if (isElementSwapped[i] != 1 && subArrayToSort[i] < localMinimum.value) {
                localMinimum.index = i;
                localMinimum.value = subArrayToSort[i];
            }
        }

        // Find the global minimum
        MPI_Allreduce(&localMinimum, &globalMinimum, 1, MPI_2INT, MPI_MINLOC, MPI_COMM_WORLD);

        if (processNumber == 0) {
            cout << "Process number " << globalMinimum.processNumber << " has a global minimum equal to " << globalMinimum.value << endl;
        }

        // Now, swap the elements
        swapElements(processNumber, currentProcessNumber, currentElementIndex, subArrayToSort, isElementSwapped, globalMinimum, localMinimum);
    }
}

void swapElements(int processNumber, int currentProcessNumber, int currentElementIndex, vector<int> &subArrayToSort, vector<int> &isElementSwapped, Minimum globalMinimum, Minimum localMinimum) {
    int elementToSwap;

    // This is the same currentProcessNumber for which sort was called
    if (processNumber == currentProcessNumber) {
        isElementSwapped[currentElementIndex] = 1;

        // The global minimum is in the current process, just swap the values
        if (currentProcessNumber == globalMinimum.processNumber) {
            elementToSwap = subArrayToSort[currentElementIndex];
            subArrayToSort[localMinimum.index] = elementToSwap;
            subArrayToSort[currentElementIndex] = globalMinimum.value;
            return;
        }

        // The global minimum is in other process, send the value to swap and save globalMinimum's value on that location
        elementToSwap = subArrayToSort[currentElementIndex];
        MPI_Send(&elementToSwap, 1, MPI_INT, globalMinimum.processNumber, 0, MPI_COMM_WORLD);
        subArrayToSort[currentElementIndex] = globalMinimum.value;
        return;
    }

    // The global minimum was in that process, save elementToSwap on the position of previous minimum
    if (processNumber == globalMinimum.processNumber) {
        MPI_Recv(&elementToSwap, 1, MPI_INT, currentProcessNumber, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        subArrayToSort[localMinimum.index] = elementToSwap;
    }
}
