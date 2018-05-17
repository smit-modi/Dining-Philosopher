/*Dining Philospher Monitor program

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
#include<string.h>
#include<errno.h>
#include <time.h>

#define BUFLEN 256
#define UDPSERVERPORT 8090

#define PHILO_thinking	'T'
#define PHILO_waiting 	'W'
#define PHILO_eating 	'E'
#define PHILO_finish 	'F'

char buf[BUFLEN];

int main(int argc, char **argv)
{

    time_t curTime;
    struct tm *timeInfo;
    int philospher_number;
    int i;
    char status;
    char *monthName[] = {"Jan", "Feb", "Mar", "Apr", "May", "Jun", "Jul", "Aug", "Sep", "Oct", "Nov", "Dec"};
    int numfinish = 0;

    int sockfd;
    struct sockaddr_in servaddr, cliaddr;
     
    // Creating socket file descriptor using UDP
    if ( (sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0 ) {
        perror("socket creation failed");
        exit(EXIT_FAILURE);
    }
     
    memset(&servaddr, 0, sizeof(servaddr));
    memset(&cliaddr, 0, sizeof(cliaddr));
     
    // Filling server information
    servaddr.sin_family    = AF_INET; // IPv4
    servaddr.sin_addr.s_addr = INADDR_ANY;
    servaddr.sin_port = htons(UDPSERVERPORT);
     
    // Bind the socket with the server address
    if ( bind(sockfd, (const struct sockaddr *)&servaddr, 
            sizeof(servaddr)) < 0 )
    {
        perror("bind failed");
        exit(EXIT_FAILURE);
    }

    printf("Current Time   \t  Philo #0  |  Philo #1 | Philo #2  | Philo #3  | Philo #4 \n");
    //for(;;)
    //{ 
    	int len, n;
	while(numfinish < 5) {

	//receieve UDP message from Philosphers
    	if (recvfrom(sockfd,buf, BUFLEN,0, ( struct sockaddr *) &cliaddr, &len)==-1)
	{
		 printf("server recvfrom() error");
	}
	
	//scan philospher number and status from response from UDP
	sscanf(buf, "%d %c", &philospher_number, &status);

	time( &curTime );
	timeInfo = localtime ( &curTime );
	printf("%s %d %02d:%02d:%02d\t", monthName[timeInfo->tm_mon],timeInfo->tm_mday, timeInfo->tm_hour, timeInfo->tm_min, timeInfo->tm_sec);
	
	//Loop to display Philosopher status
	for(i = 0; i < 5; i++) {
			putchar('|');
			if(i == philospher_number) {
				switch(status) {
				case PHILO_thinking : 
					printf(" thinking  ");
					 break;
				case PHILO_waiting :
					printf(" waiting   ");
					break;
				case PHILO_eating :
					printf(" eating    "); 
					break;
				case PHILO_finish :
					printf(" finished  ");
			        	numfinish++;
			        	break;
				}
			} else {
				printf(" ......... ");
			}
		}
	printf("\n");

	}//WHILE ends

     //} //for ends

    close(sockfd); 
    return 0;
}
