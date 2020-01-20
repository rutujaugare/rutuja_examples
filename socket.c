#include<stdio.h>
#include<stdlib.h>
#include<sys/types.h>
#include<sys/socket.h>
#include<arpa/inet.h>
#include<string.h>
#include<pthread.h>
#include<unistd.h>
#define SIZE 100
char buffer[SIZE] ={0};
int id[5],count=0,i=0;
int function(int  new)//this function sends the recived data to other clients
{
	printf("in function\n");
	int j = 0,ret1=0;
	for(j = 0;j<count;j++)
	{
		if(id[j] != new)
		{
		printf("id[%d]:%d:",j,id[j]);
		write(id[j],buffer,SIZE);
		printf("data send:%s\n",buffer);
		}
	}
	if(strcmp(buffer,"bye")==0)
	{
		printf("exiting\n");
		exit(1);
	}

	memset(buffer,'\0',SIZE);
	j = 0;
	return 0;
}
//here in read function all clients are in thread...
void *read_1(void *client_fd)//this is thread which reads the data from diffrent client
{
	int fd = *(int *)client_fd;
	int ret1;

	while(1)
	{
		printf("fd starts:%d\n",fd);
		printf("Waiting for data FD=%d\n",fd);
        	ret1= read(fd,buffer,SIZE);
		printf("data recived:%s from %d\n",buffer,fd);
		if(ret1 == -1)
		{
			printf("read function failed\n");
		}
		printf("rrrrrrrrrrrrrr\n");
        	function(fd);
		printf("fd ends:%d\n",fd);
	}
}
int main()
{
	int sockfd=0,ret=0,acceptret=0;
	struct sockaddr_in data;
	int opt=0;
	pthread_t id1[5];
	int addrlen = sizeof(data);
	sockfd = socket(AF_INET,SOCK_STREAM,0);
	if(sockfd == -1)
	{
		perror("socket");
		exit(1);
	}
	ret = setsockopt(sockfd,SOL_SOCKET,SO_REUSEADDR|SO_REUSEPORT,&opt,sizeof(opt));
	if(ret == -1)
	{
		perror("setsocket");
		exit(1);
	}
	data.sin_family = AF_INET;
	data.sin_addr.s_addr = INADDR_ANY;
	data.sin_port = htons(4000);
	ret = bind(sockfd,(struct sockaddr*)&data,sizeof(data));
	if(ret == -1)
	{
		perror("bind");
		exit(1);
	}
	ret = listen(sockfd,5);
        if(ret == -1)
        {
                perror("listen");
                exit(1);
        }
	printf("Now server is in network\n");
        printf("Max limit of client is 5\n");
	while(1)
	{
		if( count > 5)
		{
			printf("5 is maximum size to clients\n");
			
			break;
		}
		acceptret = accept(sockfd,(struct sockaddr*)&data,(socklen_t*)&addrlen);
		if(acceptret == -1)
		{
			perror("accept");
			exit(1);
		}
		id[i] = acceptret;
		//if 5 clients are connected then it will create 5 threads
		ret = pthread_create(&id1[i],NULL,&read_1,(void *)&acceptret);//this will creat threads for each client
		if(ret == -1)
		{
			perror("read");
		}
		count++;
		i++;
		printf("count:%d\n",count);
		printf("i :%d\n",i);
		
	}
	

}
