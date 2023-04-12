#include<stdio.h>
#include<stdlib.h>
#define infinity 10000

struct node
{
    unsigned distance[100];
    unsigned from[100];
}routingTable[100];

void reset(int **graph, int n)
{
	for(int i=0; i<n; i++)
	for(int j=0; j<n; j++) 
	graph[i][j] = infinity;
}

int main()
{
    int **graph;
	int n = 100;
	graph = (int **)malloc(n*sizeof(int *));
	for(int i=0; i<n; i++) graph[i] = (int *)malloc(n*sizeof(int));
	reset(graph, n);

    int num,i,j,random,count=0;
	

    printf("\nEnter the number of routers : ");
    scanf("%d",&num);

    printf("\nEnter the cost matrix :\n");
    for(i=0;i<num;i++)
    {
        for(j=0;j<num;j++)
        {
            scanf("%d",&graph[i][j]);
            graph[i][i]=0;
            routingTable[i].distance[j]=graph[i][j];
            routingTable[i].from[j]=j;
        }
    }

    do
    {
        count=0;
        for(i=0;i<num;i++)
        for(j=0;j<num;j++)
        for(random=0;random<num;random++) {
			if(routingTable[i].distance[j]>graph[i][random]+routingTable[random].distance[j]) {
                routingTable[i].distance[j]=routingTable[i].distance[random]+routingTable[random].distance[j];
                routingTable[i].from[j]=random;
                count++;
            }
		}
    }while(count!=0);

	// Printing final routing tables
    for(i=0;i<num;i++)
    {
        printf("\n\n For router %d\n",i+1);
        for(j=0;j<num;j++)
        {
            printf("\t\nnode %d via %d distanceance %d ",j+1,routingTable[i].from[j]+1,routingTable[i].distance[j]);
        }
    }

    printf("\n\n");
    // getch();
}
/*
Input: 
Enter the number of routers :
3
Enter the cost matrix :
0 2 7
2 0 1
7 1 0

Output:
For router 1
node 1 via 1 distanceance 0
node 2 via 2 distanceance 2
node 3 via 3 distanceance 3
For router 2
node 1 via 1 distanceance 2
node 2 via 2 distanceance 0
node 3 via 3 distanceance 1
For router 3
node 1 via 1 distanceance 3
node 2 via 2 distanceance 1
node 3 via 3 distanceance 0
*/
