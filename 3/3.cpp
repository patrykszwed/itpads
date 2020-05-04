#include <iostream>
#include <mpi.h>
#include <cstdlib>
#include <ctime>
#include <cmath>
#include <cstring>

using namespace std;

void sendMessage(int*, int, int, int);
void receiveMessage(int*, int, int, int);
void allToAll(int *inMsg, int *outMsg, int process_number, int number_of_processes);

int main(int argc, char **argv) {
    int process_number;
    int number_of_processes;
    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &process_number);
    MPI_Comm_size(MPI_COMM_WORLD, &number_of_processes);
    int *inMsg = new int[number_of_processes];
    int *outMsg = new int[number_of_processes];

    for (int i = 0; i < number_of_processes; ++i) {
        inMsg[i] = process_number * 1000 + i;
    }

    allToAll(inMsg, outMsg, process_number, number_of_processes);

    for(int i = 0; i < number_of_processes; ++i) {
        printf("Process %d) %d\n", process_number, outMsg[i]);
    }

    MPI_Finalize();
    return 0;
}

void allToAll(int *inMsg, int *outMsg, int process_number, int number_of_processes){
    memcpy(outMsg, inMsg, number_of_processes * sizeof(int));   // allocate memory
    int send_to_process = (process_number + 1) % number_of_processes;   // right process
    int receive_from_process = (((process_number -1) % number_of_processes) +
            number_of_processes) % number_of_processes;   // left process
    int process_index = process_number;

    for (int i = 1; i < number_of_processes; ++i) {
        int number_of_elements = number_of_processes - i;   // number of elements to send
        int *elements_to_send = new int[number_of_elements];
        int element_index = 0;

        for (int j = 0; j <= number_of_elements; ++j) {  // copy all elements except for this specific
            if (j != process_index) {
                elements_to_send[element_index] = inMsg[j];
                element_index++;
            }
        }

        process_index = (process_number - i) > 0 ? (process_number - i) : 0;
        int process_from_index = (((process_number - i) % number_of_processes) + number_of_processes) %
                                 number_of_processes; // number of processor from which data was received
        sendMessage(elements_to_send, number_of_elements, process_number, send_to_process);
        receiveMessage(inMsg, number_of_elements, receive_from_process, process_number);
        outMsg[process_from_index] = inMsg[process_index];
    }
}

void sendMessage(int* message_to_send, int number_of_elements, int source_process, int destination_process) {
    printf("Process with number %d will send a message to the process with number %d \n",
           source_process, destination_process);
    MPI_Send(message_to_send, number_of_elements, MPI_INT, destination_process, 0, MPI_COMM_WORLD);
    printf("Process with number %d sent message to the process with number %d \n",
           source_process, destination_process);
}

void receiveMessage(int* variable_to_save, int number_of_elements, int source_process, int destination_process) {
    printf("Process with number %d will receive a message from the process with number %d \n",
           source_process, destination_process);
    MPI_Recv(variable_to_save, number_of_elements, MPI_INT, source_process, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
    printf("Process with number %d received a message from the process with number %d \n",
           source_process, destination_process);
}
