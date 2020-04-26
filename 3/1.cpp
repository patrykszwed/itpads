#include <iostream>
#include <mpi.h>
#include <cstdlib>
#include <ctime>
#include <cmath>

using namespace std;

void sendMessage(int&, int, int);
void receiveMessage(int&, int, int);
void oneToAllBroadcast(int dimensions, int process_number, int &message);

int main(int argc, char **argv) {
    int process_number;
    int number_of_processes;
    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &process_number);
    MPI_Comm_size(MPI_COMM_WORLD, &number_of_processes);
    int message = process_number == 0 ? 200 : 0;
    int dimensions = 3;

    oneToAllBroadcast(dimensions, process_number, message);
    printf("Processor %d has message %d\n", process_number, message);
    MPI_Finalize();
    return 0;
}

void oneToAllBroadcast(int dimensions, int process_number, int &message) {
    int mask = (int)pow(2, dimensions) - 1;

    for (int i = dimensions - 1; i >= 0; --i) {
        mask = mask ^ (int)pow(2, i);
        if ((process_number & mask) == 0) {
            if ((process_number & (int)pow(2, i)) == 0) {
                int destination_process = process_number ^ (int)pow(2, i);
                sendMessage(message, process_number, destination_process);
            } else {
                int source_process = process_number ^ (int)pow(2, i);
                receiveMessage(message, source_process, process_number);
            }
        }
    }
}

void sendMessage(int& message_to_send, int source_process, int destination_process) {
    printf("Process with number %d will send a message to the process with number %d \n",
           source_process, destination_process);
    MPI_Send(&message_to_send, 1, MPI_INT, destination_process, 0, MPI_COMM_WORLD);
    printf("Process with number %d sent message %d to the process with number %d \n",
           source_process, message_to_send, destination_process);
}

void receiveMessage(int& variable_to_save, int source_process, int destination_process) {
    printf("Process with number %d will receive a message from the process with number %d \n",
           destination_process, source_process);
    MPI_Recv(&variable_to_save, 1, MPI_INT, source_process, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
    printf("Process with number %d received message %d from the process with number %d \n",
           destination_process, variable_to_save, source_process);
}
