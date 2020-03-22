#include <iostream>
#include <mpi.h>

using namespace std;

void sendNumber(int&, int, int);
void receiveNumber(int&, int, int);
void addNumber(int&, int, int);

int main(int argc, char **argv) {
    int process_number;
    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &process_number);
    int first_process = 0;
    int second_process = 1;
    int number;

    switch (process_number) {
        case 0:
            number = 501;
            sendNumber(number, first_process, second_process);
            receiveNumber(number, second_process, first_process);
            break;
        case 1:
            receiveNumber(number, first_process, second_process);
            addNumber(number, 99, second_process);
            sendNumber(number, second_process, first_process);
            break;
        default:
            cout << "Wrong process number. No case for it.";
            break;
    }

    MPI_Finalize();
    return 0;
}

void sendNumber(int& number_to_send, int source_process, int destination_process) {
    MPI_Send(&number_to_send, 1, MPI_INT, destination_process, 0, MPI_COMM_WORLD);
    printf("Process with number %d sent number %d to the process with number %d \n",
           source_process, number_to_send, destination_process);
}

void receiveNumber(int& variable_to_save, int source_process, int destination_process) {
    MPI_Recv(&variable_to_save, 1, MPI_INT, source_process, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
    printf("Process with number %d received number %d from the process with number %d \n",
           destination_process, variable_to_save, source_process);
}

void addNumber(int& variable_to_be_modified, int number_to_add, int process_number) {
    variable_to_be_modified += number_to_add;
    printf("Process with number %d added number %d \n",
           process_number, number_to_add);
}
