#include<bits/stdc++.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <fstream>
#include<unistd.h> 

using namespace std;
#define PORTNO "4054"
#define BACKLOG 5
#define SIZE 100
string getFileContents (ifstream&);

char ip[INET6_ADDRSTRLEN];
char msg[SIZE],buf[SIZE];
int len,numbytes;

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

	void sock_accept()
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

	}

	void sock_close()
	{
		close(comm);
		close(sockfd);
	}
	void sock_close2()
	{
		close(comm);

	}

	int sock_recv()
	{
		check = recv(comm,buf,SIZE-1,0);
		if(check==-1)
		{
			"Error Receiving\n";
			
		}	
		buf[check]='\0';
		return check;
	}

	void sock_send()
	{
		if(send(comm,msg,strlen(msg),0)==-1)			// Have to implement lost data logic. 
			cout << "Not Sent ! Error Sending\n";
	}


	
};

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
	a.sock_accept();
	
	while(1)
	{
		len = a.sock_recv();
		if(len==0)
		{
			cout << "Connection Lost!!\n";
			exit(0);
		}
		cout << ip << " says: ";
		cout << buf << endl;
		memcpy(msg,buf,strlen(buf));
		a.sock_send();
	}	
		

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