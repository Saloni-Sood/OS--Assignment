/* This the bankers algorithm with the use of multithreading
	
	Execute the program like "./a.out 7 7 7" where 	the following no are used to initailize the resource Vector.
For sysnchronization, mutex locks have been used for the request and release methods.
*/
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

#define resourceQuan 3
#define processQuan 5

int i = 0;
int j = 0;
pthread_mutex_t mutex;//mutex lock for access to global variable

int init_ResVector [resourceQuan];
//available, max, allocation, need
int availResourceVector [resourceQuan];
int allocMatrix [processQuan][resourceQuan] = {{1,1,0},{1,3,0},{0,0,2},{0,1,1},{0,2,0}};
int maxMatrix [processQuan][resourceQuan] = {{5,5,5},{3,3,6},{3,5,3},{7,1,4},{7,2,2}};
int needMatrix [processQuan][resourceQuan];

int reqResource(int processID,int requestVector[]);
int relResource(int processID,int releaseVector[]);
int greaterThanNeed(int processID,int requestVector[]);
int enoughToRelease(int processID,int releaseVector[]);
int inSafeMode();
int enoughToAlloc();
void printNeedMatrix();
void printAllocMatrix();
void printAvailable();
void printMaxMatrix();
void printReqOrRelVector(int vec[]);
void *customer(void* customerID);

int main(int argc,char const* argv[])
{

    if(argc!=resourceQuan+1)
    {
        printf("Quantity of the parameter is not correct.\n");
        return -1;
    }
    for(i=0;i<resourceQuan;i++)
    {
        init_ResVector[i]=atoi(argv[i+1]);
        availResourceVector[i]=init_ResVector[i];
    }

	//initialize needMatrix
	for (i = 0; i < processQuan; ++i)
	{
		for (j = 0; j < resourceQuan; ++j)
		{
			needMatrix[i][j] = maxMatrix[i][j] - allocMatrix[i][j];
		}
	}

	printf("Available resources vector is:\n");
	printAvailable();
	
	printf("Max Matrix is:\n");
	printMaxMatrix();

	printf("Initial allocation matrix is:\n");
	printAllocMatrix();

	printf("Initial need matrix is:\n");
	printNeedMatrix();

	pthread_mutex_init(&mutex,NULL);
	pthread_t *tid = malloc(sizeof(pthread_t) * processQuan);
	int *pid = malloc(sizeof(int) * processQuan);//customer's ID,
	//initialize pid and create threads
	for(i = 0; i < processQuan; i++)
	{
		*(pid + i) = i;
		pthread_create((tid+i),NULL, customer, (pid+i));
	}
	//join threads
	for(i = 0; i < processQuan; i++)
	{
		pthread_join(*(tid+i),NULL);
	}
	return 0;
}

void *customer(void* customerID)
{
	int processID = *(int*)customerID;

	
		//request random number of resources
		sleep(1);
		int requestVector[resourceQuan];

		//Because i is global variable, we should lock from here
		pthread_mutex_lock(&mutex);
		for(i = 0; i < resourceQuan; i++)
		{
			if(needMatrix[processID][i] != 0)
			{
				requestVector[i] = rand() % needMatrix[processID][i];
			}
			else
			{
				requestVector[i] = 0;
			}
		}


		printf("Customer %d is trying to request resources:\n",processID);
		printReqOrRelVector(requestVector);
		reqResource(processID,requestVector);
		
		pthread_mutex_unlock(&mutex);

		//release random number of resources
		sleep(1);
		int releaseVector[resourceQuan];
		//Because i is global variable, we should lock from here

		pthread_mutex_lock(&mutex);
		for(i = 0; i < resourceQuan; i++)
		{
			if(allocMatrix[processID][i] != 0)
			{
				releaseVector[i] = rand() % allocMatrix[processID][i];
			}
			else
			{
				releaseVector[i] = 0;
			}
		}
		printf("Customer %d is trying to release resources:\n",processID);
		printReqOrRelVector(releaseVector);
		relResource(processID,releaseVector);

		pthread_mutex_unlock(&mutex);
	
}

int reqResource(int processID,int requestVector[])
{

	if (greaterThanNeed(processID,requestVector) == -1)
	{
		printf("requested resources is bigger than needed.\n");
		return -1;
	}
	printf("Requested resources are not more than needed.\nPretend to allocate...\n");

	if(enoughToAlloc(requestVector) == -1)
	{
		printf("There is not enough resources for this process.\n");
		return -1;
	}

	//pretend allocated
	for (i = 0; i < resourceQuan; ++i)
	{
		needMatrix[processID][i] -= requestVector[i];
		allocMatrix[processID][i] += requestVector[i];
		availResourceVector[i] -= requestVector[i];
	}
	printf("Checking if it is still safe...\n");

	//check if still in safe status
	if (inSafeMode() == 0)
	{
		printf("Safe. Allocated successfully.\nNow available resources vector is:\n");
		printAvailable();
		printf("Now allocated matrix is:\n");
		printAllocMatrix();
		printf("Now need matrix is:\n");
		printNeedMatrix();
		return 0;
	}
	else
	{
		printf("It is not safe. Rolling back.\n");
		for (i = 0; i < resourceQuan; ++i)
		{
			needMatrix[processID][i] += requestVector[i];
			allocMatrix[processID][i] -= requestVector[i];
			availResourceVector[i] += requestVector[i];
		}
		printf("Rolled back successfully.\n");
		return -1;
	}
}

int relResource(int processID,int releaseVector[])
{
	if(enoughToRelease(processID,releaseVector) == -1)
	{
		printf("The process do not own enough resources to release.\n");
		return -1;
	}


	for(i = 0; i < resourceQuan; i++)
	{
		allocMatrix[processID][i] -= releaseVector[i];
		needMatrix[processID][i] += releaseVector[i];
		availResourceVector[i] += releaseVector[i];
	}
	printf("Release successfully.\nNow available resources vector is:\n");
	printAvailable();
	printf("Now allocated matrix is:\n");
	printAllocMatrix();
	printf("Now need matrix is:\n");
	printNeedMatrix();
	return 0;
}

int enoughToRelease(int processID,int releaseVector[])
{
	for (i = 0; i < resourceQuan; ++i)
	{
		if (releaseVector[i] <= allocMatrix[processID][i])
			continue;
		else
			return -1;
	}
	return 0;
}
int greaterThanNeed(int processID,int requestVector[])
{
	for (i = 0; i < resourceQuan; ++i)
	{
		if (requestVector[i] <= needMatrix[processID][i])
			continue;
		else
			return -1;
	}
	return 0;
}

int enoughToAlloc(int requestVector[])
{
	//first element of requestVector is processID
	for (i = 0; i < resourceQuan; ++i)
	{
		if (requestVector[i] <= availResourceVector[i])
			continue;
		else
			return -1;
	}
	return 0;
}

void printNeedMatrix()
{
	for (i = 0; i < processQuan; ++i)
	{
		printf("{ ");
		for (j = 0; j < resourceQuan; ++j)
		{
			printf("%d, ", needMatrix[i][j]);
		}
		printf("}\n");
	}
	return;
}

void printAllocMatrix()
{
	for (i = 0; i < processQuan; ++i)
	{
		printf("{ ");
		for (j = 0; j < resourceQuan; ++j)
		{
			printf("%d, ", allocMatrix[i][j]);
		}
		printf("}\n");
	}
	return;
}

void printAvailable()
{
	for (i = 0; i < resourceQuan; ++i)
	{
		printf("%d, ",availResourceVector[i]);
	}
	printf("\n");
	return;
}
void printMaxMatrix()
{
	for(i=0;i<processQuan;++i)
	{
		printf("{ ");
		for (j = 0; j < resourceQuan; ++j)
		{
			printf("%d, ",maxMatrix[i][j]);
		}
		printf("}\n");
	}
	return;
	
}

void printReqOrRelVector(int vec[])
{
	for (i = 0; i < resourceQuan; ++i)
	{
		printf("%d, ",vec[i]);
	}
	printf("\n");
	return;
}
int inSafeMode()
{
	int ifFinish[processQuan] = {0};//there is no bool type in old C
	int work[resourceQuan];//temporary available resources vector
	for(i = 0; i < resourceQuan; i++)
	{
		work[i] = availResourceVector[i];
	}
	int k;
	for(i = 0; i < processQuan; i++)
	{
		if (ifFinish[i] == 0)
		{
			for(j = 0; j < resourceQuan; j++)
			{
				if(needMatrix[i][j] <= work[j])
				{
					if(j == resourceQuan - 1)//means we checked whole vector, so this process can execute
					{
						ifFinish[i] = 1;
						for (k = 0; k < resourceQuan; ++k)
						{
							work[k] += allocMatrix[i][k];
							//execute and release resources
						}
						i = -1;//at the end of this loop, i++, so -1++ = 0
						break;
					}
					else
					{
						continue;
					}
				}
				else//resources not enough, break to loop i for next process
				{

					break;
				}
			}
		}
		else
		{
			continue;
		}
	}
	//Conditions if we finish loop i
	//1. there is no process can run in this condition.
	//2. all processes are runned, which means it is in safe status.
	for(i = 0; i < processQuan; i++)
	{
		if (ifFinish[i] == 0)
		{
			//not all processes are runned, so it is condition 1.
			return -1;
		}
		else
		{
			continue;
		}
	}
	//finished loop, so it is condition 2
	return 0;
  }
