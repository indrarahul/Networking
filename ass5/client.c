#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<unistd.h>
#include<sys/types.h>
#include<sys/socket.h>
#include<netinet/in.h>
#include<pthread.h>
#include<stdlib.h>
#include<netdb.h>
#include<sys/time.h>


struct timeval tv;
uint32_t original;
char* buf;

uint32_t getTimeStamp() {
    gettimeofday(&tv, NULL);
    return ((uint32_t)tv.tv_sec - original) * (uint32_t)1000000 + (uint32_t)tv.tv_usec;
}

void getSecond() {
	gettimeofday(&tv, NULL);
	original = (uint32_t)tv.tv_sec;
}

void toBuffer(char* buffer, int val, int pos, int len) {
	int i;
	for(i = pos + len - 1; i >= pos; i -= 1) {
		buffer[i] = val % 256;
		val /= 256;
	}
}

int main(int argc, char const *argv[])
{
	int PORT = atoi(argv[2]), len, total, i, ok;
	int P = atoi(argv[3]);
	int TTL = atoi(argv[4]), T;
	int NumPackets = atoi(argv[5]);
	total = P + 7;
	
	buf = (char*)malloc(total * sizeof(char));
	int sockfd, sTime, rTime;
	
	struct sockaddr_in serv_addr;
	struct hostent* server;
	socklen_t addr_size;
	sockfd = socket(AF_INET, SOCK_DGRAM, 0);
	if(sockfd < 0) 
		printf("Error opening socket\n");

	server = gethostbyname(argv[1]);
	if(server == NULL)
		printf("Error, no such host\n");

	bzero((char*)&serv_addr, sizeof(serv_addr));
	
	serv_addr.sin_family = AF_INET;
	bcopy((char*)server-> h_addr,(char *)&serv_addr.sin_addr.s_addr , server-> h_length);
	
	serv_addr.sin_port = htons(PORT);
	
	FILE* fp = fopen(argv[6], "w");
	FILE* fp1 = fopen(argv[7], "a");
	addr_size = sizeof(serv_addr);
	getSecond();
	for(i = 1; i <= NumPackets; i += 1) {
		T = TTL;
		ok = 0;
		while(T != 0) {
			toBuffer(buf, i, 0, 2);
			toBuffer(buf, T, 6, 1);
			sTime = getTimeStamp();
			if(ok == 0) {
				ok = 1;
				toBuffer(buf, sTime, 2, 4);
			}
			sendto(sockfd, buf, total, 0, (struct sockaddr*)&serv_addr, addr_size);
			recvfrom(sockfd, buf, total, 0, (struct sockaddr*)&serv_addr, &addr_size);
			while(i != (buf[0] * 256 + buf[1])) 
				recvfrom(sockfd, buf, total, 0, (struct sockaddr*)&serv_addr, &addr_size);
			T = buf[6] - 1;
		}
		rTime = getTimeStamp();
		fprintf(fp1, "%d, %d\n", P, rTime - sTime);
	}
	fclose(fp);
	fclose(fp1);

	return 0;
}