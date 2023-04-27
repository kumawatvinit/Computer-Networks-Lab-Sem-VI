#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <time.h>
#include <unistd.h>

#define SERVER_IP "127.0.0.1"
#define SERVER_PORT 3000

typedef struct {
  int type;
  unsigned int seq_no;
  char payload[1024];
} packet_t;

typedef struct {
  packet_t p;
  unsigned char checksum;
} packet_checksum_t;

unsigned int type1_seq = 1;
unsigned int type2_seq = 1;

void print_packet(packet_t p) {
  printf("PACKET -- Type: %d\tSeq no: %u\tPayload: %s\n", p.type, p.seq_no,
         p.payload);
}

void send_packet(int sock, packet_t *p, struct sockaddr_in server_addr) {
  print_packet(*p);

  unsigned char *temp = (unsigned char *)p;
  size_t len = sizeof(*p);
  unsigned char checksum = 0;
  for (size_t i = 0; i < len; i++) checksum ^= *(temp + i);

  packet_checksum_t ps;
  ps.p = *p;
  ps.checksum = checksum;

  int bytes_sent = sendto(sock, (unsigned char *)(&ps), sizeof(ps), 0,
                          (struct sockaddr *)&server_addr, sizeof(server_addr));
  if (bytes_sent < 0) {
    perror("Error sending data");
    exit(EXIT_FAILURE);
  }
}

void generate_rand_string(char *str, int len) {
  for (int i = 0; i < len; i++) str[i] = 'A' + rand() % 26;
  str[len] = 0;
}

void generate_packet(packet_t *p, int type) {
  p->type = type;
  if (type == 1) {
    p->seq_no = type1_seq;
    type1_seq += 1;
  } else if (type == 2) {
    p->seq_no = type2_seq;
    type2_seq += 1;
  }
  generate_rand_string(p->payload, 20);
}

int main(int argc, char *argv[]) {
  int sock;
  struct sockaddr_in server_addr;

  srand(time(NULL));

  // create socket
  sock = socket(AF_INET, SOCK_DGRAM, 0);
  if (sock < 0) {
    perror("Error creating socket");
    exit(EXIT_FAILURE);
  }

  // set server address
  memset(&server_addr, 0, sizeof(server_addr));
  server_addr.sin_family = AF_INET;
  server_addr.sin_addr.s_addr = inet_addr(SERVER_IP);
  server_addr.sin_port = htons(SERVER_PORT);

  packet_t p1, p2;

  clock_t last_millis = 0;
  while (1) {
    clock_t millis = clock() / (CLOCKS_PER_SEC / 1000);

    if (millis != last_millis) {
      if (millis % 100 == 0) {
        generate_packet(&p1, 1);
        send_packet(sock, &p1, server_addr);
      }
      if (millis % 150 == 0) {
        generate_packet(&p2, 2);
        send_packet(sock, &p2, server_addr);
      }
      last_millis = millis;
    }
  }

  close(sock);

  return 0;
}
