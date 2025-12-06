PREFIX ?= "/usr"
LINUX_BASE ?= "ARCH_BASE"
build:
	g++ main.cpp			\
		cppneofetch.cpp 	\
		cppneofetch.hpp 	\
		config.hpp 			\
		-Ilogos				\
		-lX11 				\
		-D${LINUX_BASE}  	\
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
		-D${LINUX_BASE}  	\
		-Wall -Wextra 		\
		-o cppneofetch

run:
	./cppneofetch

install: build
	mkdir -p $(PREFIX)/bin
	install ./cppneofetch $(PREFIX)/bin/cppneofetch
	