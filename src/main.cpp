#include <iostream>
#include <string.h>
#include <cstring>
#include <cstdlib>
#include <errno.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <signal.h>
#include <vector>
#include <stdlib.h>

using namespace std;

static int pid = -1;

void sig_int_handler(int signum){
    if(pid == 0){
    //child
        if(kill(0, SIGKILL) == -1) perror("kill");
    }
    else if(pid > 0){
    //parent
    //don't want to interrupt parent
    }
    else{
        //in the case pid is -1 it is handled in the calls to fork() where pid would be set to -1
    }
    return;    
}

void ch_cwdir(char *PATH){

    if( chdir(PATH) == -1) perror("chdir");
    char *newcwdbuf;
    cout << getcwd(newcwdbuf, 128) << endl;
    return;   
}

void ch_cwdir(){
    cout << "Error: must specify directory to change to." << endl; 
    return;    
}

int main(){
    signal(SIGINT, sig_int_handler);
    char **COMMANDS;
    COMMANDS[0] = "exit";
    COMMANDS[1] = "cd";



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
   
    cout << user << "@" << host << ":";
    //current working directory to be output
    char *cwdbuf;
    if(getcwd(cwdbuf, 128) == NULL) perror("getcwd");
    cout << cwdbuf << "$ ";
    string input;
    char *argv[BUFSIZ];
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
            char *andarr2 = new char[4096];
            andarr2 = strtok(connectorarr[j], andchar);
            andarr[0] = andarr2;
            unsigned cand = 0;
            while(andarr2 != NULL){
                //cout << "andarr2 " << andarr2 << endl;
                andarr2 = strtok(NULL, andchar);
                andarr[++cand] = andarr2;
            }
            delete andarr2;
unsigned it = 0;
int fd = 0;
int save;
bool redirection = false;
for(; it <= cand; ++it){
//cout << "iterations: " << it << endl;
//-----------------------space token and system calls-----------------------
       //Comment out && and || stuff to work on piping
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
//       int repos = 0;
        unsigned x = 0;
        unsigned last_redir = 0;
       for(x = 0; argv[x] != '\0'; ++x){
            if(strcmp(argv[x], "<") == 0){
                redirection = true;     
                fd = open(argv[x+1], O_RDONLY);
                if(fd < 0) perror("open");
                else{
                    save = x;
                    argv[x] = '\0';    
                    
                    if( (pid = fork()) < 0) perror("fork");
                    else if(pid == 0){
                        //cerr << "inside child" << endl;
                        if(dup2(fd, 0) == -1) perror("dup2");
                        //if(close(fd) == -1) perror("close");
                        //cout << argv[x] << endl;

                        if( strcmp(argv[last_redir], COMMANDS[0]) == 0) exit(1);
                        if( strcmp(argv[last_redir], COMMANDS[1]) == 0){
                            if(argv[last_redir + 1] != '\0'){
                                ch_cwdir(argv[last_redir + 1]);
                            }
                            else{
                                ch_cwdir();
                            }
                        }
                        if(execvp(argv[last_redir], argv) == -1)
                            perror("execvp");
                    }
                    else{
                        if(wait(0) == -1) perror("wait");
                        if(close(fd) == -1) perror("close");
                        char a[1] = {'<'};
                        argv[save] = a;
                        last_redir = x;
                    }
                }
            }
            else if(strcmp(argv[x], "<<<") == 0){
                redirection = true;
                
                save = x;
                argv[x] = '\0';
                
                string str;
                for(int i = 0; argv[x][i] != '\0'; ++i){
                    if(argv[x][i] == '<' || argv[x][i] == '>' 
                        || argv[x][i] == '|'){
                        break;
                    }
                    else{
                        str += argv[x][i];
                    }
                }
                if( (pid = fork()) < 0) perror("fork");
                    else if(pid == 0){
                        //cerr << "inside child" << endl;
                        if(dup2(fd, 0) == -1) perror("dup2");
                        //if(close(fd) == -1) perror("close");
                        if( strcmp(argv[last_redir], COMMANDS[0]) == 0) exit(1);
                        if( strcmp(argv[last_redir], COMMANDS[1]) == 0){
                            if(argv[last_redir + 1] != '\0'){
                                ch_cwdir(argv[last_redir + 1]);
                            }
                            else{
                                ch_cwdir();
                            }
                        }
                        if(execvp(argv[last_redir], argv) == -1)
                            perror("execvp");
                    }
                    else{
                        if(wait(0) == -1) perror("wait");
                        if(close(fd) == -1) perror("close");
                        char a[1] = {'<'};
                        argv[save] = a;
                        last_redir = x;
                    }
            }
            else if(strcmp(argv[x], ">") == 0){
                redirection = true;
                fd = open(argv[x+1], O_WRONLY | O_CREAT | O_TRUNC);
                if(fd < 0) perror("open");
                else{
                    save = x;
                    argv[x] = '\0';
                    
                    if( (pid = fork()) == -1) perror("fork");
                    else if(pid == 0){
                        if( strcmp(argv[last_redir], COMMANDS[0]) == 0) exit(1);
                        if( strcmp(argv[last_redir], COMMANDS[1]) == 0){
                            if(argv[last_redir + 1] != '\0'){
                                ch_cwdir(argv[last_redir + 1]);
                            }
                            else{
                                ch_cwdir();
                            }
                        }
                        if(dup2(fd, 1) == -1) perror("dup2");
                        if(execvp(argv[last_redir], argv) == -1)
                            perror("execvp");
                            exit(1);
                    }else{
                        if(wait(0) == -1) perror("wait");
                        if(close(fd) == -1) perror("close");
                        char a[1] = {'>'};
                        argv[save] = a;
                        last_redir = x;    
                    }
                }
            }
            
            else if(strcmp(argv[x], ">") == 0){
                redirection = true;
                fd = open(argv[x+1], O_WRONLY | O_CREAT | O_TRUNC);
                if(fd < 0) perror("open");
                else{
                    save = x;
                    argv[x] = '\0';
                    if( (pid = fork()) == -1) perror("fork");
                    else if(pid == 0){
                        cout << "last_redir" << last_redir << endl;
                        if( strcmp(argv[last_redir], COMMANDS[0]) == 0) exit(1);
                        if( strcmp(argv[last_redir], COMMANDS[1]) == 0){
                            if(argv[last_redir + 1] != '\0'){
                                ch_cwdir(argv[last_redir + 1]);
                            }
                            else{
                                ch_cwdir();
                            }
                        }
                        if(dup2(fd, 1) == -1) perror("dup2");
                        if(execvp(argv[last_redir], argv) == -1)
                            perror("execvp");
                            exit(1);
                    }else{
                        if(wait(0) == -1) perror("wait");
                        cout << "executed" << endl;
                        if(close(fd) == -1) perror("close");
                        char a[1] = {'>'};
                        argv[save] = a;
                        last_redir = x;    
                    }
                }
            }
            //-------------------------------------------------------------
            else if((argv[x][0] == '0' || argv[x][0] == '1' ||
                    argv[x][0] == '2' || argv[x][0] == '3' ||
                    argv[x][0] == '4' || argv[x][0] == '5'   ||
                    argv[x][0] == '6' || argv[x][0] == '7' || 
                    argv[x][0] == '8' || argv[x][0] == '9')&&  
                    string(argv[x]).find(">>") > 0)
                {
                redirection = true;
                int iopos = string(argv[x]).find(">>");
                int newfd = 0;
                for(int i = 0; i < iopos; ++i){
                    if(i == 0) newfd = argv[x][i] - 48;
                    else{
                        newfd += (argv[x][i] - 48) *10 *i;
                    }
                }
                fd = open(argv[x+1], O_RDWR | O_CREAT | O_APPEND);
                if(fd < 0) perror("open");
                else{
                    save = x;
                    argv[x] = '\0';
                    if( (pid = fork()) == -1) perror("fork");
                    else if(pid == 0){
                        if(dup2(fd, newfd) == -1) perror("dup2");
                        if( strcmp(argv[last_redir], COMMANDS[0]) == 0) exit(1);
                        if( strcmp(argv[last_redir], COMMANDS[1]) == 0){
                            if(argv[last_redir + 1] != '\0'){
                                ch_cwdir(argv[last_redir + 1]);
                            }
                            else{
                                ch_cwdir();
                            }
                        }
                        if(execvp(argv[last_redir], argv) == -1)
                            perror("execvp");
                            exit(0);
                    }else{
                        if(wait(0) == -1) perror("wait");
                        if(close(fd) == -1) perror("close");
                        char a[1] = {'>'};
                        argv[save] = a;
                        last_redir = x;    
                    }
                }
            }else if((argv[x][0] == '0' || argv[x][0] == '1' ||
                    argv[x][0] == '2' || argv[x][0] == '3' ||
                    argv[x][0] == '4' || argv[x][0] == '5'   ||
                    argv[x][0] == '6' || argv[x][0] == '7' || 
                    argv[x][0] == '8' || argv[x][0] == '9')&&  
                    string(argv[x]).find(">") > 0)
                {
                redirection = true;
                int iopos = string(argv[x]).find(">");
                int newfd = 0;
                for(int i = 0; i < iopos; ++i){
                    if(i == 0) newfd = argv[x][i] - 48;
                    else{
                        newfd += (argv[x][i] - 48) *10 *i;
                    }
                }
                fd = open(argv[x+1], O_RDWR | O_CREAT );
                if(fd < 0) perror("open");
                else{
                    save = x;
                    argv[x] = '\0';
                    if( (pid = fork()) == -1) perror("fork");
                    else if(pid == 0){
                        if(dup2(fd, newfd) == -1) perror("dup2");
                        if( strcmp(argv[last_redir], COMMANDS[0]) == 0) exit(1);
                        if( strcmp(argv[last_redir], COMMANDS[1]) == 0){
                            if(argv[last_redir + 1] != '\0'){
                                ch_cwdir(argv[last_redir + 1]);
                            }
                            else{
                                ch_cwdir();
                            }
                        }
                        if(execvp(argv[last_redir], argv) == -1)
                            perror("execvp");
                            exit(0);
                    }else{
                        if(wait(0) == -1) perror("wait");
                        if(close(fd) == -1) perror("close");
                        char a[1] = {'>'};
                        argv[save] = a;
                        last_redir = x;    
                    }
                }
            
           } else if(strcmp(argv[x], ">>") == 0){
                redirection = true;
                fd = open(argv[x+1], O_WRONLY | O_CREAT | O_APPEND);
                if(fd < 0) perror("open");
                else{
                    save = x;
                    argv[x] = '\0';
                    if( (pid = fork()) == -1) perror("fork");
                    else if(pid == 0){
                        if(dup2(fd, 1) == -1) perror("dup2");
                        if( strcmp(argv[last_redir], COMMANDS[0]) == 0) exit(1);
                        if( strcmp(argv[last_redir], COMMANDS[1]) == 0){
                            if(argv[last_redir + 1] != '\0'){
                                ch_cwdir(argv[last_redir + 1]);
                            }
                            else{
                                ch_cwdir();
                            }
                        }
                        if(execvp(argv[last_redir], argv) == -1)
                            perror("execvp");
                            exit(0);
                    }else{
                        if(wait(0) == -1) perror("wait");
                        if(close(fd) == -1) perror("close");
                        char a[1] = {'>'};
                        argv[save] = a;
                        last_redir = x;    
                    }
                }
            }
            else if(strcmp(argv[x], "|") == 0){
                int fds[2];
                char buf;
                save = x;
                argv[x] = '\0';
                if(pipe(fds) == -1) perror("pipe");

                //if( (fd = open(argv[x+1], O_RDWR | O_CREAT)) == -1)
                //        perror("open");
                if((pid = fork()) == -1) perror("fork");
                if(pid == 0){
                    if( close(fds[1]) == -1) perror("close");
                    int readfd;
                    while( (readfd = read(fds[0], &buf, 1)) >0){
                        if(readfd == -1) perror("read");
                        if(write(STDOUT_FILENO, &buf, 1) == -1)
                            perror("write");
                    }
                    if(write(STDOUT_FILENO, "\n", 1) == -1) perror("write");
                    if(close(fds[0]) == -1) perror("close");
                    exit(0);
                    //if(execvp(argv[last_redir], argv) == -1)
                    //     perror("execvp");
                }
                else{
                    if(close(fds[0]) == -1) perror("close");
                    if(write(fds[1], argv[x+1], BUFSIZ)== -1)
                         perror("write");
                    if(close(fds[1]) == -1) perror("close");
                    if(wait(0) == -1) perror("wait");
                    char a[1] = {'|'};
                    argv[save] = a;
                    last_redir = x;
                }
                
            }
       }
       if(redirection){
                
       } 
        
        
       else{
        /*argc = -1;
        token = strtok(andarr[it], tk);
        cmd = token;
        argv[++argc] = token;

            while(token != NULL){
//             cout << token << endl;
                token = strtok(NULL,tk);
                argv[++argc] = token;
            }
        argv[++argc] = '\0';*/
        pid = fork();
            if(pid == -1){
                perror("fork");
            }
            else if(pid == 0){
            //child 
                if( strcmp(cmd, "exit") == 0){ exit(2);}
                else if( strcmp(cmd, "cd") == 0){
                    exit(3);
                }
                else{
                    if( -1 == execvp(cmd,argv ) )
                        perror("execvp");
                }
                exit(1);
            }
            else{
                int stat_loc;
                if(waitpid(0,&stat_loc, 0) == -1){
                    perror("wait()");
                }
                if(stat_loc == 512) exit(1);
                if(stat_loc == 768){    
                    if(argv[1] != '\0'){
                        ch_cwdir(argv[1]);
                    }
                    else{
                        ch_cwdir();
                    }
                }
            }
       }
}
//---------------------------------------------------------------------------
        }
    }
        return 0;
    
}
