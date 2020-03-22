#include <iostream>
#include <mpi.h>

using namespace std;

int main(int argc, char **argv){
    int process_number;
    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &process_number);
    int source_process = 0;
    int destination_process = 1;
    int number;

    switch (process_number) {
        case 0:
            number = 501;
            MPI_Send(&number, 1, MPI_INT, destination_process, 0, MPI_COMM_WORLD);
            printf("Process with number %d sent number %d to the process with number %d \n",
                    source_process, number, destination_process);
            break;
        case 1:
            MPI_Recv(&number, 1, MPI_INT, source_process, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            printf("Process with number %d received number %d from the process with number %d \n",
                   destination_process, number, source_process);
            break;
        default:
            cout << "Wrong process number. No case for it.";
            break;
    }

    MPI_Finalize();
    return 0;
}