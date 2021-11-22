#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <pthread.h>

#define LENGTH 2048
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
// Global variables

int n = 100; // dimension of the square matrix
int matrixSection = 0;
int matA[100][100], matB[100][100], prdAB[100][100];

// reading input matrix from file
void readInput()
{

	FILE *fptr;
	int num;
	//Accessing file a.txt and storing value in matrixA
	fptr = fopen("matrixA.txt", "r");
	for (int i = 0; i < n; i++)
	{
		for (int j = 0; j < n; j++)
		{
			fscanf(fptr, "%d", &num);
			matA[i][j] = num;
		}
	}
	fclose(fptr);

	//Accessing file b.txt and storing the value in matrixB
	fptr = fopen("matrixB.txt", "r");
	for (int i = 0; i < n; i++)
	{
		for (int j = 0; j < n; j++)
		{
			fscanf(fptr, "%d", &num);
			matB[i][j] = num;
		}
	}
	fclose(fptr);
}

volatile sig_atomic_t flag = 0;
int sockfd = 0;
char name[32];

void catch_ctrl_c_and_exit(int sig)
{
	flag = 1;
}

void calculator()
{

	long int num = matrixSection;
	int ifrom = (num % 2) * (n / 2);
	int isteps = 0;
	int jfrom = (num / 2) * (n / 2);
	int jsteps = 0;
	//printf("from %d to %d \n", from, to);
	int iStepsLimit;
	int jStepsLimit;
	if (n % 2 == 0)
	{
		iStepsLimit = n / 2;
		jStepsLimit = n / 2;
	}
	else
	{
		if (ifrom == 0)
		{
			iStepsLimit = n / 2;
		}
		else
		{
			iStepsLimit = (n + 1) / 2;
		}
		if (jfrom == 0)
		{
			jStepsLimit = n / 2;
		}
		else
		{
			jStepsLimit = (n + 1) / 2;
		}
	}

	for (int i = ifrom; isteps < iStepsLimit; i++)
	{
		isteps++;
		for (int j = jfrom; jsteps < jStepsLimit; j++)
		{
			jsteps++;
			for (int k = 0; k < n; k++)
			{

				prdAB[i][k] += (matA[i][j] * matB[j][k]); // local to this code lock is not required
			}
		}

		jsteps = 0;
	}
	isteps = 0;
	for (int i = ifrom; isteps < iStepsLimit; i++)
	{
		isteps++;
		for (int k = 0; k < n; k++)
		{
			write(sockfd, &i, sizeof(int));			  // row number
			write(sockfd, &k, sizeof(int));			  // column number
			write(sockfd, &prdAB[i][k], sizeof(int)); // value
		}
	}
	catch_ctrl_c_and_exit(2);
}

int main(int argc, char **argv)
{
	readInput();
	if (argc != 3)
	{
		printf("Usage: %s <port>\n", argv[0]);
		return EXIT_FAILURE;
	}

	char *ip = "192.168.205.2";
	int port = atoi(argv[1]);
	matrixSection = atoi(argv[2]); // the part of matrix this worker is reponsible for

	signal(SIGINT, catch_ctrl_c_and_exit);

	struct sockaddr_in server_addr;

	/* Socket settings */
	sockfd = socket(AF_INET, SOCK_STREAM, 0);
	server_addr.sin_family = AF_INET;
	server_addr.sin_addr.s_addr = inet_addr(ip);
	server_addr.sin_port = htons(port);

	// Connect to Server
	int err = connect(sockfd, (struct sockaddr *)&server_addr, sizeof(server_addr));
	if (err == -1)
	{
		printf("ERROR: connect\n");
		return EXIT_FAILURE;
	}

	while (1)
	{
		// worker stuck in this loop until server writes 0 into s
		int s = 1;
		int receive = read(sockfd, &s, sizeof(int));
		if (receive > 0 && s == 0)
		{
			break; // my added
		}
	}

	// start calculation

	pthread_t calculatorThread;
	if (pthread_create(&calculatorThread, NULL, (void *)calculator, NULL) != 0)
	{
		printf("ERROR: pthread\n");
		return EXIT_FAILURE;
	}

	while (1)
	{
		if (flag)
		{
			printf("\nTerminated\n");
			break;
		}
	}

	close(sockfd);

	return EXIT_SUCCESS;
}
