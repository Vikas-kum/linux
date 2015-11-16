#include<iostream>
#include<sys/types.h>
#include<sys/socket.h>
#include<signal.h>
#include<unistd.h>
#include<errno.h> //--> contains int errno , that holds error information for a lot of system calls
//The header file errno.h lists a bunch of constant symbolic names for errors, such as EADDRINUSE, EPIPE, ECONNREFUSED
//
#include <netinet/in.h> // contains 
#include<netdb.h> //contains struct addrinfo
#include <arpa/inet.h> //inet_ntop is declared here
using namespace std;
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
 * sd = socket(res->ai_family/ *IPV4 or IPV6* /, res->ai_socktype / * SOCK_STREAM or DATAGRAM * /, res->ai_protocol)
 * bind(sd, res->ai_addr, res->ai_addrlen)
 *
 * accept
 */

/*
 * Sigchld handler for reaping zombie processes
 * http://www.microhowto.info/howto/reap_zombie_processes_using_a_sigchld_handler.html
 */
void sigchld_handler(int s){
 // waitpid() might overwrite errno, so we save and restore it:
    int saved_errno = errno;
    while(waitpid(-1/*for all child processes*/, NULL/*stat loc, we dont want to hold the information about the processes*/, WNOHANG/*dont block*/) > 0); 
   // above while loop ensures that all the child processes are reaped, because once a process signals goes in pending state, all other signals are 
   //ignored. So it can happen that after we get the first signal, and inside this handler, we get 5 other signals, 1 gets into PENDING and other 5 are ignored.
    
  
    errno = saved_errno; 
}

void install_sigchld_handler(){
  struct sigaction sa;
  sa.sa_handler = sigchld_handler;
  sigemptyset(&sa.sa_mask);
  sa.sa_flags = SA_RESTART;
  if (sigaction(SIGCHLD, &sa, NULL) == -1) {
    perror("sigaction");
    exit(1); 
  }
}

// get sockaddr, IPv4 or IPv6:
void *get_in_addr(struct sockaddr *sa)
{
    if (sa->sa_family == AF_INET) {
        return &(((struct sockaddr_in*)sa)->sin_addr);
}
    return &(((struct sockaddr_in6*)sa)->sin6_addr);
}

/*
 * We will create a socket on which we can accept connections
 * */
int main(int argc, char **argv){
    if(argc < 2){
        cout<<"USAGE: create-server PORT number must be specified as first argument to program";
        return 0;
    }
    struct addrinfo hint,*res, *p;
    char s[INET6_ADDRSTRLEN];
    memset(&hint, 0, sizeof(hint));
    hint.ai_family=AF_UNSPEC;//AF_INET for ipv4, AF_INET6 for ipv6, AF_UNSPEC for any ipv4orv6
    hint.ai_socktype = SOCK_STREAM;
    hint.ai_flags = AI_PASSIVE; 
    int ret = getaddrinfo(NULL, argv[1], &hint, & res);
    //assuming there is a check that ret is 0, success
    int sd = -1;
    for(p=res;p!=NULL;p=p->ai_next){
        sd = socket(p->ai_family, p->ai_socktype, p->ai_protocol);

        if(sd != 1){
            cout<<"Created Socket with family :"<<p->ai_family<<" socktype:"<<p->ai_socktype<<" protocol:"<<p->ai_protocol<<endl;
            //success
            break;
        }else {
         //check errno , man errno
        }
    }
    int bndres = bind(sd, p->ai_addr, p->ai_addrlen );//int bind(int sockfd, const struct sockaddr *addr,socklen_t addrlen);
    if(bndres != -1){
        cout<<" Bind socket to address:("<<endl;//<<pi->addr->
    //check bndres =-1 then check global var errno ,
    }
    listen(sd, 10/*BACKLOG*/);//The backlog argument defines the maximum length to which the queue of pending connections for sockfd may grow
    struct sockaddr_storage their_addr;
    socklen_t addr_size = sizeof (their_addr);
    // Now we will accept connections on socket , before accepting connections we need to setup SIGCHLD handler
    // For every connection we will create a child process, that child process will be responsible
    // for processing the new connection.
    // Once the child process terminates, the child process becomes zombie until parent will reap the child process.
    // For reaping the zombie process we will install a SIGCHLD handler which will wait for
    // all child process in while loop. We will do this wait st it doesn't block.
    // See: http://www.microhowto.info/howto/reap_zombie_processes_using_a_sigchld_handler.html
    // TODO: Do we really need to ask for status of each child process ?? How costly is asking the status?
    
    install_sigchld_handler();
    //now we will start accepting cnnections, each accept will create a new socket , we will fork a new process to communicate(send or receive on socket descriptor)
    // the forked process should close the parent socket descriptor which it inherited, sd, in our case
    // parent should close the new_fd once the child inherits it. Note that closing the socket on parent and doesnt close it from child.
    while(1){

      int newfd = accept(sd, (struct sockaddr *)&their_addr, &addr_size);/*It extracts the first connection request on the queue of pending connections for the listening socket, sockfd, creates a new connected socket, and returns a new file descriptor referring to that socket. The newly created socket is not in the listening state. The original socket sockfd is unaffected by this call.*/
      if (newfd == -1) {
         perror("accept");
         continue; 
      }
      inet_ntop(their_addr.ss_family, get_in_addr((struct sockaddr *)&their_addr), s, sizeof s);
      printf("server: got connection from %s\n", s);

      int chld_pid = fork();
      if(chld_pid == 0){
         close(sd);//dont need parent's main descriptor. close it
         send(newfd, "Hello World!", 12, 0);
         close(newfd);
         exit(0);
      }
      close(newfd);//dont need the descriptor, cild is already working on it.
    }
    freeaddrinfo(res);
    
    return 0;
}
