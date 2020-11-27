#include <sys/socket.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <pthread.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <sys/wait.h>
#include "functions.h"

#define MAX_CLIENTS 10
#define BUFFER_SIZE 2048

typedef struct {
  int socket;
  int ID;
  int has_introduced; // meaning the 0th packet is received (at first 0, when receives set it to 1)
  char username[256];
  char color[7];
} client_struct;


int client_count = 0;
int ID = 0;
client_struct* clients[MAX_CLIENTS]; // creates array of clients with size MAX_CLIENTS
volatile int leave_flag = 0;

int gameloop();
void* start_network(void* arg);
void* process_client(void* arg);

void send_packet(char *packet); // send packet to all clients
void broadcast_packet(char *packet, int id); // do not send to specified ID
void remove_client(int id);
client_struct* add_client(int client_socket);

void set_leave_flag() {
    leave_flag = 1;
}

/**
======================
Main - OK
======================
**/
int main(int argc, char **argv){

  // catch SIGINT (Interruption, e.g., ctrl+c)
	signal(SIGINT, set_leave_flag);

  // server setup
  int port;
  if (server_parse_args(argc, argv, &port) < 0) return -1;

  // networking_thread
  pthread_t networking_thread;
  pthread_create(&networking_thread, NULL, &start_network, &port);

  // start network
  if (gameloop() < 0) printf("[ERROR] error in gameloop().\n");

	return 0;
}



/* This function should be used when creating new threads for new clients... */
void* process_client(void* arg){
	// char buffer[BUFFER_SIZE];
	// char username[270];
  char received_byte[1];
  int client_leave_flag = 0;

	client_struct* client = (client_struct *) arg;

  int receive;
	while(1){
    if (client_leave_flag) break;


    receive = recv(client->socket, received_byte, 1, 0);
		if (receive > 0){ // received byte
      printf("Received %c (%d) from socket %d\n", printable_char(received_byte[0]), received_byte[0], client->socket);
      fflush(stdout); // to "refresh" the stdout (because no \n char and so it is not printed but kept in buffer)



			// if(strlen(buffer) > 0){
        // if (strcmp(buffer, "quit") == 0) { // quit
        //   sprintf(buffer, "%s left\n", client->username);
        //   printf("%s", buffer);
        //   broadcast_packet(buffer, client->ID);
        //   client_leave_flag = 1;
        // } else { // normal packet
				//   broadcast_packet(buffer, client->ID);
				//   printf("%s [from %s]\n", buffer, client->username);
        // }
			// }
		} else if (receive < 0){ // disconnection or error
      printf("[WARNING] From %s could not receive package.", client->username);
		} else { // receive == 0
      // sprintf(buffer, "%s left\n", client->username);
      // printf("%s", buffer);
      // broadcast_packet(buffer, client->ID);
      printf("Recv failed. Client leave flag set.\n");
      client_leave_flag = 1;
    }

	}

	/* stop client, thread, connection etc*/
	close(client->socket);
  remove_client(client->ID);
  free(client);
  pthread_detach(pthread_self());

	return NULL;
}


/**
======================
Start network - OK
======================
**/
void* start_network(void* arg) {
  int* port = (int *) arg;
  printf("[OK] Entered the start network with port %d.\n", *port);

  // server_network_setup
  int main_socket, client_socket;
  struct sockaddr_in server_address, client_address;
  unsigned int client_address_size = sizeof(client_address);
  if (server_network_setup(&main_socket, &server_address, *port) < 0) exit(-1);

  // infinite loop...
  pthread_t new_client_threads;
  while (1) {
    /* waiting for clients */
    client_socket = accept(main_socket, (struct sockaddr*) &client_address, &client_address_size);
    if (client_socket < 0) {
        /* if client connection fails, we can still accept other connections*/
        printf("[WARNING] Error accepting client.\n");
        continue;
    }

  	/* Check if max clients is reached */
  	if(client_count + 1 == MAX_CLIENTS){
  		printf("[WARNING] Max clients reached. Connection rejected.\n");
  		close(client_socket);
  		continue;
  	}

  	/* Add client and make new thread */
    client_struct* client = add_client(client_socket);
  	if (client == NULL) {
      close(client_socket); // if failed adding
      continue;
    };

  	pthread_create(&new_client_threads, NULL, &process_client, (void *) client);
  }

  printf("[Hmm...] Finished the start network. Should it?\n");

  // return 0;

}

/**
======================
Gameloop - TODO
======================
**/
int gameloop() {
  printf("[OK] Entered the gameloop.\n");
  while (1) {
    if (leave_flag) {
      printf("Leave flag detected in gameloop.\n");
      send_packet("quitfs\0");
      break;
    }
    sleep(1);
  }

  // wait for packets to be sent etc
  sleep(1.5);

  printf("[OK] Finished the gameloop.\n");

  return 0;
}

/**
======================
Add clients - OK
Remove clients - OK
Send packet - OK
Broadcast packet - OK
======================
**/

pthread_mutex_t lock1 = PTHREAD_MUTEX_INITIALIZER;

/* Add clients */
client_struct* add_client(int clientclient_struct client = add_client(client_socket)_socket) {
  // malloc client
  client_struct* client = (client_struct *) malloc(sizeof(client_struct));
  if (client == NULL) {
    printf("[WARNING] Malloc did not work. Denying client.\n");
    return NULL;
  }

	pthread_mutex_lock(&lock1);

	for(int i=0; i < MAX_CLIENTS; ++i) {
		if(!clients[i]) {
      client->socket = client_socket;
      client->ID = ID++;
      client->has_introduced = 0;
		}
	}

	client_count++;

	pthread_mutex_unlock(&lock1);

  return client;
}

/* Remove clients */
void remove_client(int id) {
	pthread_mutex_lock(&lock1);

	for(int i=0; i < MAX_CLIENTS; ++i) {
		if(clients[i]) {
			if(clients[i]->ID == id) {
        free(clients[i]);
				clients[i] = NULL;
				break;
			}
		}
	}

  client_count--;

	pthread_mutex_unlock(&lock1);
}

/* Send a packet to all clients */
void send_packet(char *packet) {
	pthread_mutex_lock(&lock1);

	for(int i=0; i < MAX_CLIENTS; ++i) {
    if(clients[i]) {
      if(write(clients[i]->socket, packet, strlen(packet)) < 0) {
        printf("[ERROR] Could not send packet to someone.\n");
        break;
      }
    }
	}

	pthread_mutex_unlock(&lock1);
}


void broadcast_packet(char *packet, int id) {
	pthread_mutex_lock(&lock1);

	for(int i=0; i < MAX_CLIENTS; ++i) {
		if(clients[i]) {
			if (clients[i]->ID != id) {
				if(write(clients[i]->socket, packet, strlen(packet)) < 0) {
					printf("[ERROR] Could not send packet to someone.\n");
					break;
				}
			}
		}
	}

	pthread_mutex_unlock(&lock1);
}
