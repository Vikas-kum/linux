#include<sys/types.h>
#include<sys/socket.h>
#include<iostream>
#include<unistd.h>
#include<netdb.h>
#include <arpa/inet.h>
// get sockaddr, IPv4 or IPv6:
void *get_in_addr(struct sockaddr *sa)
{
    if (sa->sa_family == AF_INET) {
        return &(((struct sockaddr_in*)sa)->sin_addr);
}
    return &(((struct sockaddr_in6*)sa)->sin6_addr);
}

#define MAXDATASIZE 20
int main(int argc, char **argv){

    struct addrinfo hint, *res, *p;
    memset(&hint, 0, sizeof hint);
    hint.ai_family = AF_UNSPEC;
    hint.ai_socktype = SOCK_STREAM;
    int rv;
    char buf[MAXDATASIZE];
    if((rv = getaddrinfo(argv[1], argv[2], &hint, &res)) != 0){
      fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
      return 1;
    }
    int sd;
    for(p=res;p!=NULL;p=p->ai_next){
        sd = socket(p->ai_family, p->ai_socktype, p->ai_protocol);
      if(sd == -1){
          perror("client:socket");
          continue;
      }
      if(connect(sd, p->ai_addr, p->ai_addrlen) ==-1 ){
        perror("client:connect");
        continue;
      }
      break;
    }

    if(p==NULL){
        perror("failed to connect\n");
    }
    inet_ntop(p->ai_family,get_in_addr((struct sockaddr*)p->ai_addr),buf, sizeof buf);
    printf("client: connecting to %s\n", buf);
    freeaddrinfo(res); // all done with this structure
    int numbytes;
    if ((numbytes = recv(sd, buf, MAXDATASIZE-1, 0)) == -1) {
      perror("recv");
      exit(1); 
    }
    buf[numbytes] = '\0';
    printf("Got data%s", buf);
    close(sd);
    return 0;
}

