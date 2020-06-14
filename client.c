#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static int sock;
static char rbuf[1024];
static char wbuf[1024];

static struct sockaddr_in server,client;

int main (int argc,char *argv[]) {
	int i,j,err;
	char name[100];
	int rsize,wsize;
	sock=socket(AF_INET,SOCK_STREAM,0);
	if (sock == -1) {
		perror("socket");
		return 1;
	}
	
	bzero(&server,sizeof(server));
	server.sin_family=AF_INET;
	server.sin_addr.s_addr=inet_addr("127.0.0.1");
	server.sin_port=htons(54188);
	err=connect(sock,(struct sockaddr *)&server,sizeof(struct sockaddr));
	if (err == -1) {
		perror("connect");
		return 2;
	}
	printf("connect success!\n");
	bzero(rbuf,sizeof(rbuf));
	bzero(name,sizeof(name));
	rsize=read(STDIN_FILENO,rbuf,sizeof(rbuf));
	rbuf[strlen(rbuf)-1]='\0';
	sprintf(name,"#name%s:",rbuf);
	send(sock,name,sizeof(name),0);

	j=0;i=0;
	fd_set rfd;
	FD_ZERO(&rfd);
	FD_SET(STDIN_FILENO,&rfd);
	FD_SET(sock,&rfd);
	while(1){
		FD_ZERO(&rfd);
		FD_SET(STDIN_FILENO,&rfd);
		FD_SET(sock,&rfd);
		switch (select(sock+1,&rfd,NULL,NULL,NULL)) {
			case -1:
			case 0:
				continue;
			default:
				if (FD_ISSET(STDIN_FILENO,&rfd)) {
					bzero(rbuf,sizeof(rbuf));
					rsize=read(STDIN_FILENO,rbuf,sizeof(rbuf));
					if (rsize>0) {
						send(sock,rbuf,rsize,0);
					}
				}

				if (FD_ISSET(sock,&rfd)) {
					bzero(rbuf,sizeof(rbuf));
					rsize=recv(sock,rbuf,sizeof(rbuf),0);
					if (rsize>0) {
						printf("\033[31m%s\033[34m\n",rbuf);
						if (!strcmp(rbuf,"quit")) {
							close(sock);
							return 11;
						}
							
					}
				}
		}
	
	}

	return 0;
}

