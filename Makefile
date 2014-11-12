opt = -Wall -Werror -ansi -pedantic
all: cp rshell ls clean 
	mkdir ./bin    
	mv rshell bin
	mv cp bin
	mv ls bin
cp: ./src/cp.cpp
	g++ $(opt) ./src/cp.cpp -o cp
ls: ./src/ls.cpp
	g++ $(opt) ./src/ls.cpp -o ls
rshell: ./src/main.cpp
	g++ $(opt) ./src/main.cpp -o rshell
clean: 
	rm -rf ./bin
