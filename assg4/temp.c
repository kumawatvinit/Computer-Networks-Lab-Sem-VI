#include<stdio.h>
#include<stdlib.h>
#define infinity 100000

struct node{
    unsigned distance[100];
    unsigned from[100];
}routingTable[100];



int main()
{
    int n = 100;
    int graph[n][n];

    for(int i=0; i<n; i++)
    for(int j=0; j<n; j++) {
        graph[i][j] = infinity;
    }

    // input
    for(int i=0; i<n; i++)
    for(int j=0; j<n; j++)
    {
        scanf("%d", &graph[i][j]);
        graph[i][i] = 0;

        routingTable[i].distance[j] = graph[i][j];
        routingTable[i].from[j] = j;
    }

    // bellman ford
    int count = 0;
    do
    {
        count = 0;
        
        for(int i=0; i<n; i++)
        for(int j=0; j<n; j++)
        for(int k=0; k<n; k++)
        {
            if(routingTable[i].distance[j] > graph[i][k] + routingTable[k].distance[j]) 
            {
                routingTable[i].distance[j] = routingTable[i].distance[k] + routingTable[k].distance[j];
                routingTable[i].from[j] = k;
                count++;
            }
        }
    } while (count!=0);
    
}