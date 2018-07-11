//
// Created by Milan Bhele on 10/31/2017.
//
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <mpi.h>

#define MSG_FROM_MASTER 1
#define MSG_FROM_WORKER 2

#define MASTER 0

#define MATRIX_SIZE 30
#define MATRIX_ELEMENT_MAX_VALUE 30

//Generate a Random Number between Range
int getRandomNumber(int min, int max)
{
    int result = 0, low = 0, high = 0;

    if (min < max)
    {
        low = min;
        high = max + 1;
    }
    else
    {
        low = max + 1;
        high = min;
    }

    result = (rand() % (high - low)) + low;
    return result;
}

int main(int argc, char** argv)
{
    srand(time(NULL));

    {
		int numOfProcessorsUsed=1;
		int rowsCountPassed=0;
        int rowsForMaster = 0;
        int rowA = getRandomNumber(1, MATRIX_SIZE);
        int colA = getRandomNumber(1, MATRIX_SIZE);
        int rowB = colA;
        int colB = getRandomNumber(1, MATRIX_SIZE);
        int index=0;
        int rankIndex=0;
        int count=0;

        int numOfWorkers, source, destination, messageTag, rows, averageRows, extraRows, offset;

        int i = 0, j = 0, k = 0;

        int matA[rowA][colA];
        int matB[rowB][colB];
        int matC[rowA][colB];

        int size, rank;

        MPI_Status Stat;
        MPI_Init(&argc, &argv);
        MPI_Comm_size(MPI_COMM_WORLD, &size);
        MPI_Comm_rank(MPI_COMM_WORLD, &rank);

        if (size < 2) {
            printf("Need at least two MPI tasks. Quitting...\n");
            MPI_Abort(MPI_COMM_WORLD, 0);
            exit(1);
        }

        if (rank == 0) {
            /* Generate a random A matrix */
            for (i = 0; i < rowA; i++) {
                for (j = 0; j < colA; j++) {
                    matA[i][j] = getRandomNumber(1, MATRIX_ELEMENT_MAX_VALUE);
                }
            }
            /* Generate a random B matrix */
            for (i = 0; i < rowB; i++) {
                for (j = 0; j < colB; j++) {
                    matB[i][j] = getRandomNumber(1, MATRIX_ELEMENT_MAX_VALUE);
                }
            }

            printf("\nMATRIX %s = %d x %d\n", "A", rowA, colA);
            printf("(\n");
            for (i = 0; i < rowA; i++) {
                for (j = 0; j < colA; j++) {
                    printf("%d ", matA[i][j]);
                }
                printf("\n");
            }
            printf(")\n");

            printf("\nMATRIX %s = %d x %d\n", "B", rowB, colB);
            printf("(\n");
            for (i = 0; i < rowB; i++) {
                for (j = 0; j < colB; j++) {
                    printf("%d ", matB[i][j]);
                }
                printf("\n");
            }
            printf(")\n");

            numOfWorkers = size;
            averageRows = rowA / numOfWorkers;
            extraRows = rowA % numOfWorkers;
            offset = 0;
            messageTag = MSG_FROM_MASTER;
            //We let some rows of Matrix A to be multiplied to Matrix B by Master itself
            rowsForMaster = (0 < extraRows) ? averageRows + 1 : averageRows;
            printf("\nFirst %d rows of Matrix A is multiplied by Master i.e. Rank 0 itself with Matrix B", rowsForMaster);
            offset = offset + rowsForMaster;
			
			rowsCountPassed=rowsCountPassed+rowsForMaster;

            //Distribute other remaining rows to slaves
            for (destination = 1; destination <= numOfWorkers - 1; destination++) {
				if (rowsCountPassed==rowA)
				{
					break;
				}
				else
				{	
                    rows = (destination < extraRows) ? averageRows + 1 : averageRows;
                    rowsCountPassed=rowsCountPassed+rows;
                    numOfProcessorsUsed++;
                    MPI_Send(&offset, 1, MPI_INT, destination, messageTag, MPI_COMM_WORLD);
                    MPI_Send(&rows, 1, MPI_INT, destination, messageTag, MPI_COMM_WORLD);
                    printf("\nSending Next %d rows of Matrix A to Slave %d i.e. Rank %d", rows, destination, destination);
                    MPI_Send(&matA[offset][0], rows * colA, MPI_INT, destination, messageTag, MPI_COMM_WORLD);
                    printf("\nSending Matrix B to Slave %d i.e. Rank %d", destination, destination);
                    MPI_Send(&matB, rowB * colB, MPI_INT, destination, messageTag, MPI_COMM_WORLD);
                    offset = offset + rows;
				}
            }
            //Rows of Matrix A multiplied to Matrix B by Master Itself
            for (k = 0; k < colB; k++) {
                for (i = 0; i < rowsForMaster; i++) {
                    matC[i][k] = 0;
                    for (j = 0; j < colA; j++) {
                        matC[i][k] = matC[i][k] + matA[i][j] * matB[j][k];
                    }
                }
            }

            //Receiving replies from Slaves
            messageTag = MSG_FROM_WORKER;
            printf("\n\n");
            for (i = 1; i <= numOfProcessorsUsed-1; i++) {
                source = i;
                MPI_Recv(&offset, 1, MPI_INT, source, messageTag, MPI_COMM_WORLD, &Stat);
                MPI_Recv(&rows, 1, MPI_INT, source, messageTag, MPI_COMM_WORLD, &Stat);
                MPI_Recv(&matC[offset][0], rows * colB, MPI_INT, source, messageTag, MPI_COMM_WORLD, &Stat);
                printf("Received results from Slave %d i.e. Rank %d\n", source, source);
            }

            //Printing result Matrix
            int index=rowsForMaster;
            printf("\nResult Matrix C = %d x %d\n(\n", rowA, colB);
            for (i = 0; i < rowA; i++) {
                if (count<index)
                {
                    printf("Rank %d: ",rankIndex);
                    count++;
                }
                else
                {
                    rankIndex++;
                    printf("Rank %d: ",rankIndex);
                    count++;
                    rows = (rankIndex < extraRows) ? averageRows + 1 : averageRows;
                    index=index+rows;
                }
                for (j = 0; j < colB; j++) {
                    printf("%d   ", matC[i][j]);
                }
                printf("\n");
            }
            printf(")\n");
        } else {
			if (rank<rowA)
			{
                //Done by Slaves
                messageTag = MSG_FROM_MASTER;
                MPI_Recv(&offset, 1, MPI_INT, MASTER, messageTag, MPI_COMM_WORLD, &Stat);
                MPI_Recv(&rows, 1, MPI_INT, MASTER, messageTag, MPI_COMM_WORLD, &Stat);
                MPI_Recv(&matA, rows * colA, MPI_INT, MASTER, messageTag, MPI_COMM_WORLD, &Stat);
                MPI_Recv(&matB, rowB * colB, MPI_INT, MASTER, messageTag, MPI_COMM_WORLD, &Stat);

                for (k = 0; k < colB; k++) {
                    for (i = 0; i < rows; i++) {
                        matC[i][k] = 0;
                        for (j = 0; j < colA; j++) {
                            matC[i][k] = matC[i][k] + matA[i][j] * matB[j][k];
                        }
                    }
                }
                messageTag = MSG_FROM_WORKER;
                MPI_Send(&offset, 1, MPI_INT, MASTER, messageTag, MPI_COMM_WORLD);
                MPI_Send(&rows, 1, MPI_INT, MASTER, messageTag, MPI_COMM_WORLD);
                MPI_Send(&matC, rows * colB, MPI_INT, MASTER, messageTag, MPI_COMM_WORLD);
			}
        }

        MPI_Finalize();
    }
    return 0;
}