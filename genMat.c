#include <stdio.h>
#include <stdlib.h>
int arow, acol, brow, bcol;
void generateMatrix()
{
	FILE *fptr;
	//Creating the Matrix A
	printf("Enter the row size for martix A(<1001):");
	scanf("%d", &arow);
	printf("Enter the column size for martix A(<1001):");
	scanf("%d", &acol);
	fptr = fopen("matrixA.txt", "w");
	for (int i = 0; i < arow; i++)
	{
		for (int j = 0; j < acol; j++)
		{
			fprintf(fptr, "%d ", rand() % 10);
		}
		fprintf(fptr, "\n");
	}
	fclose(fptr);

	//Creating the Matrix B
	printf("Enter the row size for martix B(<1001):");
	scanf("%d", &brow);
	printf("Enter the column size for martix B(<1001):");
	scanf("%d", &bcol);
	fptr = fopen("matrixB.txt", "w");
	for (int i = 0; i < brow; i++)
	{
		for (int j = 0; j < bcol; j++)
		{
			fprintf(fptr, "%d ", rand() % 10);
		}
		fprintf(fptr, "\n");
	}
	fclose(fptr);
}

int main()
{
	generateMatrix();
	//matrixMultiplication();
	return 0;
}