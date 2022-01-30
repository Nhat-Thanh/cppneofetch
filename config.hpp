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
    bool show;
} info;

const info SPACE = {"", "", true};

static std::vector<info> CONFIG = {
    // Name          data                   show
    {"",             get_userhost(),        true},
    {"",             get_bar(),             true},
    {"OS: ",         get_os(),              true},
    {"Model: ",      get_model(),           true},
    {"Kernel: ",     get_kernel(),          true},
    {"Uptime: ",     get_uptime(),          true},
    SPACE, 
    // {"Packages: ",   get_packages_apt(),    true},
    {"Packages: ",   get_packages_pacman(), true},
    {"Shell: ",      get_shell(),           true},
    {"Resolution: ", get_resolution(),      true},
    {"Terminal: ",   get_terminal(),        true},
    SPACE,      
    {"CPU: ",        get_cpu(),             true},
    {"Memory: ",     get_memory(),          true},
    SPACE,      
    {"",             get_colorbar(),        true}
};

#endif
