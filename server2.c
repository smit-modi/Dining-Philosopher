/*Dining Philospher server2 program

Implemented by Smit Modi L2430675 | Bhavdip Navadiya L204369876
 
*/

#include<stdio.h>
#include<sys/types.h>
#include<stdlib.h>
#include<unistd.h>
#include<sys/socket.h>
#include<netinet/in.h>
#include<netinet/tcp.h>
#include<arpa/inet.h>

#define PORT 5000 //port to open TCP connection

//to keep track if the flag is being used or not
int being_used[5]={0,0,0,0,0};

/*Creating 2 forks */
int main(int argc, char **argv , int no)
{
	no=2; //no of forks
	pid_t pid_server[no];
	int i=3;

	//Chopstick Child Process Creation

	while(i<5)
	{
		if ((pid_server[i] = fork()) < 0)
		{
			perror("fork");
    			abort();
  		}
		else if (pid_server[i] == 0)
		{
    			fork_server(i);
    			exit(0);	
		}
		i++;
	}
	sleep(3);

	
	//Wait to children exit

	int status;
	int j;
	pid_t pid;
	for(j=no; j>0; j--)
	{
		pid = wait(&status);
  		printf("PID %ld Exit", (long)pid);
		printf("------Released-----\n");	
	}
}

/*Call to start fork server*/
	
void fork_server(int fork_no)
{
	printf("\t\t<!___fork %d is ready___!>\n", fork_no);
	fork_server_start(fork_no);
	sleep(3);
}

//Creating TCP socket to start fork server to accept the connectons from remote philosopers

int fork_server_start(int fork_number)
{
	int sockfd, newsockfd, cli_len, port, len;
	struct sockaddr_in cli_addr, serv_addr;
	char received_string[256];
	port = fork_number+PORT; //setting port values for fork

	/* open a TCP socket */
	if((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) 
	{
		perror("Cannot Open Stream Socket.");
		exit(1);
	}
  	
	/* bind the local address, so that the client philosphers can connect to this server */
	bzero((char *) &serv_addr, sizeof(serv_addr));
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
	serv_addr.sin_port = htons(port);	
  
	if(bind(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0) 
	{
     		perror("Cannot Bind Local Address.");
     		exit(1);
  	}
  	else
  	{ 
		printf("Socket connected to %d fork %d \n",serv_addr,fork_number);
		sleep(2);
  	}
	
  	/* listen to the socket */
  	listen(sockfd, 1);
	int i;
  	for(i=0;i<2;i++) 
	{
 	    	/* wait for a connection from a client; this is an iterative server */
     		cli_len = sizeof(cli_addr);
     		newsockfd = accept(sockfd, (struct sockaddr *) &cli_addr, &cli_len);
		
     		if(newsockfd < 0) 
		{
        		perror("Cannot Bind Local Address");
     		}
   		else
		{	//read message from client					
			len = read(newsockfd, received_string, 256);
		
			//if message is to acquire fork 
			if(strcmp(received_string,"get fork")==0)
			{
				printf("before acquired fork number %d being used %d \n",fork_number,being_used[fork_number]);
			
				//if fork is not being used
				if(being_used[fork_number]==0)
				{	
					//set fork to being used
					being_used[fork_number]=1;
			                printf("after acquired fork number %d being used %d \n",fork_number,being_used[fork_number]);

					//send message as fork acquired
					send(newsockfd,"acquired",sizeof("acquired"),0);
				}
				//if fork is being used
				else
				{
					printf("not acquired fork number %d being used %d \n",fork_number,being_used[fork_number]);
					
					//send message fork not acquired
					send(newsockfd,"not acquired",sizeof("not acquired"),0);
				}
		
			}

			//if message is to release fork
			else if(strcmp(received_string,"release fork")==0)
			{
				 printf("release fork number %d being used %d \n",fork_number,being_used[fork_number]);
				 //set fork as not being used
				 being_used[fork_number]=0;
			}
		}     		
     		
     		close(newsockfd);
  	}  
}
	
