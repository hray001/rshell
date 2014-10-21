#include <iostream>
#include <string.h>
#include <cstring>
#include <cstdlib>
#include <errno.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <vector>

using namespace std;

int main(){

    string user = getlogin();
    if(user == " "){
        perror("getlogin() failed to attain username");
        user = "usererror";
    }
    char host[1024];
    int len = 1024;
    int hosterror = gethostname(host, len);
    if(hosterror == -1){
        perror("gethostname() failed to attain host name");
    }
    while(1){
   
    cout << user << "@" << host << "$ ";
    string input;
    char *argv[128];
    char *token, *cmd;
    char tk[] = " ";
    int argc = -1, semicount = 0;
    
    char semicolon[] = ";";
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
            if(input[i] == semicolon[0]){
                semicount++;
            }
            str[i] = input[i];
        }
    str[i] = '\0';


//---------------work in progress-------------------------------------
    vector <char *> connectorarr; 
    connectorarr.resize(128);
    char *arr = new char[semicount]; 
    arr = strtok(str, semicolon);
    connectorarr[++argc] = arr;
    while(arr != NULL){
        //cout << arr << endl;
        arr = strtok(NULL, semicolon);
        connectorarr[++argc] = arr;
    }
    connectorarr[++argc] = '\0';
/*for(int j = 0; j< argc - 1;j++){
cout << connectorarr[j]<< " " << argc << " "<< j << endl;
}
*/


//--------------------------------------------------------------------
unsigned iterations = argc;
        for(unsigned j = 0; j < iterations ; j++){
        argc = -1;
        token = strtok(connectorarr[j], tk);
        cmd = token;
        argv[++argc] = token;

            while(token != NULL){
            // cout << token << endl;
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
    }
        return 0;
    
}
