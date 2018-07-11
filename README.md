# MPI-Matrix-Multiplication

```
HOW TO COMPILE?
------------------------------------------------------
	Please issue the following command:
		mpicc -o matrixmult matrixmult.c -std=c99
		
HOW TO RUN?
------------------------------------------------------
	Please issue the following command:
		mpirun -n 3 ./matrixmult
		
	NOTE: Change the value in above command to change the number of processors to be used. In above example, 3 processors are used.	
	


NOTE:
------------------------------------------------------
As Per Requirement Stated in Class:
-The above program generates two random matrices (i.e. Matrix A and B) of random size (with maximum row/column size of 30) such that these two matrices are multipliable(i.e. AxB is possible).
-The Elements in the above matrices get up to a maximum value of 30.

So, to change the size of random matrices generated and the maximum value of elements in them, you can modify the following 2 lines in matrixmult.c :
	#define MATRIX_SIZE 30
	#define MATRIX_ELEMENT_MAX_VALUE 30
	
SAMPLE OUTPUTS:
------------------------------------------------------
mpirun -n 5 ./matrixmult

MATRIX A = 4 x 5
(
24 25 28 11 17
5 9 2 10 28
23 4 16 19 13
6 18 15 6 14
)

MATRIX B = 5 x 4
(
1 16 8 27
5 27 21 30
3 18 6 26
4 25 28 20
29 29 22 30
)

First 1 rows of Matrix A is multiplied by Master i.e. Rank 0 itself with Matrix B
Sending Next 1 rows of Matrix A to Slave 1 i.e. Rank 1
Sending Matrix B to Slave 1 i.e. Rank 1
Sending Next 1 rows of Matrix A to Slave 2 i.e. Rank 2
Sending Matrix B to Slave 2 i.e. Rank 2
Sending Next 1 rows of Matrix A to Slave 3 i.e. Rank 3
Sending Matrix B to Slave 3 i.e. Rank 3

Received results from Slave 1 i.e. Rank 1
Received results from Slave 2 i.e. Rank 2
Received results from Slave 3 i.e. Rank 3

Result Matrix C = 4 x 4
(
Rank 0: 770   2331   1567   2856
Rank 1: 908   1421   1137   1497
Rank 2: 544   1616   1182   1927
Rank 3: 571   1408   992   1632
)

NOTE :
The Result Matrix C has Rank of Processor at the beginning of each row that did the calculation for that Row.
```
