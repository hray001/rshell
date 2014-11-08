opt = -Wall -Werror -ansi -pedantic
all: cp rshell clean 
	mkdir ~/rshell/bin    
	mv rshell bin
	mv cp bin
cp: ./src/cp.cpp
	g++ $(opt) ./src/cp.cpp -o cp
rshell: ~/rshell/src/main.cpp
	g++ $(opt) ~/rshell/src/main.cpp -o rshell
clean: 
	rm -rf ~/rshell/bin
