#include <stdio.h>
#include <sys/types.h>
#include<stdlib.h>
#include<unistd.h>
#include<string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include<signal.h>
int sockfd;
void error(char *msg)
{
  perror(msg);
  exit(0);
}
void sigintHandler(int sig_num)
{
    send(sockfd,"/quit\n",sizeof("/quit\n"),0);
	signal(SIGINT, sigintHandler);
	close(sockfd);
    exit(1);
}

void sighandler(int sig_num)
{
    send(sockfd,"/quit\n",sizeof("/quit\n"),0);
    signal(SIGTSTP, sighandler);
	close(sockfd);
    exit(1);
}

int main(int argc,char *argv[])
{
    int portno=5000;
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    struct hostent *server;
    server = gethostbyname("127.0.0.1");
    if (server == NULL)
   {
    fprintf(stderr,"ERROR, no such host");
    exit(0);
   }
    struct sockaddr_in serv_addr;
    bzero((char *) &serv_addr, sizeof(serv_addr)); // initializes buffer
    serv_addr.sin_family = AF_INET; // for IPv4 family
    bcopy((char *)server->h_addr, (char *) &serv_addr.sin_addr.s_addr, server->h_length);
    serv_addr.sin_port = htons(portno); //defining port number
    if (connect(sockfd,(struct sockaddr *) &serv_addr,sizeof(serv_addr)) < 0)
    {
         error("ERROR connecting");
    }
    char send_buffer[1025],receive_buffer[1025];
    printf("Enter the commands and type /quit to STOP\n");
    signal(SIGINT, sigintHandler);
    signal(SIGTSTP, sighandler);
    //int n;
    //n=recv(sockfd,receive_buffer,strlen(receive_buffer),0);
    //printf("%s",receive_buffer);
    if(fork()==0)
    {
    while(1)
    {
    
        // client buffer to forward request to the server
        
        if(recv(sockfd,receive_buffer,sizeof(receive_buffer),0)==0)
        {
            printf("error occured during receiving");
            break;
            
        }
        if(strcmp(receive_buffer,"exit")==0)
        {
            break;
        }
        printf("Message from server:\n");
        printf("%s",receive_buffer);
        bzero(receive_buffer,1024);
            
        }
    }
    else
    {
        while(1)
        {
    
    
        
    bzero(send_buffer,1024);
    fgets(send_buffer,sizeof(send_buffer),stdin);
    if(send(sockfd,send_buffer,sizeof(send_buffer),0)==0)
    {
        printf("error");
        break;
    }
    if(strncmp(send_buffer,"/quit",5)==0 && strlen(send_buffer)==6)
    {
        break;
    }
        }
   }
    close(sockfd);
    return 0;
}
