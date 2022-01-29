#ifndef CONFIG_HPP
#define CONFIG_HPP

#include <string>
#include <vector>
#include "cppneofetch.hpp"

#define BLACK 	     "\e[1;30m"
#define RED 	     "\e[1;31m"
#define GREEEN 	     "\e[1;32m"
#define BROWN 	     "\e[1;33m"
#define BLUE 	     "\e[1;34m"
#define PURPLE 	     "\e[1;35m"
#define CYAN 	     "\e[1;36m"
#define LIGHT_GRAY   "\e[1;37m"
#define DARK_GRAY    "\e[1;100"
#define LIGHT_RED 	 "\e[1;101m"
#define LIGHT_GREEN  "\e[1;102m"
#define YELLOW 	     "\e[1;103m"
#define LIGHT_BLUE 	 "\e[1;104m"
#define LIGHT_PURPLE "\e[1;105m"
#define TEAL 	     "\e[1;106m"
#define WHITE 	     "\e[1;107m"

typedef struct {
    std::string name;
    std::string data;
    bool cached;
    bool show;
} info;

const info SPACE = {"", "", true, true};

static std::vector<info> CONFIG = {
    // Name          data                   cached  show
    {"",             get_userhost(),        true,   true},
    {"",             get_bar(),             true,   true},
    {"OS: ",         get_os(),              true,   true},
    {"Model: ",      get_model(),           true,   true},
    {"Kernel: ",     get_kernel(),          false,  true},
    {"Uptime: ",     get_uptime(),          false,  true},
    SPACE, 
    {"Packages: ",   get_packages_pacman(), false,  true},
    {"Shell: ",      get_shell(),           false,  true},
    {"Resolution: ", get_resolution(),      false,  true},
    {"Terminal: ",   get_terminal(),        false,  true},
    SPACE,      
    {"CPU: ",        get_cpu(),             true,   true},
    {"Memory: ",     get_memory(),          false,  true},
    SPACE,      
    {"",             get_colorbar(),        true,   true}
};

#endif
