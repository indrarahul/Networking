#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<unistd.h>
#include<sys/types.h>
#include<sys/socket.h>
#include<netinet/in.h>
#include<pthread.h>
#include<stdlib.h>
#include<arpa/inet.h>
#define SIZE 1301
char *buf;

int main(int argc, char const *argv[])
{
	int PORT = atoi(argv[1]), check,sockfd;

	struct sockaddr_in server_addr, addr; 
	sockfd = socket(AF_INET, SOCK_DGRAM, 0);
	socklen_t addr_size;

	if(sockfd < 0) 
		printf("Error opening socket\n");
	
	bzero((char*)&server_addr, sizeof(server_addr));
	bzero((char*)&addr, sizeof(addr));

	server_addr.sin_family = AF_INET;
	server_addr.sin_addr.s_addr = INADDR_ANY;
	server_addr.sin_port = htons(PORT);
	addr_size = sizeof(server_addr);
	if(bind(sockfd, (struct sockaddr*)&server_addr, addr_size) < 0) {
		printf("Binding Failed\n");
	}

	while(1) {
		buf = (char*)malloc(SIZE * sizeof(char));
		check = recvfrom(sockfd, buf, SIZE, 0, (struct sockaddr*)&addr, &addr_size);
		buf = realloc(buf, check* sizeof(char));
		buf[6] -= 1;
		sendto(sockfd, buf, check, 0, (struct sockaddr*)&addr, addr_size);
	}

	return 0;
}