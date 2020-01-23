//this code is for client to client communication with select function..........



#include<stdio.h>
#include<unistd.h>
#include<sys/types.h>
#include<sys/socket.h>
#include<netdb.h>
#include<netinet/in.h>
#include<stdlib.h>
#include<string.h>
#include<pthread.h>
#include <sys/select.h>
#include <sys/time.h>

#define BUFFER_MAX_SIZE 1024
#define MAX_CONNCETION 4
#define PORT 4000

int client_socket[MAX_CONNCETION];
static void refresh_fd_set(fd_set *fd_set_ptr)
{
	FD_ZERO(fd_set_ptr);
	for(int i=0;i<MAX_CONNCETION;i++)
	{
		if(client_socket[i]!=-1)
		{
			FD_SET(client_socket[i],fd_set_ptr);
		}
	}
}
static int get_max()
{
	int max=-1;
	for(int i=0;i<MAX_CONNCETION;i++)
	{
		if(client_socket[i]>max)
		{
			max=client_socket[i];
		}
	}
	return max;
}
int main()
{
	struct sockaddr_in name;
		
	struct hostent* h;
	int master_skt;
	int data_socket,ret;
	char buffer[BUFFER_MAX_SIZE];
	char data[BUFFER_MAX_SIZE];
	int current_conn,opt=1;
	int count=1,status;
	char shut[BUFFER_MAX_SIZE]="exit";
	char hostname[BUFFER_MAX_SIZE];
	fd_set readfds;
	
	 
	
	for(int i=0;i<MAX_CONNCETION;i++)		// initialization 
		client_socket[i]=-1;

	master_skt=socket(AF_INET,SOCK_STREAM,0);	//Creating Master socekt

	if(master_skt==-1)
	{
		perror("socket");
		exit(EXIT_FAILURE);
	}
	
	printf("Master Socket is created\n");
	if( setsockopt(master_skt, SOL_SOCKET, SO_REUSEADDR, (int *)&opt, sizeof(opt)) < 0 )   // Forcefully attaching socket to the port 4000
	{   
		printf("error in setsockopt\n");  
		exit(1);
		
	}
	else
	{
		
	   	printf("setsockopt succesfully\n"); 
	}   
	memset(hostname,'\0',BUFFER_MAX_SIZE);
	
	gethostname(hostname,BUFFER_MAX_SIZE);
	printf("Hostname: %s\n", hostname);
	
	h = gethostbyname(hostname);
	printf("h_name: %s\n", h->h_name);
	
	memset(&name,0,sizeof(struct sockaddr_in));
	name.sin_family=AF_INET;
	name.sin_port=htons(PORT);
	name.sin_addr.s_addr=htonl(INADDR_ANY);

	ret = bind(master_skt,(const struct sockaddr *) &name,sizeof(struct sockaddr_in));	//bind port at 4000
	 
	if(ret==-1)
	{
		perror("bind");
		exit(EXIT_FAILURE);
	}

	printf("bind() call succeed\n");

	ret=listen(master_skt,5);							//listening client request 
	if(ret==-1)
	{
		perror("listen");
		exit(EXIT_FAILURE);
	}

	else
	printf("listening sucessfully\n\n");
	
	client_socket[0]=master_skt;
	
		
	
	
	while(1)
	{
		
		refresh_fd_set(&readfds);

		status=select(get_max()+1,&readfds,NULL,NULL,NULL);	//allows to monitor multiple file descriptors
		if(status==-1)
		{
			 perror("select()");
			 exit(EXIT_FAILURE);
		}
		else
		{	 
			//printf("available now: %d\n",status);
		}

		if(FD_ISSET(master_skt,&readfds))	//If something happened on the master socket , then its an incoming connection
		{
			data_socket = accept(master_skt,NULL,NULL);
			if(data_socket==-1)
			{
				perror("accept");
				exit(EXIT_FAILURE);
			}

			printf("\n\nConnection accepted from client at fd :- %d\n",data_socket);
			
			
			if(count<MAX_CONNCETION)	//checking for valid conn (count<max conn) and storing to client array  
			{
				count++;
				printf("count =%d\n",count);
				int i;
				for(i=0;i<MAX_CONNCETION;i++)		 
				{	
					if(client_socket[i]!=-1)
						continue;
					client_socket[i]=data_socket;
					break;
				}
				printf("New entry at %d is %d\n",i,data_socket);
			}
			else 					
			{
				memset(data,'\0',BUFFER_MAX_SIZE);	
				sprintf(data,"Server is full\n");		//if count>max conn then it will print server busy
				write(data_socket,data,BUFFER_MAX_SIZE);				
			}
			

	}
	else
	{
		
		for(int i=0;i<MAX_CONNCETION;i++)	
		{	
			
			if(FD_ISSET(client_socket[i],&readfds))	 // checking active client conn and  start reading on that port and address
			{
				memset(buffer,'\0',BUFFER_MAX_SIZE);	
				current_conn=client_socket[i];
				ret=read(current_conn,buffer,BUFFER_MAX_SIZE);
				if(ret == -1)
				{
					perror("read");
					
				
				}
				
				printf("ret:- %d\n",ret);		//it returns the number of bytes it read succesfully				
				if(ret==0) 		//comaparing which client shutdown without indicating
				{
					printf("Client %d:- %s\n",i,shut);
					printf("COMMUNICATION EXIT\n");
					close(current_conn);
					printf("FD close:%d\n",current_conn);	//then it will print which fd disconnected
					client_socket[i]=-1;			//storing -1 into  disconnectd client  
					printf("client_socket[%d]:%d\n",i,client_socket[i]);
					count--;
					printf("count =%d\n",count);
					for (int j=1;j<MAX_CONNCETION;j++)//it will broadcast the msg to every client which are active except current client
					{
						if(client_socket[j]!=current_conn && client_socket[j]!=-1)
						{
							//printf("Client %d\n",j);
							sprintf(shut,"\nclient %d:exit\n",i);
		 					write(client_socket[j],shut,BUFFER_MAX_SIZE);
						}
					}
					memset(shut,'\0',BUFFER_MAX_SIZE);
					break;
				}
				
				
				printf("\nclient %d:%s",i,buffer);	
				memset(data,'\0',BUFFER_MAX_SIZE);
				memcpy(&data,buffer,BUFFER_MAX_SIZE);
				
				
				if(strncmp(data,"exit",4)==0) //comaparing which client written exit 
				{
					printf("\nCOMMUNICATION EXIT\n");
					close(current_conn);
					printf("FD close:%d\n",current_conn);	//then it will print which fd disconnected
					client_socket[i]=-1;			//storing -1 into  disconnectd client  
					printf("client_socket[%d]:%d\n",i,client_socket[i]);
					count--;
					printf("count =%d\n",count);
					
				}

				for (int j=1;j<MAX_CONNCETION;j++)//it will broadcast the msg to every client which are active except current client
				{
					if(client_socket[j]!=current_conn && client_socket[j]!=-1)
					{
						//printf("Client %d\n",j);
						sprintf(data,"\nclient %d:",i);
						strcat(data,buffer);
	 					write(client_socket[j],data,BUFFER_MAX_SIZE);
					}
				}
				
				}

			}			
		}
	}
}




/*DATA
#include<stdio.h>
#include<unistd.h>
#include<sys/types.h>
#include<sys/socket.h>
#include<netdb.h>
#include<netinet/in.h>
#include<stdlib.h>
#include<string.h>
#include<pthread.h>
#include <sys/select.h>
#include <sys/time.h>

#define BUFFER_MAX_SIZE 1024
#define MAX_CONNCETION 5
#define PORT 4000

int client_socket[MAX_CONNCETION];
static void refresh_fd_set(fd_set *fd_set_ptr)
{
	FD_ZERO(fd_set_ptr);
	for(int i=0;i<=MAX_CONNCETION;i++)
	{
		if(client_socket[i]!=-1)
		{
			FD_SET(client_socket[i],fd_set_ptr);
		}
	}
}
static int get_max()
{
	int max=-1;
	for(int i=0;i<=MAX_CONNCETION;i++)
	{
		if(client_socket[i]>max)
		{
			max=client_socket[i];
		}
	}
	return max;
}
int main()
{
	struct sockaddr_in name;
	
	int master_skt;
	int data_socket,ret;
	char buffer[BUFFER_MAX_SIZE];
	char data[BUFFER_MAX_SIZE];
	int current_conn,opt=1;
	int count=0;
	fd_set readfds;

	for(int i=0;i<=MAX_CONNCETION;i++)// initialize
		client_socket[i]=-1;

	master_skt=socket(AF_INET,SOCK_STREAM,0);

	if(master_skt==-1)
	{
		perror("socket");
		exit(EXIT_FAILURE);
	}
	
	printf("Master Socket is created\n");
	if( setsockopt(master_skt, SOL_SOCKET, SO_REUSEADDR, (char *)&opt, sizeof(opt)) < 0 )   
	{   
		printf("error in setsockopt\n");  
		exit(1);
		
	}
	else
	{
		
	   	printf("setsockopt succesfully\n"); 
	}   

	memset(&name,0,sizeof(struct sockaddr_in));
	name.sin_family=AF_INET;
	name.sin_port=htons(PORT);
	name.sin_addr.s_addr=htonl(INADDR_ANY);

	ret = bind(master_skt,(const struct sockaddr *) &name,sizeof(struct sockaddr_in));
	 
	if(ret==-1)
	{
		perror("bind");
		exit(EXIT_FAILURE);
	}

	printf("bind() call succeed\n");

	ret=listen(master_skt,20);
	if(ret==-1)
	{
		perror("listen");
		exit(EXIT_FAILURE);
	}

	else
	printf("listening sucessfully\n\n");
	
	client_socket[0]=master_skt;
	
	while(1)
	{
		refresh_fd_set(&readfds);

		select(get_max()+1,&readfds,NULL,NULL,NULL);

		if(FD_ISSET(master_skt,&readfds))
		{
			data_socket = accept(master_skt,NULL,NULL);
			if(data_socket==-1)
			{
				perror("accept");
				exit(EXIT_FAILURE);
			}

			printf("\n\nConnection accepted from client at fd :- %d\n",data_socket);
			
			
			if(count<MAX_CONNCETION)
			{
				count++;
				printf("count =%d\n",count);
				int i;
				for(i=0;i<=MAX_CONNCETION;i++)
				{	
					if(client_socket[i]!=-1)
						continue;
					client_socket[i]=data_socket;
					break;
				}
				printf("New entry at %d is %d\n",i,data_socket);
			}
			else 
			{
				memset(data,'\0',BUFFER_MAX_SIZE);
				sprintf(data,"Server is full\n");
				write(data_socket,data,BUFFER_MAX_SIZE);
				
				

				
			}
			

		}
		else
		{
			
			for(int i=0;i<=MAX_CONNCETION;i++)
			{	
				
				if(FD_ISSET(client_socket[i],&readfds))
				{
					memset(buffer,'\0',BUFFER_MAX_SIZE);
					current_conn=client_socket[i];
					ret=read(current_conn,buffer,BUFFER_MAX_SIZE);
					if(ret == -1)
					{
						perror("read");
						exit(EXIT_FAILURE);	
					}
					printf("\nclient %d:%s",i,buffer);	
					memset(data,'\0',BUFFER_MAX_SIZE);
					memcpy(&data,buffer,BUFFER_MAX_SIZE);
					
					
					if(strncmp(data,"exit",4)==0)
					{
						printf("COMMUNICATION EXIT\n");
						close(current_conn);
						printf("FD close:%d\n",current_conn);
						client_socket[i]=-1;
						printf("client_socket[%d]:%d\n",i,client_socket[i]);
						count--;
						printf("count =%d\n",count);
						
					}

					for (int j=1;j<=MAX_CONNCETION;j++)
					{
						if(client_socket[j]!=current_conn && client_socket[j]!=-1)
						{
							sprintf(data,"\nclient %d:",i);
							strcat(data,buffer);
		 					write(client_socket[j],data,BUFFER_MAX_SIZE);
						}
					}
				
				}

			}			
		}
	}
}

*/
