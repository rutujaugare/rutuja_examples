#include<stdio.h>
#include<stdlib.h>
#include<netdb.h>
#include<unistd.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include<sys/socket.h>
#include<string.h>
#include<pthread.h>
#include<signal.h>
#define MAX 1024
#define SA struct sockaddr

void READ(int i);
void WRITE();

int sockfd,fd;
int online;



void handle_sigint(int sig) 
{ 
    char d[MAX]="exit";
    printf("exiting");
    write(sockfd,d,MAX);
    exit(1);
} 


  

int main(int argc, char **argv)
{
	
	struct sockaddr_in server,clie;
	sockfd=socket(AF_INET,SOCK_STREAM,0);
	if(sockfd==-1)
	{
		printf("socket is not created\n");
		exit(1);
	}
	else
	printf("socket is create\n");
	server.sin_family=AF_INET;
	server.sin_port=htons(atoi(argv[1]));
	server.sin_addr.s_addr=inet_addr(argv[2]);

	fd=connect(sockfd,(SA*)&server,sizeof(server));
	if(fd!=0)
	{
		printf("not connected\n");
		exit(1);
	}
	else
		printf("Succesfully connected\n\n");

	
	pthread_t tid1,tid2;
	int  ret1, ret2;
	online=1;
	int i=0;
	
	
	ret1=pthread_create(&tid1,NULL,&WRITE,NULL);
	
	ret2=pthread_create(&tid2,NULL,&READ,i);
	
	
        if(ret1!=0 && ret2!=0)
        {
                printf("Cant create thread\n");
        }

	ret1=pthread_join(tid1,NULL);
	ret2=pthread_join(tid2,NULL);
  	if(ret1!=0 && ret2!=0)
        {
                printf("failed to join thread\n");
        }
	signal(SIGINT, handle_sigint); 
	close(sockfd);
	return 0;
}
		
		
void WRITE()
{
	int r;
	char buff1[MAX];
	signal(SIGINT, handle_sigint);
	
	while(online)
	{
		memset(buff1,'\0',1024);
		printf("Me:- \t");
		fgets(buff1,MAX,stdin);
		r=write(sockfd,buff1,1024);
		if (r==-1)
		{
			perror("write");
			exit(EXIT_FAILURE);
		}

		//printf("Me:%s\n",buff1);
		
		if (strncmp(buff1,"exit",4)==0)
		{ 
			printf("\nDisconnected client \n");
			close(sockfd);
			memset(buff1,'\0',1024);
			online=0;
			exit(1);
		}
		
	}
	memset(buff1,'\0',1024);
}


void READ(int i)
{
	
	
	char buff[MAX];
	int n;

        signal(SIGINT, handle_sigint); 
	
	while(online)
	{
		n=read(sockfd,buff,1024);
		
		if(n>0)
		{
			printf("\n%s\nMe:\t",buff);
			fflush(stdout);
		}
		else
		{
			 perror("read");
			 exit(EXIT_FAILURE);
		}
		
		if (strncmp(buff,"Server is full",14)==0)
		{ 
			printf("\nDisconnected client \n");
			close(sockfd);
			memset(buff,'\0',1024);
			online=0;
			exit(1);
		}
	 	
	}
}
