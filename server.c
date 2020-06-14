#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdlib.h>

#define PORT 54188
#define MAX_CLIENT 10

struct client {
	int s;
	char name[50];
	int online;
};

static int sock;
static char rbuf[1024];
static char wbuf[1024];

static struct sockaddr_in server,client;
static struct client cli[MAX_CLIENT];

int main (int argc,char *argv[]) {
	int max_sock;
	int len;

	sock=socket(AF_INET,SOCK_STREAM,0);
	if (sock == -1) {
		perror("socket");
		return 1;
	}

	max_sock=sock;

	bzero(&server,sizeof(server));
	server.sin_port=htons(PORT);
	server.sin_addr.s_addr=INADDR_ANY;
	server.sin_family=AF_INET;
	if (bind(sock,(struct sockaddr *)&server,sizeof(struct sockaddr)) == -1) {
		perror("bind");
		return 2;
	}

	listen(sock,MAX_CLIENT);
	
	int i=0,j=0;
	fd_set rfd;
	while (1) {
		FD_ZERO(&rfd);
		FD_SET(sock,&rfd);
		for (i=0;i<MAX_CLIENT;i++) {
			if (cli[i].s == 0)
				continue;
			FD_SET(cli[i].s,&rfd);
		}
		switch (select(max_sock+1,&rfd,NULL,NULL,NULL)) {
			case 0:
				continue;
			case -1:
				continue;
			default:
				if (FD_ISSET(sock,&rfd)) {
					for (i=0;i<MAX_CLIENT;i++) {
						if (cli[i].s == 0) {
							len=sizeof(struct sockaddr);
							cli[i].s=accept(sock,(struct sockaddr *)&client,&len);
							if (cli[i].s == -1) {
								perror("accept");
								cli[i].s=0;
								break;
							}
							cli[i].online=0;
							if (cli[i].s>max_sock)
								max_sock=cli[i].s;
							printf("A user enters the music chat room!\n");
							break;
						}
					}
				}
		
			for (i=0;i<MAX_CLIENT;i++) {
				if (cli[i].s == 0)
					continue;
				if (FD_ISSET(cli[i].s,&rfd)) {
					int rsize;
					bzero(rbuf,sizeof(rbuf));
					rsize=recv(cli[i].s,rbuf,sizeof(rbuf),0);
					if (rsize>0) {
						printf("%s\n",rbuf);
						if (cli[i].online) {
							for (j=0;j<MAX_CLIENT;j++) {
								if (cli[j].s == 0 || j == i || cli[j].online == 0)
									continue;
								bzero(wbuf,sizeof(wbuf));
								strcpy(wbuf,cli[i].name);
								strcat(wbuf,rbuf);
								send(cli[j].s,wbuf,strlen(wbuf)+rsize,0);
							}
							if (!strcmp(rbuf,"EMCR\n")) {
								send(cli[i].s,"quit",4,0);
								close(cli[i].s);
								cli[i].s=0;
								cli[i].online=0;
								memset(cli[i].name,0,sizeof(cli[i].name));
							}
						} else {
							if (!strncmp(rbuf,"#name",5)) {
								strcpy(cli[i].name,rbuf+5);
								cli[i].online=1;
							}
						}
					}
				}
			}

		}
	
	}

	return 0;
}


