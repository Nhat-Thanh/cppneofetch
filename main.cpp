#include "logos/garuda.hpp"
#include "cppneofetch.hpp"
#include "config.hpp"

#include <iostream>

int main(int argc, char** argv) {
    int len_logo = LOGO.size();
    int len_config = CONFIG.size();
    int limit = std::max(len_logo, len_config);

    int idx_logo = 0;
    int idx_config = 0;
    
    while (limit > 0) {
        
        if (idx_logo < len_logo) {
            std::cout << "\e[0m" << CYAN << LOGO[idx_logo];
            idx_logo += 1;
        } else {
            std::string spaces(LOGO[0].size(), ' ');
            std::cout << "\e[0m" << spaces;
        }
        
        if (idx_config < len_config && CONFIG[idx_config].show) {
            std::cout << CYAN << CONFIG[idx_config].name;
            std::cout << "\e[0m" << CONFIG[idx_config].data;
            idx_config += 1;
        }

        std::cout << '\n';
        limit -= 1;
    }
}
