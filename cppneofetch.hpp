#ifndef CPPNEOFETCH_HPP
#define CPPNEOFETCH_HPP

char* get_userhost();

char* get_bar();

char* get_os();

char* get_model();

char* get_kernel();

char* get_uptime();

char* get_packages(const char* dirname, const char* pacman_name);

char* get_packages_pacman();

char* get_packages_apt();

char* get_shell();

char* get_resolution();

char* get_terminal();

char* get_cpu();

char* get_memory();

char* get_colorbar();


#endif