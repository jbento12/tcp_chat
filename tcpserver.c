#include <sys/socket.h>
#include <sys/types.h>
#include <resolv.h>
#include <pthread.h>
#include <netdb.h>
#include <stdlib.h>
#include <string.h>


void panic(char *msg);
#define panic(m)	{perror(m); abort();}
#define MAX_CLIENTS 10

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t mutex2 = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t mutex3 = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t cond_sig;
pthread_cond_t cond_sig2;

void insert_msg(char* msg, int arg, char* name);
int arrayNull(char* arr);
void *threadfunction(void *arg);
void *threadfunction_s(void *arg);
void *threadfunction_time(void *arg);
void broadcast_msg(int sd);

int client_number = 0;

typedef enum status{

    AFK ,
    ONLINE

}status_t;

struct client_list{
	int sd;
	char name[100];
	int nmr;
	status_t status;

	struct client_list *next;
}; 
typedef struct client_list client_t;

struct message{

	char mesg[256];
	client_t client;

} ;
typedef struct message message_t;

struct node
{
    message_t message;
    struct node *next;

}; typedef struct node node_t;

node_t *message_list = NULL;
client_t *client_list = NULL;

char send_buffer[256] = {0};

struct timespec curr_time;

int main(int count, char *args[])
{	
	struct sockaddr_in addr;
	int listen_sd, port;
	int sd;

	if ( count != 2 )
	{
		printf("usage: %s <protocol or portnum>\n", args[0]);
		exit(0);
	}

	/*---Get server's IP and standard service connection--*/
	if ( !isdigit(args[1][0]) )
	{
		struct servent *srv = getservbyname(args[1], "tcp");
		if ( srv == NULL )
			panic(args[1]);
		printf("%s: port=%d\n", srv->s_name, ntohs(srv->s_port));
		port = srv->s_port;
	}
	else
		port = htons(atoi(args[1]));

	/*--- create socket ---*/
	listen_sd = socket(PF_INET, SOCK_STREAM, 0);
	if ( listen_sd < 0 )
		panic("socket");

	/*--- bind port/address to socket ---*/
	memset(&addr, 0, sizeof(addr));
	addr.sin_family = AF_INET;
	addr.sin_port = port;
	addr.sin_addr.s_addr = INADDR_ANY;                   /* any interface */
	if ( bind(listen_sd, (struct sockaddr*)&addr, sizeof(addr)) != 0 )
		panic("bind");

	/*--- make into listener with 10 slots ---*/
	if ( listen(listen_sd, 10) != 0 )
		panic("listen")

	/*--- begin waiting for connections ---*/
	else
	{	
		                       /* process all incoming clients */
		while(1){
			int n = sizeof(addr);
			 sd = accept(listen_sd, (struct sockaddr*)&addr, &n);     /* accept connection */
			if(sd!=-1)
			{
				pthread_t child, child_send, child_time;
				if(client_number < MAX_CLIENTS)
				{
                curr_time.tv_sec = time(NULL);
				client_number ++;
				pthread_create(&child, 0, threadfunction, &sd);       /* start thread */
                pthread_create(&child_send, 0, threadfunction_s, &sd);
                pthread_create(&child_time, 0, threadfunction_time, &sd);
                
				//pthread_detach(child);                     [index] /* don't track it */
				// pthread_join(&child, NULL);
				// pthread_join(&child_send, NULL);

				}
				//printf("New connection\n");
				
                }
			
		}
	}
}
void *threadfunction_time(void *arg)
{
	struct timespec passed_time;
    int sd = *(int*)arg;
    char buffer[256]={0};
	curr_time.tv_sec = time(NULL);
    while(1)

    {   

		pthread_mutex_lock(&mutex2);
		pthread_cond_wait(&cond_sig2, &mutex2);

		pthread_mutex_unlock(&mutex2);
		sleep(10);
		pthread_mutex_lock(&mutex2);

		client_t* client = client_list;

		while(client != NULL) {
			if(client->sd != sd)
			client = client->next;
			else break;
		}

		if(client->status == ONLINE)
		{
			strcpy(buffer, client->name);
			strcat(buffer, " status is : ");
			strcat(buffer, "ONLINE");
			printf("%s\n", buffer);
		}
		else
		{	
			strcpy(buffer, client->name);
			strcat(buffer, " status is : ");
			strcat(buffer, "ONLINE");
			printf("%s\n", buffer);

		}
		memset(buffer, 0 , sizeof(buffer));
		pthread_mutex_unlock(&mutex2);

        }
    

}



void *threadfunction(void *arg)                    
{	
    
    int name_recv = 0;
	int sd = *(int*)arg;
    char buffer[256]={0};

    
	client_t *cli = (client_t*)malloc(sizeof(client_t));
	//inserir sd no cliente e inserir na lista
	cli->sd = sd;

    if(recv(sd,buffer,sizeof(buffer),0))
    strcpy(cli->name, buffer);
   
	cli->next=client_list;
	client_list = cli;
	
	client_t *client = client_list;
	// memset(buffer, 0, sizeof(buffer));
    while (1){     
	


	if(recv(sd,send_buffer,sizeof(send_buffer),0)){

		while (cli->sd != sd) cli = cli->next;

        if(!(strcmp(send_buffer,"AFK")&&strcmp(send_buffer,"ONLINE")))
		{
			if(!strcmp(send_buffer,"AFK"))
			cli->status = AFK;
			else cli->status = ONLINE;

			pthread_cond_signal(&cond_sig2);
		}
		else 
        {
            insert_msg(send_buffer, sd, cli->name);
            curr_time.tv_sec = time(NULL);
            cli->status = ONLINE;
        }
        
	}
	
	else{



	}
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
    int sd = *(int*)arg;
    while (1){
		pthread_mutex_lock(&mutex);
		pthread_cond_wait(&cond_sig,&mutex);
		broadcast_msg(sd);
    }
     
}

void insert_msg(char* msg, int sd, char* name)
{
	pthread_mutex_lock(&mutex);
	
	if(!arrayNull(msg)){

		node_t *aux = (node_t*)malloc(sizeof(node_t));
		strcpy(aux->message.mesg, msg);
		aux->message.client.sd = sd; 
        strcpy(aux->message.client.name, name);
		aux->next = message_list;
		message_list=aux;
		pthread_cond_signal(&cond_sig);
        memset(send_buffer, 0, sizeof(send_buffer));
	}
	pthread_mutex_unlock(&mutex);
}


int arrayNull(char* arr)
{
	for(int i=0; i<sizeof(arr); i++)
	{
		if(arr[i]!=0) return 0;
	}
	return 1;
}

//receive and insert message in list
//missing : broadcast received message
//			
// bool isEmpty() {
//    return message_list == NULL;
// }

void broadcast_msg(int sd) {

	pthread_mutex_unlock(&mutex);
	pthread_mutex_lock(&mutex);
   //start from the first link
   client_t* client = client_list;
	node_t* current = message_list;
	char buffer[256]={0};
    char said[10] = " said:";
   //if list is empty
   if(message_list == NULL) {
      return;
   }

   //navigate through list
   while(current!= NULL) { //while it isn't last node
	//send to all clients

	strcpy(buffer,current->message.client.name);
    strcat(buffer, said);
	strcat(buffer, current->message.mesg);

	while(client!= NULL){

		if(current->message.client.sd != client->sd){

		send(client->sd,buffer,sizeof(buffer),0);
		client = client->next;
		} // rever esta funç
		else 
		client = client->next;
    	
	}

	current = current->next;
	
	}
	pthread_mutex_unlock(&mutex);
}  
