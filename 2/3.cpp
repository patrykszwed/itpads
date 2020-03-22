#include <iostream>
#include <mpi.h>
#include <cstdlib>
#include <ctime>

using namespace std;

void sendNumber(int&, int, int);
void receiveNumber(int&, int, int);
void addNumber(int&, int, int);
int getRandomNumber();

int main(int argc, char **argv) {
    int process_number;
    int number_of_processes;
    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &process_number);
    MPI_Comm_size(MPI_COMM_WORLD, &number_of_processes);
    const int first_process = 0;
    const int last_process = number_of_processes - 1;
    int number;
    int number_to_add;

    if (process_number == first_process) {
        srand(time(NULL));
        number = 0;
        number_to_add = getRandomNumber();
        addNumber(number, number_to_add, first_process);
        sendNumber(number, first_process, first_process + 1);
        receiveNumber(number, last_process, first_process);
    } else if (process_number == last_process) {
        receiveNumber(number, last_process - 1, last_process);
        number_to_add = getRandomNumber();
        addNumber(number, number_to_add, last_process);
        sendNumber(number, last_process, first_process);
    } else {
        receiveNumber(number, process_number - 1, process_number);
        number_to_add = getRandomNumber();
        addNumber(number, number_to_add, process_number);
        sendNumber(number, process_number, process_number + 1);
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

int getRandomNumber() {
    return 1 + (rand() % 100);
}
