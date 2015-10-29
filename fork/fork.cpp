#include<iostream>
#include<unistd.h>
using namespace std;
int childfun(void);
int main(){
    int rv;
    cout << " I am parent with pid :"<<getpid()<<endl;
    pid_t child_pid = fork();
    if(child_pid == 0){
      cout << "I am child :"<<getpid()<<" My parent is :"<<getppid()<<endl;
      (*childfun)();
      cin>> rv;
      exit(rv); // this value is sent to parent which is waiting.
    }else {
      cout<<"waiting for child pid:"<<child_pid<<endl;
      wait(&rv);//waitpid can be used to specify exact process to wait for. wait will simply wait for first exiting process
      printf("PARENT: My child's exit status is: %d\n", WEXITSTATUS(rv));

    }
    return 0;
}
int childfun(){
      cout << "I am child :"<<getpid()<<endl;
      return 0;
}

