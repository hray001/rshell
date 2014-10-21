#include <iostream>
#include <string.h>
#include <cstring>
#include <cstdlib>
#include <errno.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>


using namespace std;

int main(){

    while(1){
   
    cout << "$ ";
    string input;
    //char *str;
    char *argv[128];
    char *token, *cmd;
    char tk[] = " ";
    int argc = -1;
    char semicolon = ';';
    getline(cin, input);

    if(input == "exit"){
        exit(1);
    }

    char *str = new char[input.size()+1];
    unsigned i; 
        for(i = 0; i < input.length(); i++){
            if(input[i] == '#'){
                break;
            }
            str[i] = input[i];
        }
    str[i] = '\0';


//---------------work in progress-------------------------------------
    


//--------------------------------------------------------------------
    token = strtok(str, tk);
    cmd = token;
    argv[++argc] = token;

    while(token != NULL){
        cout << token << endl;
        token = strtok(NULL,tk);
        argv[++argc] = token;
    }
    argv[++argc] = '\0';

    int pid = fork();
        if(pid == -1){
            perror("fork failed");
        }
        else if(pid == 0){
    //child 
            if(-1 == execvp(cmd,argv ) ){
                perror("execvp error");
            }
            exit(1);
        }
        else{
            if(wait(0) == -1){
                perror("wait() error");
            }
        }   
    }
        return 0;
    
}
