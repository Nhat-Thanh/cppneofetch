#include "CTerminalInfo.hpp"
#include "CStringHelper.hpp"
#include "CFileReader.hpp"
#include "Definitions.hpp"
#include <unistd.h>
#include <climits>

#define PID_INIT 1

CTerminalInfo::CTerminalInfo() {
}

std::string CTerminalInfo::fstr_GetName() const {
    if (mstr_Name.empty()) {
        std::string str_Line;
        std::string str_Path;
        int i32_Pid = getpid();
        int i32_PPid = getppid();
        int i32_TerminalPid = i32_Pid;
        char c_State = CHAR_ENDSTR;
        char ac_Process[NAME_MAX];
        /*
        For instance, open "gnome-terminal" which runs "fish" shell, an user types "bash" to open bash shell afterwards.
        We will trace back until the 2nd row below, then get process name of "its child process"

        Path                 i32_Pid  ac_process         c_State  i32_PPid  ....
        /proc/1/stat      -> 1        (systemd)          S        0         ....
        /proc/981/stat    -> 981      (systemd)          S        1         ....
        /proc/12952/stat  -> 12952    (gnome-terminal-)  S        981       ....
        /proc/325461/stat -> 325461   (fish)             S        12952     ....
        /proc/695775/stat -> 695775   (bash)             S        325461    ....
        /proc/747131/stat -> 747131   (our binary)       S        695775    ....
         */
        while (PID_INIT != i32_PPid) {
            i32_TerminalPid = i32_Pid; // Save the PID of child process
            CStringHelper::fv_Format(str_Path, PATH_PROC_STAT_FORMAT, i32_PPid);
            if (EXIT_SUCCESS == CFileReader::fi64_GetLine_nth(str_Line, str_Path)) {
                sscanf(str_Line.c_str(), "%d (%[^)]) %c %d", &i32_Pid, ac_Process, &c_State, &i32_PPid);
            } else {
                break;
            }
        }
        if (PID_INIT == i32_PPid) {
            CStringHelper::fv_Format(str_Path, PATH_PROC_COMM_FORMAT, i32_TerminalPid);
            if (EXIT_SUCCESS == CFileReader::fi64_GetLine_nth(mstr_Name, str_Path)) {
                CStringHelper::fv_RightTrim(mstr_Name, '-');
            }
        }
    }
    return mstr_Name;
}