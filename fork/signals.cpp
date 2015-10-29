#include<iostream>
#include<unistd.h>
#include<signal.h>
/*
 * LIMITATIONS:
 * 1.Async safe functions shouldonly be called in signal handler.
 * Async safe function are functions are functions which can be restarted
 * safely even after interrupt. There are some functions defined by different systems as async safe.
 *For ex: if a process doing printf is interrupted and handler contains printf, both printf can intermingle
 * Similalry if interrupts happen when a process is doin malloc(malloc is not atomic process), and handler is 
 * malloc, those can be problematic.
 *
 * 2. Only atomic lock free global varibale can be acceseed or modified in handler.
 *  Those varibale should have volatile keyword so that processor should know not to play
 *  with thsoe during optimization.
 *  volatile sig_atomic_t flag;
 *  volatile boolean can be considered as atomic on architecture whose word size is upto 64 bits.
 *
 *  3. Once a process enters signal handler, any furhter same signal is always put in PENDING, if a signal is
 *  in PENDING state and another same signal is given, nothing will happen.
 *  IF a processs gets a different signal while is handling a signal and process is not masking that signal
 *  process will interrupted for that different signal.
 *  For Ex, if we dont set sa_mask below, and signals are ^C^\^C^\^C^C^\
 *  after ^C sigint handler will be called, as it is executing and sigQUIT comes,
 *  the process will jump to sigquit handler, next ^C will go in pending ^\ will also go in pending.
 *  further ^C and ^\ will be ignored because both the signals are already in pending.
 *
 *  4. If 2 signals are in pending state, any of them can be chosen next.
 *  5. A process can signalled/ interrupted while sleeping
 */
void sigint_handler(int sig){
   // int ret = write(0, "Ahhh! SIGINT!\n", 14);
   // if(ret == -1){
   //   exit(1);
   // }
    sleep(10);
    for(int i=0;i<10;i++) sleep(1);
    write(0,"exiting\n",8);
}
void sigquit_handler(int sig){
    write(0,"SigQuit\n",8);
    for(int i=0;i<2;i++)sleep(5);
    write(0,"exitSQ\n",7);
}

int main(){
    struct sigaction sa;
    sa.sa_handler = sigint_handler;
    sa.sa_flags = SA_RESTART;//Flags to modify the behavior of the handler, or 0
    sigemptyset(&sa.sa_mask);// sa_mask --> A set of signals to block while this one is being handled
     if (sigaction(SIGINT, &sa, NULL) == -1) {
                 perror("sigaction");
                         exit(1);
                             }
    struct sigaction sb;
    sb.sa_handler = sigquit_handler;
    sb.sa_flags=0;
    sigemptyset(&sb.sa_mask);
     sigaction(SIGQUIT, &sb, NULL);
     char buf[100];

    printf("my pid:%d\n", getpid());
    sleep(20);
    while(gets(buf)){
      std:: cout<< buf<<"\n";
    }
    return 0;
}


