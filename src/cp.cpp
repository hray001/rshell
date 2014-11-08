#include <iostream>
#include <fstream>
#include <string.h>
#include <stdio.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

#include "Timer.h"

using namespace std;

int main(int argc, char** argv)
{    
    int arr[2];
    int x = 0;
    char* arg_1;
    char* arg_2;
    bool print_time = 0;
    
    // checks for valid flags
    for(int i = 1; i < argc; i++)
    {
        if(argv[i][0] == '-')
        {
            for(unsigned j = 1; j < strlen(argv[i]); j++)
            {
                if(argv[i][j] == 't')
                    print_time = 1;
                else
                {
                    cerr << "Error: Unsupported argument: " << argv[i][j] << endl;
                    cerr << "Valid arguments are: \n\t-t" << endl;
                }
            }

        }
        else
        {
            arr[x] = i;
            x++;
        }
    }

    if(x > 2)
    {
        cerr << "Error: Too many arguments. Only input and output files required." << endl;
        exit(1);
    }
    else if(x < 1 || argc <= 2)
    {
        cerr << "Error: Too few arguments. Input and output files required." << endl;
        exit(1);
    }
    else
    {
        arg_1 = argv[arr[0]];
        arg_2 = argv[arr[1]];
    }

    Timer t;
    
    // Code below tests to see if the first argument is a directory
    // Outputs an error if it is
    // FIXME: The condition may need to be changed in conjunction with
    // the above requirements of flags being able to be placed anywhere
    struct stat dir_test;
    if(stat(arg_1, &dir_test) == -1)
        perror("stat");
    if(S_ISDIR(dir_test.st_mode))
    {
        cerr << "Error: Directory passed in." << endl;
        exit(1);
    }

    // FIXME: Need to check to see if the file being written to
    // exists or not. Output an error if the second file exists.

    // Use the in.get(char) and out.put(char) C++ functions
    // to copy the input file one character at a time.
    
    if(print_time)
    {
        double wall1, user1, system1;
        t.start();

        ifstream in(arg_1);

        // Check if file exists
        fstream check;
        check.open(arg_2);
        if(check.is_open())
        {
            cerr << "Error: Output file already exists." << endl;
            exit(1);
        }

        ofstream out(arg_2);
        char a;
        while(in.good())
        {
            a = in.get();
            if(in.good())
                out.put(a);
        }

        in.close();

        t.elapsedWallclockTime(wall1);
        t.elapsedUserTime(user1);
        t.elapsedSystemTime(system1);
        cout << "\nFunction 1" << endl;
        cout << "Wall Clock: " << wall1 << endl;
        cout << "User Time: " << user1 << endl;
        cout << "System Time: " << system1 << endl << endl;
    }
    // Use the Unix system calls read() and write() to copy the
    // input file to the output file one character at a time.

    // ssize_t read(int fd, void *buf, size_t count);
    // Make sure to use error checking since this is a system call.
    if(print_time)
    {
        double wall2, user2, system2;
        t.start();

        int file1 = open(arg_1, O_RDONLY);
        
        int file2 = 0;
        if(file1 == -1)
        {
            perror("open");
        }
        if(print_time)
        {
            file2 = open(arg_2, O_WRONLY | O_CREAT);
        }    
        else
        {
            file2 = open(arg_2, O_WRONLY | O_CREAT | O_EXCL);
        }

        if(file2 == -1)
        {
            perror("open");
            exit(1);
        }
        else
        {
            char buffer[1];

            int bytes = read(file1, buffer, 1);
            while(bytes > 0)
            {
                if(write(file2, buffer, 1) == -1)
                    perror("write");
                bytes = read(file1, buffer, 1);
            }

            if(bytes == -1)
                perror("read");
        }
        int close1 = close(file1);
        if(close1 == -1)
            perror("close");

        int close2 = close(file2);
        if(close2 == -1)
            perror("close");

        t.elapsedWallclockTime(wall2);
        t.elapsedUserTime(user2);
        t.elapsedSystemTime(system2);
        cout << "Function 2" << endl;
        cout << "Wall Clock: " << wall2 << endl;
        cout << "User Time: " << user2 << endl;
        cout << "System Time: " << system2 << endl << endl;
    }

    // Use the Unix system calls read() and write() to copy the input
    // file one buffer at a time. The buffer should be of size 
    // BUFSIZ, which is declared in the stdio.h include file.
    double wall3, user3, system3;
    t.start();

    char buffer2[BUFSIZ];
    int file_a = open(arg_1, O_RDONLY);
    if(file_a == -1)
    {
        perror("open");
    }

    int file_b = 0;
    if(print_time)
    {
        file_b = open(arg_2, O_WRONLY | O_CREAT);
    }    
    else
    {
        file_b = open(arg_2, O_WRONLY | O_CREAT | O_EXCL);
    }
    if(file_b == -1)
    {
        perror("open");
        exit(1);
    }

    int buf = read(file_a, buffer2, BUFSIZ);
    if(buf == -1)
        perror("read");
    if(write(file_b, buffer2, buf) == -1)
        perror("write");
    
    int close_a = close(file_a);
    if(close_a == -1)
        perror("close");

    int close_b = close(file_b);
    if(close_b == -1)
        perror("close");

    if(print_time)
    {
        t.elapsedWallclockTime(wall3);
        t.elapsedUserTime(user3);
        t.elapsedSystemTime(system3);
        cout << "Function 3" << endl;
        cout << "Wall Clock: " << wall3 << endl;
        cout << "User Time: " << user3 << endl;
        cout << "System Time: " << system3 << endl << endl;
    }

    return 0;
}

