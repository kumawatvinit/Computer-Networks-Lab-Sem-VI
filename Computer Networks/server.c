#include <netinet/in.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

#define SERVER_PORT 3000
#define PRINT_PACKETS 1  // Set to 0 or 1
/*
	If set to 0, packets will not be printed evrytime we receive a new packet
	if set to 1, packets will be printed same as in client side
*/

typedef struct {
  int type;
  unsigned int seq_no;
  char payload[1024];
} packet_t;

typedef struct {
  packet_t p;
  unsigned char checksum;
} packet_checksum_t;

int sockfd;
struct sockaddr_in addr;
struct sockaddr_in client_addr;

unsigned int type1_count = 0;
unsigned int type2_count = 0;
packet_t *global_packet = NULL;
pthread_mutex_t global_packet_mutex;

void print_packet(packet_t p) {
  printf("PACKET -- Type: %d\tSeq no: %u\tPayload: %s\n", p.type, p.seq_no,
         p.payload);
}

void *error_handler(void *args) {
  socklen_t client_addr_len = sizeof(client_addr);
  packet_checksum_t ps;

  while (1) {
    pthread_mutex_lock(&global_packet_mutex);
    if (global_packet == NULL) {
      memset(&ps, 0, sizeof(ps));
      int n = recvfrom(sockfd, &ps, sizeof(ps), 0,
                       (struct sockaddr *)&client_addr, &client_addr_len);
      if (n < 0) {
        perror("recvfrom");
        exit(EXIT_FAILURE);
      }

      unsigned char *temp = (unsigned char *)(&ps.p);
      size_t len = sizeof(ps.p);
      unsigned char checksum = 0;
      for (size_t i = 0; i < len; i++) checksum ^= *(temp + i);

      if (checksum != ps.checksum)
        printf("ERROR: Packet checksum not matched\n");
      else {
        global_packet = &(ps.p);
        if (PRINT_PACKETS) print_packet(ps.p);
      }
    }
    pthread_mutex_unlock(&global_packet_mutex);
  }
}

void *type1_handler(void *args) {
  while (1) {
    pthread_mutex_lock(&global_packet_mutex);
    if (global_packet != NULL)
      if (global_packet->type == 1) {
        type1_count += 1;
        global_packet = NULL;
      }
    pthread_mutex_unlock(&global_packet_mutex);
  }
}

void *type2_handler(void *args) {
  while (1) {
    pthread_mutex_lock(&global_packet_mutex);
    if (global_packet != NULL)
      if (global_packet->type == 2) {
        type2_count += 1;
        global_packet = NULL;
      }
    pthread_mutex_unlock(&global_packet_mutex);
  }
}

void *print_handler(void *args) {
  while (1) {
    usleep(300000);
    printf("COUNT: Type 1: %d\tType 2: %d\n", type1_count, type2_count);
  }
}

int main() {
  sockfd = socket(AF_INET, SOCK_DGRAM, 0);
  if (sockfd < 0) {
    perror("socket");
    exit(EXIT_FAILURE);
  }

  memset(&addr, 0, sizeof(addr));
  addr.sin_family = AF_INET;
  addr.sin_addr.s_addr = INADDR_ANY;
  addr.sin_port = htons(SERVER_PORT);

  if (bind(sockfd, (struct sockaddr *)&addr, sizeof(addr)) < 0) {
    perror("bind");
    exit(EXIT_FAILURE);
  }

  printf("Server listening on port %d...\n", SERVER_PORT);

  pthread_t error_thread, type1_thread, type2_thread, print_thread;

  pthread_mutex_init(&global_packet_mutex, NULL);

  pthread_create(&error_thread, NULL, error_handler, NULL);
  pthread_create(&type1_thread, NULL, type1_handler, NULL);
  pthread_create(&type2_thread, NULL, type2_handler, NULL);
  pthread_create(&print_thread, NULL, print_handler, NULL);

  pthread_join(error_thread, NULL);
  pthread_join(type1_thread, NULL);
  pthread_join(type2_thread, NULL);
  pthread_join(print_thread, NULL);

  pthread_mutex_destroy(&global_packet_mutex);

  close(sockfd);
  return 0;
}
