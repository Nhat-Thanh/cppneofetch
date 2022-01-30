#include <iostream>
#include <string.h>
#include <unistd.h>
#include <dirent.h>

#include <sys/utsname.h>
#include <sys/sysinfo.h>

#include <X11/Xlib.h>
#include <X11/Xatom.h>

#include "cppneofetch.hpp"
#include "config.hpp"

#define BUFFER_SIZE 150

size_t userhost_len = 0;

void check_status(const int& status, 
                  const char* error_mess) {
    if (status != 0) {
        std::cerr << error_mess;
        exit(status);
    }
}

void remove_substr(char* str, char* substr, const int& len) {
    char* sub = strstr(str, substr);
    if (sub == NULL)
        return;
    
    while (*(sub + len) != '\0') {
        *sub = *(sub + len);
        sub++;
    }
    *sub = '\0';
}

void remove_endline(char* str) {
    char* pstr = str;
    while (*pstr != '\0' && *pstr != '\n')
        pstr++;
    *pstr = '\0';
}

void replace_substring(char *str, 
                       const char *sub_str, 
                       const char *repl_str, 
                       const size_t& sub_len, 
                       const size_t& repl_len) {
    char buffer[BUFFER_SIZE / 2];
    char *start = strstr(str, sub_str);
    if (start == NULL) 
        return;

    /* check if we have enough space for new substring */
    if (strlen(str) - sub_len + repl_len >= BUFFER_SIZE / 2) {
        check_status(-1, "new substring too long to replace");
    }

    strcpy(buffer, start + sub_len);
    strncpy(start, repl_str, repl_len);
    strcpy(start + repl_len, buffer);
}

char* get_userhost() {
    int status;
    char host_name[BUFFER_SIZE / 3];
    status = gethostname(host_name, BUFFER_SIZE / 3);
    check_status(status, "Unable to retrive host name\n");

    char user_name[BUFFER_SIZE / 3];
    status = getlogin_r(user_name, BUFFER_SIZE / 3);
    check_status(status, "Unable to retrive user name\n");

    userhost_len = strlen(host_name) + strlen(user_name) + 1;
    
    /* user_name@host_name */
    char* userhost = (char*)malloc(BUFFER_SIZE);
    snprintf(userhost, BUFFER_SIZE, CYAN"%s\e[0m@" CYAN"%s", user_name, host_name);
    
    return userhost;        
}

char* get_bar() {
    char* bar = (char*)malloc(BUFFER_SIZE);
    for (int i = 0; i < userhost_len; ++i) 
        bar[i] = '-';
    bar[userhost_len] = '\0';
    return bar;    
}

char* get_os() {
    char* os = (char*)malloc(BUFFER_SIZE);
    char* name = (char*)malloc(BUFFER_SIZE);
    char* line = NULL;
    size_t len;

    FILE* ifs = fopen("/etc/os-release", "r");
    if (ifs != NULL) {
        while (getline(&line, &len, ifs) != -1)
            // if match the format, sscanf will return a number greater then 0
            if (sscanf(line, "PRETTY_NAME=\"%[^\"]", name) > 0) 
                break;

    } else 
        check_status(-1, "unable to open /etc/os-release or file does not exist\n");

    snprintf(os, BUFFER_SIZE, "%s", name);
    
    free(name);
    free(line);
    fclose(ifs);
    
    return os;
}

char* get_model() {
    char* model = (char*)malloc(BUFFER_SIZE);
    char* line = NULL;

    size_t len;
    FILE* ifs = fopen("/sys/devices/virtual/dmi/id/product_name", "r");
    if (ifs != NULL) {
        getline(&model, &len, ifs);
        remove_endline(model);
    } else {
        ifs = fopen("/sys/firmware/devicetree/base/model", "r");
        getline(&model, &len, ifs);
        remove_endline(model);
    }

    free(line);
    fclose(ifs);

    return model;
}

char* get_kernel() {
    struct utsname uname_info;
    uname(&uname_info);
    char* kernel = (char*)malloc(BUFFER_SIZE);
    strncpy(kernel, uname_info.release, BUFFER_SIZE);
    return kernel;
}

char* get_uptime() {
    struct sysinfo system_info;
    sysinfo(&system_info);

    char* uptime = (char*)malloc(BUFFER_SIZE);
    long seconds = system_info.uptime;
    
    struct { const char *name; int seconds; } units[] = {
        { "day",  60 * 60 * 24 },
        { "hour", 60 * 60 },
        { "min",  60 }
    };

    int n , len = 0;
    for (int i = 0; i < 3; ++i) {
        n = seconds / units[i].seconds;
        if (n != 0 || i == 2) {
            len += snprintf(uptime + len, BUFFER_SIZE, "%d %s%s", 
                            n, units[i].name, (n > 1) ? "s " : " ");
            seconds %= units[i].seconds;
        }
    }
    uptime[len - 1] = '\0';
    
    return uptime;
}

char* get_packages(const char* dirname, const char* pacman_name) {
    int num_packages = 0;
    DIR* pDir;

    pDir = opendir(dirname);
    if (pDir == NULL)
        check_status(-1, "You must locate the true dirname");
    
    struct dirent* entry = readdir(pDir);    
    while (entry != NULL) {
        if (entry->d_type == DT_DIR)
            num_packages += 1;
        entry = readdir(pDir);
    }

    num_packages -= 2; // accounting for . and ..
    closedir(pDir);
    
    char* packages = (char*)malloc(BUFFER_SIZE);
    snprintf(packages, BUFFER_SIZE, "%d (%s)", num_packages, pacman_name);
    return packages;
}

char* get_packages_pacman() {
    return get_packages("/var/lib/pacman/local", "pacman");
}

char* get_packages_apt() {
    int num_packages = 0;

    FILE *proc = popen("dpkg -l | grep -c '^ii'", "r");
    fscanf(proc, "%d", &num_packages);

    char *packages = (char*)malloc(BUFFER_SIZE);
    snprintf(packages, BUFFER_SIZE, "%d (%s)", num_packages, "dpkg");
    return packages;
}

char* get_shell() {
    char* shell = (char*)malloc(BUFFER_SIZE);
    char* shell_path = getenv("SHELL");
    char* shell_name = strrchr(getenv("SHELL"), '/');

    if (shell_name == NULL) 
        strncpy(shell, shell_path, BUFFER_SIZE);
    else 
        strncpy(shell, shell_name + 1, BUFFER_SIZE);
    
    return shell;
}

char* get_resolution() {
    Display* display = XOpenDisplay(NULL);
    if (display == NULL)
        check_status(-1, "unable to get resolution");
    
    int screen, h, w;
    screen = DefaultScreen(display);
    h = DisplayHeight(display, screen);
    w = DisplayWidth(display, screen);

    char* resolution = (char*)malloc(BUFFER_SIZE);
    snprintf(resolution, BUFFER_SIZE, "%dx%d", w, h);
    return resolution;

}

// I copied from https://github.com/ss7m/paleofetch/blob/master/paleofetch.c
char* get_terminal() {
 unsigned char *prop;
    char *terminal = (char*)malloc(BUFFER_SIZE);
    Display* display = XOpenDisplay(NULL);

    /* check if xserver is running or if we are running in a straight tty */
    if (display != NULL) {   

    unsigned long _, // not unused, but we don't need the results
                  window = RootWindow(display, XDefaultScreen(display));    
        Atom a,
             active = XInternAtom(display, "_NET_ACTIVE_WINDOW", True),
             __class = XInternAtom(display, "WM_CLASS", True);

#define GetProp(property) \
        XGetWindowProperty(display, window, property, 0, 64, 0, 0, &a, (int *)&_, &_, &_, &prop);

        GetProp(active);
        window = (prop[3] << 24) + (prop[2] << 16) + (prop[1] << 8) + prop[0];
        free(prop);
        if(!window) goto terminal_fallback;
        GetProp(__class);

#undef GetProp

        snprintf(terminal, BUFFER_SIZE, "%s", prop);
        free(prop);
    } else {
terminal_fallback:
        strncpy(terminal, getenv("TERM"), BUFFER_SIZE); /* fallback to old method */
        /* in tty, $TERM is simply returned as "linux"; in this case get actual tty name */
        if (strcmp(terminal, "linux") == 0) {
            strncpy(terminal, ttyname(STDIN_FILENO), BUFFER_SIZE);
        }
    }

    return terminal;
}

char* get_cpu() {
    FILE* ifs = fopen("/proc/cpuinfo", "r");
    if (ifs == NULL)
        check_status(-1, "unabel to open /proc/cpuinfo");
    
    char* cpu_name = (char*)malloc(BUFFER_SIZE);
    char* max_freq = (char*)malloc(BUFFER_SIZE);
    int num_cores = 0;
    char* line;
    size_t len;
    while (getline(&line, &len, ifs) != -1) 
        num_cores += sscanf(line, "model name   : %[^\n@]", cpu_name);
    
    free(line);
    line = NULL;
    fclose(ifs);

    ifs = fopen("/sys/devices/system/cpu/cpu0/cpufreq/cpuinfo_max_freq", "r");
    if (ifs == NULL)
        check_status(-1, "unabel to open /sys/devices/system/cpu/cpu0/cpufreq/cpuinfo_max_freq");
    
    int freq = 0;
    if (getline(&line, &len, ifs) != -1) {
        sscanf(line, "%d", &freq);
        snprintf(max_freq, BUFFER_SIZE, "%.2f%s", float(freq) / 1000000, "GHz");
    } else {
        snprintf(max_freq, BUFFER_SIZE, "%s", "");
        fclose(ifs);
        free(line);
    }
    

    char* cpu = (char*)malloc(BUFFER_SIZE);
    snprintf(cpu, BUFFER_SIZE, "%s(%d) %s", cpu_name, num_cores, max_freq);
    free(cpu_name);
    free(max_freq);

    remove_substr(cpu, "(R)", 3);
    remove_substr(cpu, "(TM)", 4);
    remove_substr(cpu, "Dual-Core", 9);
    remove_substr(cpu, "Qual-Core", 9);
    remove_substr(cpu, "Six-Core", 8);
    remove_substr(cpu, "Eight-Core", 10);
    remove_substr(cpu, "Core", 4);
    remove_substr(cpu, "CPU", 3);
    replace_substring(cpu, "  ", " ", 2, 1);

    return cpu;
}

char* get_memory() {
    int total_memory, used_memory;
    int total, shared, memfree, buffers, cached, reclaimable;

    FILE *ifs = fopen("/proc/meminfo", "r");
    if(ifs == NULL)
        check_status(-1, "Unable to open meminfo");

    char *line = NULL;
    size_t len;

    while (getline(&line, &len, ifs) != -1) {
        sscanf(line, "MemTotal: %d", &total);
        sscanf(line, "Shmem: %d", &shared);
        sscanf(line, "MemFree: %d", &memfree);
        sscanf(line, "Buffers: %d", &buffers);
        sscanf(line, "Cached: %d", &cached);
        sscanf(line, "SReclaimable: %d", &reclaimable);
    }

    free(line);
    fclose(ifs);

    used_memory = (total + shared - memfree - buffers - cached - reclaimable) / 1024;
    total_memory = total / 1024;
    int percentage = (int) (100 * (used_memory / (double) total_memory));

    char *memory = (char*)malloc(BUFFER_SIZE);
    snprintf(memory, BUFFER_SIZE, "%dMiB / %dMiB (%d%%)", used_memory, total_memory, percentage);

    return memory;
}

char* get_colorbar() {
    char* colorbar = (char*)malloc(BUFFER_SIZE);
    char *s = colorbar;

    for(int i = 8; i < 16; i++) {
        sprintf(s, "\e[48;5;%dm   ", i);
        s += 12 + (i >= 10 ? 1 : 0);
    }
    snprintf(s, 5, "\e[0m");

    return colorbar;
}
