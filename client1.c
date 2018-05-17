/*Code for Dining Philosopher problem - Remove Deadlock - Release after 90 seconds - Socket Programming*/
/*Smit Modi L20432675 | Bhavdip Navadiya L20436986*/
/*
Algorithm and Implementation Steps

https://www.cs.mtu.edu/~shene/NSF-3/e-Book/MUTEX/TM-example-philos-1.html
http://www.cs.gordon.edu/courses/cs322/projects/p3/
http://www.cs.rpi.edu/~moorthy/Courses/os98/Pgms/socket.html - For Socket Connection */

#include<stdio.h>
#include<sys/types.h>
#include<stdlib.h>
#include<unistd.h>
#include<sys/socket.h>
#include<netinet/in.h>
#include<netinet/tcp.h>
#include<arpa/inet.h>
#include<string.h>
#include<errno.h>

#define PORT 5000
#define UDPSERVERPORT 8090
#define BUFLEN 256

#define PHILO_thinking	'T'
#define PHILO_waiting 	'W'
#define PHILO_eating 	'E'
#define PHILO_finish 	'F'

int udp_sockfd;
struct sockaddr_in servaddr;

char buf[BUFLEN];

/*Creating 2 Philosopher as Client*/
int main(int argc, char **argv)
{	
	int no=2; //no of philosphers
	pid_t PidPhil[no];
	int i=0;

	//Creating UDP Client to connect to monitor	
	udp_client();
	

	//philosopher Child Process Creation
	while(i<3)
	{
		if ((PidPhil[i] = fork()) < 0) 
		{
 			perror("fork");
    			abort();
  		} 
		else if (PidPhil[i] == 0)
		{
    			philosophers(i);
    			exit(0);
		}
		i=i+2;
	}

	//Wait for children to exit

	int status;
	pid_t pid;
	sleep(90);
	int j;
	for(j=no; j>0; j--)
	{		
		pid = wait(&status);
  		printf("PID %ld Exit", (long)pid);
		printf("------Released-----\n");	
	}
	close(udp_sockfd);
}

/*This function is to put philospher in thinking state*/
	
void philosophers(int philosopher_number)
{
	printf("\t\t\t\t[Philosopher No %d starts Thinking***] \n", philosopher_number);

	//send status as thinking to monitor using udp
	char status=PHILO_thinking;
	sprintf(buf, "%d %c", philosopher_number, status);
	sendto(udp_sockfd, buf, BUFLEN, 0, (const struct sockaddr *) &servaddr, sizeof(servaddr));
		
	//int random_time = rand() %5 + 1;
	//sleep(random_time);
	sleep(philosopher_number*3);
	philosopher_client(philosopher_number);
}	

/* Making fork as a server and each time philosopher request for forks and communicate using socket. For communication we used socket with TCP protocol for connection. Create socket then bind to specific port no. and listen socket and read the value from client and In last close the connection*/

/* Acquiring left fork  and right fork and implement logic of hold and wait to prevent deadlock  */

int philosopher_client(int philosopher_number)
{
	//sleep(philosopher_number*3);
	int left_fork_port,right_fork_port;
	char left_fork_ip[INET_ADDRSTRLEN]="", right_fork_ip[INET_ADDRSTRLEN]="";
	
	if(philosopher_number>=0 && philosopher_number<4)
	{
		left_fork_port=PORT+philosopher_number; right_fork_port=PORT+philosopher_number+1;
		if(philosopher_number==0 || philosopher_number==1)
		{
			char temp_left_fork_ip[INET_ADDRSTRLEN]="140.158.130.244"; //ip address of server 1
			strcpy(left_fork_ip,temp_left_fork_ip);
			char temp_right_fork_ip[INET_ADDRSTRLEN]="140.158.130.244"; //ip address of server 1
			strcpy(right_fork_ip,temp_right_fork_ip);
		}
		else if(philosopher_number==2)
		{
			char temp_left_fork_ip[INET_ADDRSTRLEN]="140.158.130.244"; //ip address of server 1
			strcpy(left_fork_ip,temp_left_fork_ip);
			char temp_right_fork_ip[INET_ADDRSTRLEN]="140.158.130.244"; //ip address of server 2
			strcpy(right_fork_ip,temp_right_fork_ip);
		}
		else if(philosopher_number==3)
		{
			char temp_left_fork_ip[INET_ADDRSTRLEN]="140.158.130.244"; //ip address of server 2
			strcpy(left_fork_ip,temp_left_fork_ip);
			char temp_right_fork_ip[INET_ADDRSTRLEN]="140.158.130.244"; //ip address of server 2
			strcpy(right_fork_ip,temp_right_fork_ip);
		}	
	}

	if(philosopher_number==4)
	{
		left_fork_port=PORT+4; right_fork_port=PORT;
		char temp_left_fork_ip[INET_ADDRSTRLEN]="140.158.130.244"; //ip address of server 2
		strcpy(left_fork_ip,temp_left_fork_ip);
		char temp_right_fork_ip[INET_ADDRSTRLEN]="140.158.130.244"; //ip address of server 1
		strcpy(right_fork_ip,temp_right_fork_ip);
	}

	//Send status as waiting to monitor using udp
	char status=PHILO_waiting;
	sprintf(buf, "%d %c", philosopher_number, status);
	sendto(udp_sockfd, buf, BUFLEN, 0, (const struct sockaddr *) &servaddr, sizeof(servaddr));
	printf("Philosopher %d is hungry(waiting)\n", philosopher_number);

	printf("Philosopher %d is aquiring left fork no %d\n",philosopher_number,left_fork_port);
	
	//acquiring left fork
	int  left_fork_acquired = philosopher_connect(left_fork_port, philosopher_number,left_fork_ip,1); // 1 is code for acquiring fork

	//if left fork acquired
	if(left_fork_acquired==1)
	{
		printf("Left fork no %d acquired by philosopher no %d\n",left_fork_port, philosopher_number);
		printf("Philosopher %d is aquiring right fork no %d\n",philosopher_number,right_fork_port);
	
		//acquiring left fork
		int right_fork_acquired = philosopher_connect(right_fork_port, philosopher_number,right_fork_ip,1); // 1 is code for acquiring fork

		//if right fork acquired
		if(right_fork_acquired==1)
		{	
			//Send status as eating to monitor using udp
			status=PHILO_eating;
			sprintf(buf, "%d %c", philosopher_number, status);
			sendto(udp_sockfd, buf, BUFLEN, 0, (const struct sockaddr *) &servaddr, sizeof(servaddr));
			printf("Philosopher number %d is eating",philosopher_number);
			

			//int random_time = rand() %5 + 1;
			//sleep(random_time);
			
			sleep(2);

			//release forks
			printf("Release both forks");		
			philosopher_connect(left_fork_port, philosopher_number,left_fork_ip,2); // 2 is code for releasing fork
			philosopher_connect(right_fork_port, philosopher_number,right_fork_ip,2); // 2 is code for releasing fork
			
			//Send status as finished to monitor using udp
			status=PHILO_finish;
			sprintf(buf, "%d %c", philosopher_number, status);
			sendto(udp_sockfd, buf, BUFLEN, 0, (const struct sockaddr *) &servaddr, sizeof(servaddr));
			
		}
		//if right fork not acquired
		else
		{
			printf("Release left fork no %d acquired by philosopher no %d\n", left_fork_port, philosopher_number);
			
			//release left fork
			philosopher_connect(left_fork_port, philosopher_number,left_fork_ip,2); // 2 is code for releasing fork
			
			//sending philospher again to thinking state
			philosophers(philosopher_number);

		}
	}
	//if left fork not acquired
	else
	{
		//Send status as waiting to monitor using udp
		printf("Philosopher number %d is waiting\n",philosopher_number);
		status=PHILO_waiting;
		sprintf(buf, "%d %c", philosopher_number, status);
		sendto(udp_sockfd, buf, BUFLEN, 0, (const struct sockaddr *) &servaddr, sizeof(servaddr));
		
		//sending philosopher to waiting state
		philosopher_client(philosopher_number);
	}

	//sending philosopher to waiting state
	philosophers(philosopher_number);
  	return 0;
}

// Connecting philosophers to forks using TCP

int philosopher_connect(int fork_port_number,int philosopher_number, char* fork_ip, int command)
{
	printf("In Ph_Connect: fork_port_number:%d philosopher_number:%d fork_ip:%s\n",fork_port_number, philosopher_number, fork_ip);
	int c_socket;
	int count;
  	char buffer[256];

  	struct sockaddr_in server_addr;
  	socklen_t addr_size;
  	server_addr.sin_family = AF_INET;
  	server_addr.sin_port = htons(fork_port_number);
  	server_addr.sin_addr.s_addr = inet_addr(fork_ip);

 	memset(server_addr.sin_zero, '\0', sizeof server_addr.sin_zero); 

  	addr_size = sizeof server_addr;
	
	//Open TCP Socket
  	if((c_socket = socket(AF_INET, SOCK_STREAM, 0)) < 0)
	{
     		perror("Cannot Open Stream Socket\n");
     		exit(1);
	}
	else
	{
		printf("Socket opened successfully\n");
	}
	
	//Connect To The Server

	//while(1)
	{
		if(connect(c_socket, (struct sockaddr *) &server_addr, addr_size) < 0) 
		{
      			printf("refused %d  %d \n",fork_port_number,philosopher_number);  
      			perror("Cannot Connect To The Server\n");
    			sleep(1);
  		}
  		else
		{
			printf("Connected fork port number %d with philosopher number %d \n",fork_port_number,philosopher_number);
  			//break;
		}
	}
	
	//Message Writing To Servertcp client server program
	if (command==1){ // command 1 for acquiring fork
		strcpy(buffer,"get fork");

		//sending message using socket
		int n = write(c_socket, buffer, sizeof(buffer));
		
		//Receiving message using socket
		recv(c_socket,buffer,sizeof(buffer),0);

		//If fork acquired
		if(strcmp(buffer,"acquired")==0)
		{
			count=1;
		}
		else
		{
			count=0;
		}
	}
	else if (command==2) // command 2 for releasing fork
	{
		//sending message to release fork
		strcpy(buffer,"release fork");
  		write(c_socket, buffer, sizeof(buffer));
	}
      	close(c_socket);
	return count;
}

//UDP client initialization program
void udp_client()
{
	
	// Creating socket file descriptor
	if ( (udp_sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0 ) 
	{
	perror("socket creation failed");
	exit(EXIT_FAILURE);
	}
	 
	memset(&servaddr, 0, sizeof(servaddr));
	     
	// Filling server information
	servaddr.sin_family = AF_INET;
	servaddr.sin_port = htons(UDPSERVERPORT); //port address of monitor
	servaddr.sin_addr.s_addr = inet_addr("140.158.130.244"); //IP address of monitor
}

