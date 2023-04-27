#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <time.h>



void generatePayload(char *payload, int size)
{
    for (int i = 0; i < size; i++)
    {
        payload[i] = 'A' + (rand() % 26);
    }
}

int main()
{
    srand(time(NULL));
    while (1)
    {
        char payload[1024] = {0};
        
        generatePayload(payload, 10);
        sleep(0.1);
        printf("Payload: %s\n", payload);
    }

    return 0;
}