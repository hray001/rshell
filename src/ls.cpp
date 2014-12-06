#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <dirent.h>
#include <errno.h>
#include <iostream>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <vector>
#include <algorithm>
#include <iomanip>
#include <time.h>
#include <pwd.h>
#include <grp.h>

using namespace std;

namespace { //string compare to ignore case
    struct ignoreCaseSort : public binary_function<char,char,bool>{
        bool operator() (char x, char y) const{
            return toupper(static_cast<unsigned char>(x)) < toupper(static_cast<unsigned char>(y));
        }
    };
    bool ignore_case_less(const string &a, const string &b){
        return lexicographical_compare(a.begin(),a.end(), b.begin(), b.end(), ignoreCaseSort() );
    }
}
static bool flag_a = false, flag_l = false, flag_R = false;
void output_sorted(vector<string> v){

        sort(v.begin(), v.end(), ignore_case_less);
        unsigned int format = 0;
        unsigned int size = v.size();
        unsigned int largest_file = 0;
        cout << fixed;
        for(unsigned i = 0; i < size; ++i){
            unsigned int cursize = v.at(i).size();
            if(cursize > largest_file)
                largest_file = cursize;
        }
        for(unsigned i = 0; i < size; ++i){
            unsigned int cursize = v.at(i).size();
            format += cursize;
            cout << left << setw(largest_file+1) << v.at(i);
            format+=(largest_file);
            if(i+1 < size && format+v.at(i+1).size() > 80){
                cout << endl;
                format = 0;
            }
        }
   cout << endl; 
}
void ls_noflag(dirent *direntp, vector<string> &v){

    if( direntp->d_name[0] != '.'){
        char *temp = direntp->d_name;
        string a = temp;
        v.push_back(a);
    }
}
void ls_R( char *r_arg){
    if(!flag_a){
    vector<string> v;
    char R_arg[4096][256];
    unsigned int numdirs = 0;
    DIR  *dirp;
    if( ( dirp = opendir(r_arg)) == NULL){perror("opendir"); return;} 
    dirent *direntp;
    while((direntp = readdir(dirp)) != NULL){
       if(direntp->d_name[0] != '.'){
       v.push_back(direntp->d_name);
       struct stat sb;
       char buffer[BUFSIZ];
       unsigned buffsize = 0;
       while(r_arg[buffsize] != '\0'){
           buffer[buffsize] = r_arg[buffsize];
           buffsize++;
       }
       buffer[buffsize] = '/';
       ++buffsize;
       unsigned k = 0;
       while(direntp->d_name[k] != '\0'){
            buffer[buffsize] = direntp->d_name[k];
            ++buffsize;
            ++k;
       }
       if(stat(buffer, &sb) == -1) perror("stat");
       if(S_ISDIR(sb.st_mode)){
           //cout << direntp->d_name << endl;
           for(unsigned x = 0; direntp->d_name[x] != '\0';++x)
           R_arg[numdirs][x] = direntp->d_name[x];
           numdirs++;
       }
        for(unsigned i = 0; i < buffsize; ++i){
            buffer[i] = '\0';
        }
       }
    }
    if( errno != 0) perror("readdir");
    if(direntp == NULL) perror("readdir");
    output_sorted(v);
    if(flag_a){
        cout << endl << ".:"<< endl;
        output_sorted(v);
    }
    if(numdirs >0)
        for(unsigned  i = 0; i < numdirs; ++i){
           // if( (R_arg[i][0] != '.' && R_arg[i][1] !='\0') &&
            //    ( R_arg[i][0] != '.' && R_arg[i][1] != '.' &&
            //    R_arg[i][2] != '\0' ) ){
                cout << R_arg[i] << ":" << endl;
                ls_R(R_arg[i]);
           // }    
        }
    }
}
int main(int argc, char** argv)
{
    struct stat sb;
    vector<int> arg;    
    for(int i = 1; i < argc; ++i){
        if(argv[i][0] == '-'){
            for(unsigned j = 1; argv[i][j] != '\0'; ++j){
                if(argv[i][j] == 'a'){
                    flag_a = true;
                }
                else if(argv[i][j] == 'l'){
                    flag_l =  true;
                }
                else if(argv[i][j] == 'R'){
                    flag_R = true;
                }
                else{
                    cout << "Unsupported flag: " << argv[i][j] << endl;
                }
            }
        }
        else{
            arg.push_back(i);
        }
    }
    if(arg.size() == 0){

    }
    
    //for(unsigned i = 0; i < arg.size(); ++i){
    //    cout << arg[i]<<endl;
    //}
    for(unsigned i = 0; i < arg.size(); ++i){
    //-----------------------------------------------------------------
    if(arg.size() > 1){
        if(i > 0){
            cout << endl;
        }
        cout << argv[arg[i]] << ":" << endl;
    }
    char dirName[4096];
    string dot = ".";
    strcpy(dirName, dot.c_str() );
    DIR *dirp;
    if ( (dirp = opendir(argv[arg[i]]))  == NULL){
        perror("opendir");
        exit(1);
    }
    //cout << "open succeeded" << endl;
    dirent *direntp;
    vector< string > v;
    while ((direntp = readdir(dirp)) != NULL){
        char buf[BUFSIZ];
        unsigned bufsize = 0;
        while(argv[arg[i]][bufsize] != '\0'){
            buf[bufsize] = argv[arg[i]][bufsize];
            bufsize++;
        }
        buf[bufsize] = '/';
        ++bufsize;
        unsigned k = 0;
        while(direntp->d_name[k] != '\0'){
            buf[bufsize] = direntp->d_name[k];
            ++bufsize;
            ++k;
        }
        if( errno != 0) perror("readdir");
        //sprintf(buf, "%s/%s", argv[arg[i]], direntp->d_name);
        //cout << endl << endl;
        if(flag_R){
            cout << argv[arg[i]] << ":" << endl;
            ls_R(argv[arg[i]]);
            break;
            /*if(!flag_a){
                
                if( direntp->d_name[0] != '.'){
                char *temp = direntp->d_name;
                string a = temp;
                v.push_back(a); // use stat here to find attributes of file
                }
                if(stat(buf, &sb) == -1){
                    perror("stat");
                    exit(1);
                }
               // cout << "direntp: " << direntp->d_name<< endl;
                if(S_ISDIR(sb.st_mode) )
                 //   cout << " is dir" << endl;
               
            }
            else if(flag_a){

            }
*/            
        }
        else if(flag_l){
                if(!flag_a && direntp->d_name[0] != '.'){
                    char *temp = direntp->d_name;
                    string a = temp;
                    v.push_back(a); 
                    if(stat(buf, &sb) == -1){
                        perror("stat");
                        exit(1);
                    }
                    char perm[10];
                    time_t t= sb.st_mtime;
                    struct tm ts;
                    localtime_r(&t, &ts);
                    char timebuf[80];
                    errno = 0;
                    struct passwd *getpwuid(uid_t uid);
                    if(errno != 0) perror("*getpwuid");
                    char *ID = getpwuid(sb.st_uid)->pw_name;
                    if(ID == NULL) perror("getpwuid");
                    errno = 0;
                    struct group *getgrgid(gid_t gid);
                    if(errno != 0) perror("*getgrgid");
                    char *GID = getgrgid(sb.st_gid)->gr_name;
                    if(GID == NULL) perror("getgrgid");

                    strftime(timebuf, sizeof(timebuf), "%b %d %I:%M", &ts);
                    for(unsigned i = 0; i < 10; ++i) perm[i] = '-';
                    if(S_ISDIR(sb.st_mode)) perm[0] = 'd';
                    if(S_ISCHR(sb.st_mode)) perm[0] = 'c';
                    if(S_ISBLK(sb.st_mode)) perm[0] = 'b';
                    
                    if(sb.st_mode & S_IRUSR ) perm[1] = 'r';
                    if(sb.st_mode & S_IWUSR ) perm[2] = 'w';
                    if(sb.st_mode & S_IXUSR ) perm[3] = 'x';
                    
                    if(sb.st_mode & S_IRGRP ) perm[4] = 'r';
                    if(sb.st_mode & S_IWGRP ) perm[5] = 'w';
                    if(sb.st_mode & S_IXGRP ) perm[6] = 'x';
                    
                    if(sb.st_mode & S_IROTH ) perm[7] = 'r';
                    if(sb.st_mode & S_IWOTH ) perm[8] = 'w';
                    if(sb.st_mode & S_IXOTH ) perm[9] = 'x';
                    string s_perm;
                    for(unsigned i = 0; i < 10; ++i) s_perm+=perm[i];
                     
                    cout << s_perm << " " << sb.st_nlink << " " 
                         << ID << " " << GID
                         << " " << sb.st_size << " " 
                         << timebuf  << " " << direntp->d_name
                         << endl;
                }
            else if(flag_a){
                    char *temp = direntp->d_name;
                    string a = temp;
                    v.push_back(a); 
                    if(stat(buf, &sb) == -1){
                        perror("stat");
                        exit(1);
                    }
                    
                    char perm[10];
                    
                    time_t t= sb.st_mtime;
                    struct tm ts;
                    localtime_r(&t, &ts);
                    char timebuf[80];

                    struct passwd *getpwuid(uid_t uid);
                    char *ID = getpwuid(sb.st_uid)->pw_name;
                    if(ID == NULL) perror("getpwuid");
                    struct group *getgrgid(gid_t gid);
                    char *GID = getgrgid(sb.st_gid)->gr_name;
                    if(GID == NULL) perror("getgrgid");

                    strftime(timebuf, sizeof(timebuf), "%b %d %I:%M", &ts);
                    for(unsigned i = 0; i < 10; ++i) perm[i] = '-';
                    if(S_ISDIR(sb.st_mode)) perm[0] = 'd';
                    if(S_ISCHR(sb.st_mode)) perm[0] = 'c';
                    if(S_ISBLK(sb.st_mode)) perm[0] = 'b';
                    
                    if(sb.st_mode & S_IRUSR ) perm[1] = 'r';
                    if(sb.st_mode & S_IWUSR ) perm[2] = 'w';
                    if(sb.st_mode & S_IXUSR ) perm[3] = 'x';
                    
                    if(sb.st_mode & S_IRGRP ) perm[4] = 'r';
                    if(sb.st_mode & S_IWGRP ) perm[5] = 'w';
                    if(sb.st_mode & S_IXGRP ) perm[6] = 'x';
                    
                    if(sb.st_mode & S_IROTH ) perm[7] = 'r';
                    if(sb.st_mode & S_IWOTH ) perm[8] = 'w';
                    if(sb.st_mode & S_IXOTH ) perm[9] = 'x';
                    string s_perm;
                    for(unsigned i = 0; i < 10; ++i) s_perm+=perm[i];
                     
                    cout << s_perm << " " << sb.st_nlink << " " 
                         << ID << " " << GID
                         << " " << sb.st_size << " " 
                         <</* timebuf  <<*/ " " << direntp->d_name
                         << endl;
 
            }
        }
        //--------------------------------------------------------
        else if(!flag_l && !flag_R && !flag_a){
          ls_noflag(direntp, v);
          /* if( direntp->d_name[0] != '.'){
            char *temp = direntp->d_name;
            string a = temp;
            v.push_back(a); // use stat here to find attributes of file

           if(stat(direntp->d_name, &sb) == -1){
                perror("stat");
                exit(1);
            }
            }*/
        }
        else if(flag_a && !flag_l && !flag_R){
            char *temp = direntp->d_name;
            string a = temp;
            v.push_back(a); // use stat here to find attributes of file
        }
        //----------------------------------------------------------
        for(unsigned i = 0; i < bufsize; ++i){
            buf[i] = '\0';
        }
    }
    if(errno != 0) perror("readdir");
    if(!flag_l && !flag_R){
        output_sorted(v);
        /*sort(v.begin(), v.end(), ignore_case_less);
        unsigned int format = 0;
        unsigned int size = v.size();
        unsigned int largest_file = 0;
        cout << fixed;
        for(unsigned i = 0; i < size; ++i){
            unsigned int cursize = v.at(i).size();
            if(cursize > largest_file)
                largest_file = cursize;
        }
        for(unsigned i = 0; i < size; ++i){
            unsigned int cursize = v.at(i).size();
            format += cursize;
            cout << left << setw(largest_file+1) << v.at(i);
            format+=(largest_file);
            if(i+1 < size && format+v.at(i+1).size() > 80){
                cout << endl;
                format = 0;
            }
        }
        */
    }
       if( closedir(dirp) == -1) perror("closedir");
    }
}
