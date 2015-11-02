#include<iostream>
#include<sys/types.h>
#include<sys/sockets.h>
/*
 * This file tells us how to create socket.
 * 1.First step is to getaddrinfo , see show_ip.c
 * 2. 2nd step is creating a socket descriptor
 * creating socket requires 3 arguments
 * int PF --> protocol family ->IPV4 or IPV6 or any other see man socket, getaddrinfo()->ai_family or PF_INET or PF_INET6
 * int socket type type --> UDP, TCP or any other getaddrinfo()->ai_socktype
 * int protocol --> we can set it to 0 to chose proper protocol for give type, or call getprotobyname("")
 * so, socket(res->ai_family, res->ai_sockettype, res->ai_protocol) will create a socket descriptor
 * 3. Once we have a socket descriptor, we need to bind this descriptor to a port
 * using bind(int socket_descriptor, 
 *            getaddrinfo()->ai_addr ,//address to which to bind this socket to
 *            getaddrinfo()->ai_addrlen)//length of address
 *
 * In this program we will get a address on localhost, specified port on which the application should listen/accept connection
 * We will then create a socket and bind that socket to the address we created so that connections can be accepted on localhost:port
 *
 * To create a adresss so that application can accept connections on localhost:port, we need to specify hints->ai_flags = AI_PASSIVE,
 * node = null // means localhost
 * so getaddrinfo(NULL /\*null node means localhost* / , port / *must be greater > 1000 * /, hints / * ai_flags = AI_PASSIVE * /, res   )
 * Above in getaddrinfo, either of Node or service(port) can be null, in case service is null but node is not null, res will contain unitialized port
 * sd = socket(res->ai_family, res->ai_socktype, res->ai_protocol)
 * bind(sd, res->ai_addr, res->ai_addrlen)
 */
int main(int argc, char **argv){
    struct addrinfo hint,*resi, *p;
    memset(&hint, 0, sizeof(hint));
    hint->ai_family=AF_UNSPEC;//AF_INET for ipv4, AF_INET6 for ipv6, AF_UNSPEC for any ipv4orv6
    hint->ai_socktype = SOCK_STREAM;
    hint->ai_flags = AI_PASSIVE; 
    int ret = getaddrinfo(argv[1], NULL, &hint, & res);
    //assuming there is a check that ret is 0, success
    int sd = -1;
    for(p=res;p!=NULL;p=p->ai_next){
        sd = socket(p->ai_family, p->ai_socktype, p->ai_protocol);
        if(sd != 1){
            //success
            break;
        }else {
         //check errno , man errno
        }
    }
    int bndres = bind(sd, p->ai_addr, p->ai_addrlen );
    //check bndres =-1 then check global var errno ,
    freeaddrinfo(res);
    return 0;
}
