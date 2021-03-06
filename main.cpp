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

   // if(input == "exit"){
   //     exit(1);
   // }

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
    string end = "exit";
    int endcount = 0;
    for(unsigned i = 0; i < 5; i++){
        if(str[i] != end[i]){
            break;
        }
        else{
            endcount++;
        }
    }
    if(endcount >= 4){
        exit(1);
    }
//-------------------------split tokens by ';'-------------------------------
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


//---------------------------------------------------------------------------
unsigned iterations = argc; //Number of commands separated by ';'
        for(unsigned j = 0; j < iterations-1 ; j++){
            char andchar[] = "&|";
            vector <char *> andarr;
            andarr.resize(128);
            char *andarr2 = new char[128];
            andarr2 = strtok(connectorarr[j], andchar);
            andarr[0] = andarr2;
            unsigned cand = 0;
            while(andarr2 != NULL){
                //cout << "andarr2 " << andarr2 << endl;
                andarr2 = strtok(NULL, andchar);
                andarr[++cand] = andarr2;
            }
unsigned it = 0;
for(; it <= cand; ++it){
//cout << "iterations: " << it << endl;
//-----------------------space token and system calls-----------------------
        //cout << "cand " << cand << endl;
        argc = -1;
        token = strtok(andarr[it], tk);
        cmd = token;
        argv[++argc] = token;

            while(token != NULL){
//             cout << token << endl;
                token = strtok(NULL,tk);
                argv[++argc] = token;
            }
        argv[++argc] = '\0';

        int pid = fork();
            if(pid == -1){
                perror("fork");
            }
            else if(pid == 0){
            //child 
                if(-1 == execvp(cmd,argv ) ){
                    perror("execvp");
                }
                exit(1);
            }
            else{
                if(wait(0) == -1){
                    perror("wait()");
                }
            }      
}
//---------------------------------------------------------------------------
        }
    }
        return 0;
    
}
