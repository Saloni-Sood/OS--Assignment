#include<stdio.h>	
#include<pthread.h>
#include<stdlib.h>
int i,j;
int cash = 4500;
int numberofprocess;                             
int no_of_resources;
int isActive[200]={0};                      
int totalResource[200];
int availResource[200];
int allocResource[200][200];
int maxResource[200][200];
int needResource[200][200];
pthread_mutex_t mutex;
void printAvailResource();
void printAllocResource();
void printNeedResource();
int requestResource(int a,int b[]);
int isEnoughToAlloc(int a[]);
int isGreaterThanNeedResource(int a,int b[]);
int isNeedIsZero(int a);
int isSafeState();
void* Processes_fun(void *);
int main()
{
    printf("Initially cash in the Account : %d\n",cash);
    printf("Enter the number of Process/Thread in the System : ");
    scanf("%d",&numberofprocess);
    printf("Enter the Types of Resources present in the System : ");
    scanf("%d",&no_of_resources);    
    printf("\n----------Enter the number of instance of each resources present in the system-----------\n");
    for(i=0;i<no_of_resources;i++)
    {
        printf("Resource %d : ",i);
        scanf("%d",&totalResource[i]);
        availResource[i] = totalResource[i];
    }
    int check[no_of_resources];
    s3:
        printf("\n----------Enter the number of instance of Resources initially allocated to each Process-----------\n");
        for(i=0;i<no_of_resources;i++)
            check[i]=0;   
        for(i=0;i<numberofprocess;i++)
        {
            printf("For Process %d : ",i);
            for(j=0;j<no_of_resources;j++)
                {
                    scanf("%d",&allocResource[i][j]);
                    check[j] += allocResource[i][j];
                }
        }
        if(isEnoughToAlloc(check)==0)
            {
                printf("Sorry! Resource Allocated is bigger than available in system. Re-Enter\n");
                goto s3;
            }
        else
        {
            for(i = 0;i<no_of_resources;i++)
                availResource[i] = totalResource[i] - check[i];
        }
    printf("\n---------Enter number of Maximum instance that can be allocte to a Process-----------------\n");
    for(i=0;i<numberofprocess;i++)
    {
        printf("For Process %d : ",i);
        for(j=0;j<no_of_resources;j++)
        {
            scanf("%d",&maxResource[i][j]);
        }
    }
    for(i=0;i<numberofprocess;i++)
        for(j=0;j<no_of_resources;j++)
            needResource[i][j] = maxResource[i][j] - allocResource[i][j];
    printf("Resources Currently Available :- \n");
    printAvailResource();
    printf("Initial Resources allocated :- \n");
    printAllocResource();
    printf("Initial Resources Needed :- \n");
    printNeedResource();
    pthread_mutex_init(&mutex,NULL);
    pthread_t *threadid = malloc(sizeof(pthread_t)*numberofprocess);
    int *proid = malloc(sizeof(int)*numberofprocess);
    for(i=0;i<numberofprocess;i++)
    {
        *(proid + i) = i;
        isActive[i]=1;
        pthread_create((threadid+i),NULL,Processes_fun,(void *)(proid+i));
    }
    for(i=0;i<numberofprocess;i++)
    {
        pthread_join(*(threadid+i),NULL);
    }
    printf("Final cash in the Acount : %d\n",cash);
    return 0;
}
void* Processes_fun(void *x)
{
    int processID = *(int*)x;
    while(isActive[processID]==1)
    {
        sleep(1);
        int reqResource[no_of_resources];
        pthread_mutex_lock(&mutex); 
        printf("Process %d is trying to Request some instance of each Resources : ",processID);
        for(i=0;i<no_of_resources;i++)
        {
            if(needResource[processID][i] != 0)
            {
                reqResource[i] = rand() % ( needResource[processID][i] + 1 );
            }
            else
            {
                reqResource[i] = 0;
            }
        }
        for(i=0;i<no_of_resources;i++)
            printf("%d ",reqResource[i]);
        printf("\n");
        requestResource(processID,reqResource);
	    sleep(1);
        if(isNeedIsZero(processID)==1)
        {
            int amt;
            amt = cash;
            amt = amt + 100;
            cash = amt;
            printf("Process %d is changing cash of the Account\n",processID);
            printf("\nProcess %d is releasing its all alocated resource and finnally terminated \n",processID);
            isActive[processID]=0;
            for(i=0;i<no_of_resources;i++)
            {
                availResource[i] += allocResource[processID][i];
                allocResource[processID][i]=0;
            }
        }
        pthread_mutex_unlock(&mutex);
    }
}
void printAvailResource()
{
    for(i=0;i<no_of_resources;i++)
    {
        printf("%d\t",availResource[i]);
    }
    printf("\n");
}
void printAllocResource()
{
    for(i=0;i<numberofprocess;i++)
    {
        printf("Process %d :\t",i);
        for(j=0;j<no_of_resources;j++)
            printf("%d\t",allocResource[i][j]);
        printf("\n");
    }
}
void printNeedResource()
{
    for(i=0;i<numberofprocess;i++)
    {
        printf("Process %d :\t",i);
        for(j=0;j<no_of_resources;j++)
            printf("%d\t",needResource[i][j]);
        printf("\n");
    }
}
int requestResource(int processID,int reqResource[])
{
    if(isGreaterThanNeedResource(processID,reqResource)==1)
    {
        printf("Requested Resource is greater than needed.\n");
        printf("Can't be Allocated\n");
        return 0;
    }
    if(isEnoughToAlloc(reqResource)==0)
    {
        printf("Requested Resource is greater than the Available Resource\n");
        printf("Can't be Allocated\n");
        return 0;
    }
    for (i = 0; i < no_of_resources; i++)
	{
		needResource[processID][i] -= reqResource[i];
		allocResource[processID][i] += reqResource[i];
		availResource[i] -= reqResource[i];
	}
	printf("Checking if it is safe to allocate the Resource...\n");
    if(isSafeState()==1)
    {
        printf("System is in Safe State. Resources Successfully Allocated to the Process %d!...\n",processID);
        printf("Resources Currently Available :- \n");
        printAvailResource();
        printf("Resources Allocated\n");
        printAllocResource();
        printf("Resources Needed\n");
        printNeedResource();
        return 1;
    }
    else
    {
        printf("Allocating Resource does not leave the System in Safe State\n");
        for (i = 0; i < no_of_resources; ++i)
		{
		    needResource[processID][i] += reqResource[i];
		    allocResource[processID][i] -= reqResource[i];
		    availResource[i] += reqResource[i];
		}
        return 0;
    }
}
int isGreaterThanNeedResource(int processID,int reqResource[])
{
	for (i = 0; i < no_of_resources; i++)
	{
		if (reqResource[i] <= needResource[processID][i])
		{
			continue;
		}
		else
		{
			return 1;
		}
	}
	return 0;
}
int isEnoughToAlloc(int reqResource[])
{
	for (i = 0; i < no_of_resources; ++i)
	{
		if (reqResource[i] <= availResource[i])
		{
			continue;
		}
		else
		{
			return 0;
		}
	}
	return 1;
}
int isNeedIsZero(int processID)
{
    for(i=0;i<no_of_resources;i++)
    {
        if(needResource[processID][i]==0)
        {
            continue;
        }
        else
        {
            return 0;
        }
    }
    return 1;
}
int isSafeState()
{
    int isEnd[numberofprocess];
    for(i=0;i<numberofprocess;i++)
        {
            if(isActive[i]==1)
                isEnd[i]=0;
            else    
                isEnd[i]=1;
        }
    int avail[no_of_resources];
    for(i=0;i<no_of_resources;i++)
    {
        avail[i]=availResource[i];
    }
    int k;
    for(i=0;i<numberofprocess;i++)
    {
        if(isEnd[i]==0)
        {
            for(j=0;j<no_of_resources;j++)
            {
                if(needResource[i][j] <= avail[j])
                {
                    if(j==no_of_resources-1)
                    {
                        isEnd[i]=1;
                        for(k=0;k<no_of_resources;k++)
                        {
                            avail[k]+=allocResource[i][k];
                        }
                        i = -1;
                        break;
                    }
                    else   
                    {
                        continue;
                    }
                }
                else
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
    for(i=0;i<numberofprocess;i++)
    {
        if(isEnd[i]==1)
            continue;
        else
            return 0;
    }
    return 1;
}
