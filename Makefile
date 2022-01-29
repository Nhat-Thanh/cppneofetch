
build:
	g++ main.cpp			\
		cppneofetch.cpp 	\
		cppneofetch.hpp 	\
		config.hpp 			\
		-Ilogos				\
		-lX11 				\
		-O2 -Wall -Wextra  	\
		-o cppneofetch 


debug:
	g++ -g main.cpp			\
		cppneofetch.cpp 	\
		cppneofetch.hpp 	\
		config.hpp 			\
		-Ilogos				\
		-lX11 				\
		-O2 -Wall -Wextra  	\
		-o cppneofetch 

run:
	./cppneofetch 