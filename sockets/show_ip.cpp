#include<stdio.h>
#include<unistd.h>
#include<sys/types.h>
#include<sys/socket.h>
#include<netdb.h>
#include<arpa/inet.h>
#include<netinet/in.h>
#include<iostream>
using namespace std;
/*
 struct addrinfo {
               int              ai_flags;
               int              ai_family;
               int              ai_socktype;
               int              ai_protocol;
               socklen_t        ai_addrlen;
               struct sockaddr *ai_addr;
               char            *ai_canonname;
               struct addrinfo *ai_next;
           };
struct sockaddr_in {
    short int          sin_family;  // Address family, AF_INET
    unsigned short int sin_port;    // Port number
    struct in_addr     sin_addr;    // Internet address
    unsigned char      sin_zero[8]; // Same size as struct sockaddr
};
*/
int main(int argc, char **argv){
    struct addrinfo hints, *res, *p;
    void *addr;
    char ipstr[INET6_ADDRSTRLEN];
    memset(&hints, 0, sizeof(hints));
    char *ipver;
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    if(argc < 2){cout<<"Error Usage: showip hostname\n"; return 0;}
    int status = getaddrinfo(argv[1], NULL, &hints, &res); // char *node, char *service , struct addrinfo *hints, struct addrinfo **res
    if(status != 0){
        cout << "ERROR: "<< status <<"\n";
        return 0;
    }

    printf("ip addresses for %s\n\n", argv[1]);
    for(p=res;p!= NULL;p=p->ai_next){
        if(p->ai_family == AF_INET){// AF_INET address family refer to IPV4 , PF_INET is protocol family. although both are same
            struct sockaddr_in *ipv4 = (struct sockaddr_in *) p->ai_addr;
            addr = &(ipv4->sin_addr);
            ipver = "IPV4";
        } else {
            struct sockaddr_in6 *ipv6 = (struct sockaddr_in6 *) p->ai_addr;
            addr = &(ipv6->sin6_addr);
            ipver = "IPV6";
        }
        inet_ntop(p->ai_family, addr, ipstr, sizeof(ipstr));
        printf("  %s: %s\n", ipver, ipstr);
        cout<< p->ai_flags << " "<<p->ai_socktype<<" "<<p->ai_protocol<< " "<<p->ai_addrlen<<" "<<(p->ai_canonname ? p->ai_canonname:"NULL")<<"\n";
    }
    freeaddrinfo(res);
    return 0;
}
            
            


