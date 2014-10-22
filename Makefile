opt = -Wall -Werror -ansi -pedantic
all: rshell clean 
	mkdir ~/rshell/bin    
	mv rshell bin
rshell: ~/rshell/src/main.cpp
	g++ $(opt) ~/rshell/src/main.cpp -o rshell
clean: 
	rm -rf ~/rshell/bin
