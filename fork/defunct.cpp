#include<cstdio>
#include<unistd.h>
#include<iostream>
using namespace std;
int main(){
    cout<<"Parent process id:"<<getpid()<<"\n";
    int cid = fork();
    if(cid != 0){
        sleep(20);
        cout<<"Process returning : this is parent, process id is:"<<getpid()<<"\n";
        return 0;
    }else {
    //this is child process
      int k;
      cout<<"Returning This is child process, pid is :"<<getpid()<<"\n";
     // cin>>k;
    }
}
      
    

    
