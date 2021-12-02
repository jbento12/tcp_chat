/*****************************************************************************/
/*** tc
 *  an TCP client.                                            ***/
/*****************************************************************************/

#include <stdio.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <resolv.h>
#include <netdb.h>
#include <stdlib.h>
#include <string.h>

void panic(char *msg);
#define panic(m)	{perror(m); abort();}
/****************************************************************************/
/*** This program opens a connection to a server using either a port or a ***/
/*** service.  Once open, it sends the message from the command line.     ***/
/*** some protocols (like HTTP) require a couple newlines at the end of   ***/
/*** the message.                                                         ***/
/*** Compile and try 'tcpclient lwn.net http "GET / HTTP/1.0" '.          ***/
/****************************************************************************/

typedef enum status{

    AFK ,
    ONLINE

}status_t;


struct arg_struct {
	int sd;
	char name[100];
    status_t status;
}args;

struct timespec curr_time;

void *threadfuntion(void *arg)                    
{	
	struct arg_struct arguments= *(struct arg_struct*) arg;
	int sd = arguments.sd;
	char buffer[256] = {0};
    while (1){
	//int sd = *(int*)arg;            /* get & convert the socket */
	if(recv(sd,buffer,sizeof(buffer),0))
	printf("%s\n",buffer);
    }
    
	//send(sd,buffer,sizeof(buffer),0);
	// shutdown(sd,SHUT_RD);
	// shutdown(sd,SHUT_WR);
	// shutdown(sd,SHUT_RDWR);
			                  /* close the client's channel */
	                           /* terminate the thread */
}

void *threadfunction_s(void *arg)
{	
	struct arg_struct arguments= *(struct arg_struct*) arg;
   
    while (1){
    //int sd = *(int*)arg;
	int sd = arguments.sd;
	// strcpy(c_name, arguments.name);
    char buffer[256];
    if(scanf("%[^\n]s", buffer))
   {
     	while((getchar())!='\n');
		//  printf("%s:", c_name);
   	 	curr_time.tv_sec = time(NULL);
    	arguments.status = ONLINE;
    	send(sd,buffer,sizeof(buffer),0);
   }
    }
    
    
}

void *threadfunction_time(void *arg)
{
    struct timespec passed_time;
    struct arg_struct arguments= *(struct arg_struct*) arg;
	int sd = arguments.sd;
    arguments.status = ONLINE;
    curr_time.tv_sec = time(NULL);
	char buffer[256]={0};
  
   

    while(1)
    {   
        passed_time.tv_sec = time(NULL);
			
        if(60 <= passed_time.tv_sec - curr_time.tv_sec)
            arguments.status = AFK;
		else arguments.status = ONLINE;

		if (arguments.status == AFK)
			strcpy(buffer,"AFK");
		else strcpy(buffer,"ONLINE");

		send(arguments.sd,buffer,sizeof(buffer),0);
        
        sleep(1);
    }



}





int main(int count, char *args[])
{	struct hostent* host;
	struct sockaddr_in addr;
	int sd, port;
	struct arg_struct arguments;
   

	if ( count != 3 )
	{
		printf("usage: %s <servername> <protocol or portnum>\n", args[0]);
		exit(0);
	}

// printf("Insert your name: ");
// scanf("%s", arguments.name);
// printf("\nname is %s\n", arguments.name);
	

/*---Get server's IP and standard service connection--*/
host = gethostbyname(args[1]);
//printf("Server %s has IP address = %s\n", args[1],inet_ntoa(*(long*)host->h_addr_list[0]));
if ( !isdigit(args[2][0]) )
{
	struct servent *srv = getservbyname(args[2], "tcp");
	if ( srv == NULL )
		panic(args[2]);
	printf("%s: port=%d\n", srv->s_name, ntohs(srv->s_port));
	port = srv->s_port;
}
else
	port = htons(atoi(args[2]));

/*---Create socket and connect to server---*/
arguments.sd = socket(PF_INET, SOCK_STREAM, 0);        /* create socket */
if (arguments.sd < 0 )
	panic("socket");
memset(&addr, 0, sizeof(addr));       /* create & zero struct */
addr.sin_family = AF_INET;        /* select internet protocol */
addr.sin_port = port;                       /* set the port # */
addr.sin_addr.s_addr = *(long*)(host->h_addr_list[0]);  /* set the addr */

/*---If connection successful, send the message and read results---*/

if ( connect(arguments.sd, (struct sockaddr*)&addr, sizeof(addr)) == 0)
{	

	pthread_t child, child_send, child_time;

    printf("Insert your name\n");
	if(scanf("%[^\n]s", arguments.name))
    {
     while((getchar())!='\n');
    }

    send(arguments.sd,arguments.name,sizeof(arguments.name),0);
  
	if(pthread_create(&child, 0, threadfuntion, &arguments))printf("creation_error");      /* start thread */
	if(pthread_create(&child_send, 0, threadfunction_s, &arguments))printf("creation_error");
    pthread_create(&child_time, 0, threadfunction_time, &arguments );
	pthread_join(child, NULL);
	pthread_join(child_send, NULL);
    pthread_join(child_time, NULL);
}
else{
	panic("connect");
}
}

