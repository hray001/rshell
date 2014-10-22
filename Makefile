opt = -Wall -Werror -ansi -pedantic
all: rshell
            
rshell: ~/rshell/src/main.cpp
	g++ $(opt) ~/rshell/src/main.cpp -o rshell
	mkdir ~/rshell/bin
	mv rshell bin
clean: 
	rm -rf ~/rshell/bin
