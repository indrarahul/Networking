#include <bits/stdc++.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <fstream>
#include <unistd.h> 

using namespace std;
#define PORTNO "4053"
#define BACKLOG 5
#define SIZE 100
#define SIZE_CLIENT 10
string getFileContents (ifstream&);

int cfd[SIZE_CLIENT]={0};
char ip[INET6_ADDRSTRLEN];
char new_msg[SIZE],buf[SIZE];
int len,numbytes;
int clients=0;
int com;
thread s[SIZE_CLIENT],r[SIZE_CLIENT];
class sock{
public:
	
	struct sockaddr_in addr;
	socklen_t addr_size;
 	struct addrinfo hints, *res;
 	int sockfd, comm,yes=1,check;

	void gainfo()
	{
		memset(&hints, 0, sizeof(hints));
 		hints.ai_family = AF_INET;
 		hints.ai_socktype = SOCK_STREAM;
 		hints.ai_flags = AI_PASSIVE;
 		if(getaddrinfo(NULL, PORTNO, &hints, &res)!=0)
 			{
 				cout << "Error getaddrinfo()\n";
 				exit(1);
 			}

	}

	void create_socket()
	{
		sockfd = socket(AF_INET, SOCK_STREAM, res->ai_protocol);
		if(sockfd<0)
		{
			cout << "Error Creating Socket\n";
			exit(1);
		}
	}


	void sock_bind()
	{
		if (setsockopt(sockfd,SOL_SOCKET,SO_REUSEADDR,&yes,sizeof yes) == -1) {
		 perror("setsockopt\n");
 		exit(1);
		} 
		
		if(bind(sockfd, res->ai_addr, res->ai_addrlen)==-1)
		{
			cout << "Error Binding\n";
			exit(1);
		}

	}

	void sock_listen()
	{
		if(listen(sockfd, BACKLOG)==-1)
			{
				cout << "Error Listening\n";
				exit(1);
			}

	}

	int sock_accept()
	{
		addr_size = sizeof(addr);
		comm = accept(sockfd, (struct sockaddr *)&addr, &addr_size);
		if(comm==-1)
		{
			cout << "Error Accepting\n";
			exit(1);
		}

		cout << "Connection Established!!\n";
		inet_ntop(AF_INET, &(addr.sin_addr), ip, sizeof(ip));
		cout << "IP Address: " << ip <<endl<<endl;
		return comm;

	}

	void sock_close()
	{
		// close(comm);
		close(sockfd);
	}


	int sock_recv(char buf[],int com)
	{
		check = recv(com,buf,SIZE-1,0);
		if(check==-1)
		{
			"Error Receiving\n";
			
		}	
		buf[check]='\0';
		return check;
	}

	void sock_send(char *msg,int com)
	{
		if(send(com,msg,strlen(msg),0)==-1)			// Have to implement lost data logic. 
			cout << "Not Sent ! Error Sending\n";
	}


	
};

// void sender(sock a,int fd)
// {
// 	if(fd==5)
// 		fd++;
// 	else
// 		fd--;
// 	while(1){

// 		if(new_msg=="`")
// 			break;
// 		if(strlen(new_msg)>0)
// 			a.sock_send(new_msg,fd);
// 		memset(new_msg,'\0',strlen(new_msg));
// 	}

// }

void receiver(sock a,int fd)
{

	while(1){
	len = a.sock_recv(buf,fd);
	if(len==0)
			{	
			cout << "Connection Lost\n";
			close(fd);
			cfd[clients--]=0;
			return;
			}
		if(fd==5)
		fd++;
	else
		fd--;
			a.sock_send(buf,fd);
			memset(buf,'\0',strlen(buf));
		}
	}

void accepter(sock a)
{
	
	while(1){
	if((cfd[clients] = a.sock_accept())>0)
	{
		r[clients] = thread(receiver,a,cfd[clients]);
		clients++;
	}	

	}
	int i=0;
	for(i=0;i<SIZE_CLIENT;i++){
	r[i].join();}
}

int main(int argc, char *argv[])
{
	
	ifstream Reader ("k.txt");
	string Art = getFileContents (Reader);
	cout << Art << endl;

	cout << "Server Started at 127.0.0.1:" << PORTNO << endl;
	sock a;
	
	a.gainfo();
	a.create_socket();
	a.sock_bind();
	a.sock_listen();
	
	thread acc(accepter,a);
	acc.join();
	
	a.sock_close();
	return 0;
}




string getFileContents (ifstream& File)
{
    string Lines = "";        
    
    if (File)                      
    {
	while (File.good ())
	{
	    string TempLine;                  
	    getline (File , TempLine);        
	    TempLine += "\n";                      
	    
	    Lines += TempLine;                     
	}
	return Lines;
    }
    else                           
    {
	return "ERROR File does not exist.";
    }
}