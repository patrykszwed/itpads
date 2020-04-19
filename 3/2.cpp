#include <iostream>
#include <mpi.h>
#include <cstdlib>
#include <ctime>
#include <cmath>

using namespace std;

int main(int argc, char **argv) {
    int process_number;
    int number_of_processes;
    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &process_number);
    MPI_Comm_size(MPI_COMM_WORLD, &number_of_processes);
    int message = process_number == 0 ? 200 : 0;

    if (process_number == 0) {
        printf("Process with number %d will broadcast a message %d \n",
               0, message);

    } else {
        printf("Process with number %d will receive a message \n",
               process_number);
    }

    MPI_Bcast(&message, 1, MPI_INT, 0, MPI_COMM_WORLD);

    if (process_number == 0) {
        printf("Process with number %d broadcasted a message %d \n",
               0, message);
    } else {
        printf("Process with number %d received a message %d \n",
               process_number, message);
    }

    MPI_Finalize();
    return 0;
}
