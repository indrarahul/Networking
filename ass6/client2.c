#include <stdio.h> 
#include <string.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>
#include <unistd.h>

#define RRQ 1 
#define WRQ 2
#define DATA 3
#define ERROR 5
#define PORT 69
#define SIZE 516 
#define PORT 69
#define MAXRTR 5

//Variables
static char* MODE="octet";
struct sockaddr_in *addr;
int sockfd, addrlen,ACK=4;
//Variables

void put(char *srv ,char *fname);
void get(char *srv ,char *fname);

int main(int argc , char **argv)
{
	char *srv = argv[1];
	char fname[200];
	addrlen=sizeof(addr);
	int cmd;

	if ((sockfd=socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) == -1)
    {
        printf("Error Creating Socket");
    }

    addr->sin_family = AF_INET;
    addr->sin_port = htons(PORT);
     
    if (inet_aton(srv , &(addr->sin_addr)) == 0) 
    {
        printf("Error info");
        exit(1);
    }    

    struct timeval timeout;      
    timeout.tv_sec = 10;
    timeout.tv_usec = 0;

    if (setsockopt (sockfd, SOL_SOCKET, SO_RCVTIMEO, (char *)&timeout,
                sizeof(timeout)) < 0)
        printf("SETSOCKOPT ERROR\n");

    if (setsockopt (sockfd, SOL_SOCKET, SO_SNDTIMEO, (char *)&timeout,
                sizeof(timeout)) < 0)
        printf("SETSOCKOPT ERROR\n");

    while(1)
	{
		printf("tftp(@0xeax)> \n");
		printf("Select option\n1-> put\n2-> get\n3-> help\n4-> exit\n\n");
		scanf("%d",&cmd);
		switch(cmd){
			
			case 1:
			printf("Enter File Name\n");
			scanf("%s",fname);
			put(srv, fname);
			break;
			
			case 2:
			printf("Enter File Name\n");
			scanf("%s",fname);
			get(srv, fname);
			break;
			
			case 3:
			printf("HELP SECTION NOT DONE YET\n");
			break;
			
			case 4:
			exit(1);
		}
	}

}


void put(char *srv ,char *fname)
{
	FILE *fp;
	fp = fopen(fname, "r");
	int filelen = strlen(fname);
	
	char msg[SIZE] ,buf[SIZE];
	bzero(msg, SIZE);
	
	msg[0]=0x0;
	msg[1]=WRQ; //WRQ

	strcpy(msg+2, fname);
	strcpy(msg+2+filelen+1 ,MODE);
	
	int request_len=2+strlen(fname)+1+strlen(MODE)+1;

	if (sendto(sockfd, msg, request_len , 0 , (struct sockaddr *) &addr, addrlen)==-1)
    {
            printf("Sending Error");
    }
    printf("WRQ Sent Successfully\n");
  

    int n;
    int packetnum=0;
    struct sockaddr_in *their_addr ;
    int addrlen2= sizeof(their_addr);

    int i;
    while(1)
    {
    	for(i=1;i<=MAXRTR;i++)
    	{
    		bzero(buf,SIZE);
		    if (recvfrom(sockfd, buf, SIZE, 0, (struct sockaddr *) &their_addr, &addrlen2) == -1)
		    {
		        printf("Receiving Error");
		    }
		    ACK = (buf[2]<<8) + buf[3];

		    if((buf[1]==ERROR) || (ACK==(packetnum-1)))
		    {
		    	printf("Error Sending!! Sending again%d\n", packetnum);
		    	if (sendto(sockfd, msg, SIZE , 0 , (struct sockaddr *) &addr, addrlen)==-1)
			    {
			            printf("Sending Error");
			    }

		    }

	    else
		    	break;
    	}
    	if(i>MAXRTR)
    		{
    			printf("Error Sending File!\n" );
    			return ;
    		}
    	

	    printf("ACK RCVD for PACKET NUMBER%d.\n", packetnum);
	    fflush(stdout);

	    packetnum++;
    	bzero(msg, SIZE);
    	msg[1]=DATA;
    	msg[2]=packetnum>>8;
		msg[3]=packetnum%(0xff+1);

    	n = fread(msg+4 , 1 , 512 , fp);
    	printf("Sending packet %d of %d Bytes.\n", packetnum,n);
    	if (sendto(sockfd, msg, n+4 , 0 , (struct sockaddr *) &their_addr, addrlen2)==-1)
	    {
	             printf("Sending Error");
	    }
	    

    	
    	if(n<512)
    		break;
    }
    fclose(fp);
    printf("Transmission Done!\n");


}

void get(char *srv ,char *fname)
{
	FILE *fp;
	fp = fopen(fname, "w");
	int filelen = strlen(fname);

	char msg[SIZE]  , buf[SIZE];
	bzero(msg, SIZE);
	
	msg[0]=0x0;
	msg[1]=RRQ; //RRQ
	
	strcpy(msg+2, fname);
	strcpy(msg+2+filelen+1 , MODE);

	
	if (sendto(sockfd, msg, SIZE , 0 , (struct sockaddr *) &addr, addrlen)==-1)
    {
             printf("Sending Error");
    }
    printf("RRQ Sent Successfully\n");


    int n;
    int packetnum=1;
    struct sockaddr_in their_addr ;
    int addrlen2= sizeof(their_addr);

    while(1)
    {
    	addrlen2= sizeof(their_addr);
    	bzero(buf,SIZE);
    	n = recvfrom(sockfd, buf, SIZE, 0, (struct sockaddr *) &their_addr, &addrlen2);
    	
    	if (n == -1)
	    {
	        printf("Receiving Error");
	    }
	    n -= 4;
	    if(buf[1]==ERROR)
	    	printf("Server Sending Error");
	     
	    fwrite(&buf[4],1,n,fp);
	    printf("Received Packet of size = %d\n", n);

	    bzero(msg, SIZE);
		msg[0]=0x0;
		msg[1]=ACK;
		msg[2]=packetnum>>8;
		msg[3]=packetnum%(0xff+1);

		if (sendto(sockfd, msg, 4 , 0 , (struct sockaddr *) &their_addr, addrlen2)==-1)
	    {
	            printf("Sending Error");
	    }    	
    	printf("Sent ACK for PACKET NUM : %d.\n", packetnum);
		
		packetnum++;
	    if(n<512)
	    {
	    	break;
	    }

    }

    fclose(fp);
    printf("Transmission Done!\n");

}