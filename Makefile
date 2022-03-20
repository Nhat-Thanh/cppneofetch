PREFIX := ~/.local
build:
	g++ main.cpp			\
		cppneofetch.cpp 	\
		cppneofetch.hpp 	\
		config.hpp 			\
		-Ilogos				\
		-lX11 				\
		-O2 -Wall -Wextra  	\
		-o cppneofetch 
	strip cppneofetch


debug:
	g++ -g main.cpp			\
		cppneofetch.cpp 	\
		cppneofetch.hpp 	\
		config.hpp 			\
		-Ilogos				\
		-lX11 				\
		-O2 -Wall -Wextra  	\
		-o main 

run:
	./cppneofetch

install: build
	mkdir -p $(PREFIX)/bin
	install ./cppneofetch $(PREFIX)/bin/cppneofetch
	
